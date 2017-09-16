#include <stdio.h>

#include "output.h"

void jsonc_print_xml_struct(const JSONC_Struct *obj) {
	printf("<structure name=\"%s\">\n", obj->name);
	puts("  <description>");
	printf("  %s\n", obj->description ? obj->description : "");
	puts("  </description>");
	//printf(" (%d members)\n", obj->count);
	JSONC_Member *member;
	puts("  <members>");
	for (member = obj->head; member; member = member->next) {
		printf("    <member name=\"%s\" type=\"", member->name); 
		if (member->value->jutype == JSONC_ARRAY) {
			JSONC_Array *arr = member->value->aval;
			JSONC_Value *first = arr->head;
			if (first && first->jutype == JSONC_STRUCT) {
				JSONC_Struct *link = first->sval;
				printf("Array[%s]", link->name);
			} else if (first) {
				printf("Array[%s]", JSONC_TYPE(first->jutype));
			} else {
				printf("%s", "Array");			
			}
		}
		else if (member->value->jutype != JSONC_STRUCT) {
			printf("%s", JSONC_TYPE(member->value->jutype));
		} else {
			JSONC_Struct *link = member->value->sval;
			printf("%s", link->name,link->name);
		}
		printf("\" optional=\"%s\">\n", member->optional ? "true" : "false");
		puts("      <description>");
		if (member->description) printf("%s", member->description);
		puts("      </description>");
		puts("    </member>");
	}
	puts("</members>");
	puts("</structure>");
}

void jsonc_print_xml(const JSONC_Array *arr) {
	puts("<structures>\n");
	JSONC_Value *iter;
	for (iter = arr->head; iter; iter = iter->next) {
		if (iter->jutype != JSONC_STRUCT) continue;
		jsonc_print_xml_struct(iter->sval);
	}
	puts("</structures>\n");
}


