#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "output.h"

typedef struct {

	int cols[16];
	char *vals[256][16];
	int align[16];
	char fmts[16][8];

	int ncols;
	int nrows;

	int colindex;
	int colsum;
} prtable;

prtable* prtable_init(int ncols) {
	prtable* prt = malloc(sizeof(prtable));
	memset(prt, 0, sizeof(prtable));

	prt->ncols = ncols;

	return prt;
}
void prtable_free(prtable* prt) {
	int i, j;
	for (j = 0; j < prt->nrows; j++) {
		for (i = 0; i < prt->ncols; i++) {
			free(prt->vals[j][i]);
		}
	}
	free(prt);
}
int strlen_ml(const char *str) {
	char *ctx;
	char *s = strdup(str);
	char *tok = strtok_r(s, "\n", &ctx);
	int maxfound = 0;
	while (tok) {
		int n = strlen(tok);
		if (n > maxfound) maxfound = n;
		tok = strtok_r(NULL, "\n", &ctx);
	}
	free(s);
	return maxfound;
}
void prtable_col(prtable* prt, const char *str) {
	if (prt->nrows >= 255) {
		fprintf(stderr, "PRTable is overflowing with rows, ignoring `%s`\n", str);
	}
	int len = strlen_ml(str) + 2;
	if (len > prt->cols[prt->colindex]) {
		prt->cols[prt->colindex] = len;
	}
	prt->vals[prt->nrows][prt->colindex++] = strdup(len == 2 ? " " : str);
	if (prt->colindex >= prt->ncols) {
		prt->colindex = 0;
		prt->nrows++;
	}
}
int prtable_sum(prtable* prt) {
	int i, t = 0;
	for (i = 0; i < prt->ncols; i++) {
		t += prt->cols[i]+1;
		sprintf(prt->fmts[i], "%%%s%ds",
			prt->align[i] <= 0 ? "-" : "",
			prt->cols[i] - 2
		);
		//printf("new format for col %d = %s\n", i, prt->fmts[i]);
	}
	prt->colsum = t;
	return t;
}
void prtable_pcol(prtable* prt, int i, char *txt) {

	if (prt->align[i] == 0) {

		int len = strlen(txt);
		int left = prt->cols[i] - len - 2;
		int half = left / 2;
		int otherhalf = left - half;
		int k;
		for (k = 0; k < half; k++) {
			putchar(' ');
		}
		printf("%s", txt);
		for (k = 0; k < otherhalf; k++) {
			putchar(' ');
		}
		return;
	}

	printf(prt->fmts[i], txt);

}
void prtable_row(prtable* prt, int j) {
	int i, c = 0, k;
	for (i = 0; i < prt->ncols; i++) {
		char *line = strdup(prt->vals[j][i]);
		char *start = strtok(line, "\n");
		int first = 1;
		while (start) {
			//printf("\nLS %d:{{{ %s }}}\n", first, start);
			if (i) putchar(' ');
			if (first) {
				printf("| ");

				prtable_pcol(prt, i, start);

			} else {
				printf("|\n|");
				int l;
				for (l = 0; l < i; l++) {
					for (k = 0; k < prt->cols[l]; k++) {
						putchar(' ');
					}
					printf("|");
				}
				putchar(' ');
				prtable_pcol(prt, i, start);

				for (l = i+1; l < prt->ncols; l++) {
					for (k = 0; k < prt->cols[l]; k++) {
						putchar(' ');
					}
					printf("|");
				}
			}
			start = strtok(NULL, "\n");
			first = 0;
		}
		free(line);
		c += prt->cols[i];
	}
	printf(" |\n");
}
void prtable_rows(prtable* prt, int start) {
	int j;
	for (j = start; j < prt->nrows; j++) {
		prtable_row(prt, j);
	}
}
void prtable_line(prtable* prt) {
	int i, j, k;
	for (j = 0; j < prt->ncols; j++) {
		printf("|");
		k = 1;
		if (prt->align[j] <= 0) {
			printf("%c", ':');
		}
		if (prt->align[j] == 0) k++;
		for (i = 0; i < prt->cols[j] - k; i++)
			printf("%c", '-');
		if (prt->align[j] >= 0)
			printf("%c", ':');
	}
	printf("|\n");
}
void prtable_print(prtable* prt) {
	prtable_sum(prt);

	prtable_row(prt, 0);
	prtable_line(prt);
	prtable_rows(prt, 1);
	//prtable_line(prt);
}


void jsonc_print_md_struct(const JSONC_Struct *obj) {
	printf("##  %s\n", obj->name ? obj->name : "");
	putchar('\n');
	printf("%s\n", obj->description ? obj->description : "");

	char buf[1024];
	prtable *prt = prtable_init(3);

	JSONC_Member *member;
	printf("\n");
	prtable_col(prt, "Member");
	prtable_col(prt, "Type");
	prtable_col(prt, "Description");
	prt->align[0] = prt->align[1] = prt->align[2] = -1;
	prt->align[0] = 1;
	//printf("| Member | Type | Description |\n");
	//printf("|--------|------|-------------|\n");
	for (member = obj->head; member; member = member->next) {
		//printf("| ");
		//printf("%s ", member->name);
		//printf("| ");
		prtable_col(prt, member->name);
		if (member->value->jutype == JSONC_ARRAY) {
			JSONC_Array *arr = member->value->aval;
			JSONC_Value *first = arr->head;
			if (first && first->jutype == JSONC_STRUCT) {
				JSONC_Struct *link = first->sval;
				sprintf(buf, "Array [ %s ]", link->name);
				prtable_col(prt, buf);
			} else if (first) {
				sprintf(buf, "Array [ %s ]", JSONC_TYPE(first->jutype));
				prtable_col(prt, buf);
			} else {
				prtable_col(prt, "Array [ ]");
			}
		}
		else if (member->value->jutype != JSONC_STRUCT) {
			prtable_col(prt, JSONC_TYPE(member->value->jutype));
		} else {
			JSONC_Struct *link = member->value->sval;
			prtable_col(prt, link->name);
		}
		if (member->description) {
			prtable_col(prt, member->description);
		} else {
			prtable_col(prt, "");
		}
	}
	prtable_print(prt);

	prtable_free(prt);
	printf("\n");
	printf("----\n");
}

void jsonc_print_md(const JSONC_Array *arr) {
	JSONC_Value *iter;
	for (iter = arr->head; iter; iter = iter->next) {
		if (iter->jutype != JSONC_STRUCT) continue;
		jsonc_print_md_struct(iter->sval);
	}
}
