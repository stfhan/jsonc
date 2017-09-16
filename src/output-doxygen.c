#include <stdio.h>

#include "output.h"

void jsonc_print_doxygen_struct(const JSONC_Struct *obj) {
	printf("/** %s */\n", obj->description ? obj->description : "");
	printf("struct %s {\n", obj->name ? obj->name : "");

	//printf(" (%d members)\n", obj->count);
	JSONC_Member *member;
	for (member = obj->head; member; member = member->next) {
		printf("  /** %s */\n", member->description);

		if (member->value->jutype == JSONC_ARRAY) {
			JSONC_Array *arr = member->value->aval;
			JSONC_Value *first = arr->head;
			if (first && first->jutype == JSONC_STRUCT) {
				JSONC_Struct *link = first->sval;
				printf("Array [ %s ]", link->name);
			} else if (first) {
				printf("Array [ %s ]", JSONC_TYPE(first->jutype));
			} else {
				printf("Array");			
			}
		}
		else if (member->value->jutype != JSONC_STRUCT) {
			printf("  %s", JSONC_TYPE(member->value->jutype));
		} else {
			//JSONC_Struct *link = member->value->sval;
			//printf("<a href='#%s'>%s</a>", link->name,link->name);
		}
		printf("  %s; \n", member->name);
	}
	printf("};\n");
}

void jsonc_print_doxygen(const JSONC_Array *arr) {
	printf("namespace JSON_API {\n");
	JSONC_Value *iter;
	for (iter = arr->head; iter; iter = iter->next) {
		if (iter->jutype != JSONC_STRUCT) continue;
		jsonc_print_doxygen_struct(iter->sval);
	}
	printf("}\n");
}
