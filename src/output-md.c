#include <stdio.h>

#include "output.h"

void jsonc_print_md_struct(const JSONC_Struct *obj) {
	printf("##  %s\n", obj->name ? obj->name : "");
	printf("%s\n", obj->description ? obj->description : "");

	JSONC_Member *member;

	printf("\n");
	printf("| Member | Type | Description |\n");
	printf("|--------|------|-------------|\n");
	for (member = obj->head; member; member = member->next) {
		printf("| ");
		printf("%s ", member->name);
		printf("| ");
		if (member->value->jutype == JSONC_ARRAY) {
			JSONC_Array *arr = member->value->aval;
			JSONC_Value *first = arr->head;
			if (first && first->jutype == JSONC_STRUCT) {
				JSONC_Struct *link = first->sval;
				printf("Array [ %s ] ", link->name);
			} else if (first) {
				printf("Array [ %s ] ", JSONC_TYPE(first->jutype));
			} else {
				printf("Array [ ] ");			
			}
		}
		else if (member->value->jutype != JSONC_STRUCT) {
			printf("%s ", JSONC_TYPE(member->value->jutype));
		} else {
			JSONC_Struct *link = member->value->sval;
			printf("%s ", link->name );
		}
		printf("| ");
		if (member->description) printf("%s", member->description);
		printf(" | ");
		//jsonc_printf_value(iter->value);
		//printf(", ");
		//if (iter->description) printf("//;%s\n", iter->description);
		printf("\n");
	}
	printf("------------------------------- \n");
}

void jsonc_print_md(const JSONC_Array *arr) {
	JSONC_Value *iter;
	for (iter = arr->head; iter; iter = iter->next) {
		if (iter->jutype != JSONC_STRUCT) continue;
		jsonc_print_md_struct(iter->sval);
	}
}
