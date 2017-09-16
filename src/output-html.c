#include <stdio.h>

#include "output.h"

void jsonc_print_html_header() {
	puts("<!DOCTYPE html>\n");
	puts("<html>\n");
	puts("<head>\n");

		puts("<style>\n");
		jsonc_print_html_css();
		puts("</style>\n");

	puts("</head>\n");
	puts("<body>\n");
}
void jsonc_print_html_footer() {
	puts("</body>\n");
	puts("</html>\n");
}
void jsonc_print_html_css() {
	puts("table { table-layout: fixed; width: 100%; border-collapse: collapse; }\n");
	puts("table td { padding: 4px 8px; font-family: sans-serif; }\n");
	puts("table td { border: 1px solid #999; vertical-align: top; }\n");
	puts("h2, p.description { font-family: sans-serif; }\n");
	puts("h2:target { text-decoration: underline; }\n");
	puts("table td:nth-child(1) {  }\n");
	puts("table tr.required td:nth-child(1) { color: #339; font-weight: bold; }\n");
	puts("table tr.optional td:nth-child(1) { color: #669; }\n");
	puts("table td:nth-child(2) {  }\n");
	puts("table td:nth-child(3) {  }\n");
	puts("table td:nth-child(4) { font-size: 13px; }\n");
	puts(".ad-type-String { color: #393; }");
	puts(".ad-type-Number { color: #33E; }");
	puts(".ad-type-Boolean { color: #33E; }");
	puts(".ad-type-null { color: #000; }");
	puts(".ad-type-undefined { color: #666; }");
	puts(".ad-struct { max-width: 800px; }");
}

void jsonc_print_html_struct(const JSONC_Struct *obj) {
	printf("  <h2 id='%s'>", obj->name ? obj->name : "");
	printf("  %s", obj->name ? obj->name : "");
	puts("  </h2>\n");
	puts("  <p class='description'>");
	printf("  %s\n", obj->description ? obj->description : "");
	puts("  </p>\n");
	//printf(" (%d members)\n", obj->count);
	JSONC_Member *member;
	puts("<table class='ad-table'>");
	puts("<tbody>\n");
	for (member = obj->head; member; member = member->next) {
		printf("<tr class='%s'>\n", member->optional ? "optional" : "required");
		printf("  <td>");
		printf("  %s ", member->name);
		printf("  </td>\n");
		printf("  <td>");
		if (member->value->jutype == JSONC_ARRAY) {
			JSONC_Array *arr = member->value->aval;
			JSONC_Value *first = arr->head;
			if (first && first->jutype == JSONC_STRUCT) {
				JSONC_Struct *link = first->sval;
				printf("Array [ <a href='#%s'>%s</a> ]", link->name,link->name);
			} else if (first) {
				printf("Array [ %s ]", JSONC_TYPE(first->jutype));
			} else {
				printf("Array [ ]");			
			}
		}
		else if (member->value->jutype != JSONC_STRUCT) {
			printf("  %s", JSONC_TYPE(member->value->jutype));
		} else {
			JSONC_Struct *link = member->value->sval;
			printf("<a href='#%s'>%s</a>", link->name,link->name);
		}
		printf("  </td>\n");
		printf("  <td class='ad-type-%s'>", JSONC_TYPE(member->value->jutype));
		if (member->value->jutype != JSONC_STRUCT
		&& member->value->jutype != JSONC_ARRAY) {
			jsonc_print_value(member->value);
		}
		printf("  </td>\n");
		printf("  <td>");
		if (member->description) printf("%s", member->description);
		printf("  </td>\n");
		//jsonc_print_value(iter->value);
		//printf(", ");
		//if (iter->description) printf("//;%s\n", iter->description);
		printf("</tr>\n");
	}
	printf("</tbody>\n");
	printf("</table>");
}

void jsonc_print_html(const JSONC_Array *arr) {

	//printf("<h1>%d members</h1>\n", arr->count);

	JSONC_Value *iter;
	for (iter = arr->head; iter; iter = iter->next) {
		if (iter->jutype != JSONC_STRUCT) continue;
		printf("<article class='ad-struct'>\n");
		jsonc_print_html_struct(iter->sval);
		printf("</article>\n");
	}
}
