#include <stdio.h>

#include "output.h"

void jsonc_print_value(const JSONC_Value *obj) {
	if (obj->danglingL) printf("//,%s\n", obj->danglingL->buffer);
	if (obj->danglingR) printf("//.%s\n", obj->danglingR->buffer);
	//printf("<%d>\n", obj->jutype);
	switch (obj->jutype) {
		case JSONC_FLOAT:
			printf("%0.2f", obj->fvalue);
		break;
		case JSONC_BOOL:
			printf("%s", obj->fvalue ? "true" : "false");
		break;
		case JSONC_STRING:
			printf("\"%s\"", obj->strval);
		break;
		case JSONC_NULL:
			printf("null");
		break;
		case JSONC_UNDEFINED:
			printf("undefined");
		break;
		//-----
		case JSONC_ARRAY:
			jsonc_print_array(obj->aval);
		break;
		case JSONC_STRUCT:
			jsonc_print_struct(obj->sval);
		break;
		default:
			printf("GADEL! Got value with undefined jutype (%d %s)\n", obj->jutype, JSONC_TYPE(obj->jutype));
		break;
	}
	printf("\n");
}


void jsonc_print_struct(const JSONC_Struct *obj) {
	printf("{ ");
	//printf(" (%d members)\n", obj->count);
	JSONC_Member *iter;
	for (iter = obj->head; iter; iter = iter->next) {
		printf("%s: ", iter->name);
		if (iter->description) printf("//;%s\n", iter->description);
		printf("         ");
		jsonc_print_value(iter->value);
		printf(", ");
	}
	printf(" }");
}

void jsonc_print_array(const JSONC_Array *arr) {
	printf("[ ");
	//printf(" (%d members)\n", arr->count);
	JSONC_Value *iter;
	for (iter = arr->head; iter; iter = iter->next) {
		jsonc_print_value(iter);
		printf(", ");
	}
	printf(" ]");
}

void jsonc_print_flat_struct(const JSONC_Struct *obj) {
	printf("%s -- ", obj->name ? obj->name : "Nul");
	printf("%s\n", obj->description ? obj->description : "Nil");
	printf("{ \n");
	//printf(" (%d members)\n", obj->count);
	JSONC_Member *iter;
	for (iter = obj->head; iter; iter = iter->next) {
		printf("  %12s: ", iter->name);
		printf("%s", iter->description);
		if (iter->value->jutype == JSONC_STRUCT) {
			//iter->value
			JSONC_Struct *link = iter->value->sval;
			printf("<a href='#%s'>%s - %s</a>", link->name,link->name,link->description);
		}
		printf("\n");
		//jsonc_print_value(iter->value);
		//printf(", ");
		//if (iter->description) printf("//;%s\n", iter->description);
	}
	printf("} \n");
}

void jsonc_print_flat(const JSONC_Array *arr) {
	printf("[ ");
	printf(" (%d members)\n", arr->count);
	JSONC_Value *iter;
	for (iter = arr->head; iter; iter = iter->next) {
		if (iter->jutype != JSONC_STRUCT) continue;
		jsonc_print_flat_struct(iter->sval);
		printf(", ");
	}
	printf(" ]");
}




