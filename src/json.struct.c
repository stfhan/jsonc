#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json.struct.h"
#include "minire.h"

char* JSONC_TypeNames[] = {
	"undefined",
	"null",
	"Boolean",
	"Number",
	"String",
	"Object",
	"Array",
};

JSONC_Struct* jsonc_newstruct(
	) {

	JSONC_Struct* obj = (JSONC_Struct*)malloc(sizeof(JSONC_Struct));

	obj->head = NULL;
	obj->tail = NULL;
	obj->count = 0;

	obj->calign = 0;
	obj->name = NULL;
	obj->description = NULL;

	return obj;
}

JSONC_Array* jsonc_newarray(
	) {

	JSONC_Array* obj = (JSONC_Array*)malloc(sizeof(JSONC_Array));

	obj->head = NULL;
	obj->tail = NULL;
	obj->count = 0;

	return obj;
}

JSONC_Member* jsonc_newmember(
	const char *name, 
	const char *desc,
	JSONC_Value *s) {

	JSONC_Member* obj = (JSONC_Member*)malloc(sizeof(JSONC_Member));

	obj->name = strdup(name);
	obj->description = desc ? strdup(desc) : NULL;

	obj->value = s;

	obj->optional = 0;
	obj->next = NULL;

	return obj;
}


JSONC_Comment *jsonc_newcomment(const char *str) {

	JSONC_Comment* obj = (JSONC_Comment*)malloc(sizeof(JSONC_Comment));

	obj->buffer = strdup(str);

	return obj;
}

void jsonc_delcomment(JSONC_Comment *obj, int absorbed) {
	if (!absorbed) free(obj->buffer);
	free(obj);
}

JSONC_Value* jsonc_newvalue(int jutype, const void *s) {
//printf("Creating value with type %d (%s)\n",jutype,JSONC_TYPE(jutype));

	JSONC_Value* obj = (JSONC_Value*)malloc(sizeof(JSONC_Value));
	
	obj->next = NULL;
	obj->danglingL = NULL;
	obj->danglingR = NULL;	

	obj->jutype = jutype;
	switch (jutype) {
		case JSONC_NULL:
		case JSONC_UNDEFINED:
			
		break;
		case JSONC_FLOAT:
			obj->fvalue = atoi((char*)s);
		break;
		case JSONC_BOOL:
			obj->fvalue = ((int)s) ? 1 : 0;
		break;
		case JSONC_STRING:
			obj->strval = s;
		break;
		//-----
		case JSONC_ARRAY:
			obj->aval = (JSONC_Array*)s;
		break;
		case JSONC_STRUCT:
			obj->sval = (JSONC_Struct*)s;
		break;
		default:
			printf("Unknown type!\n");
		break;
	}

	return obj;
}

void jsonc_struct_addmember(JSONC_Struct *s, JSONC_Member *m) {
	if (s->head == NULL) {
		s->head = s->tail = m;
		
		if (!s->description) {
			//printf("Adding member %s to struct\n", m->name);
			//s->description = m->description;
			//s->name = m->description;
		}
		
	} else {
		s->tail->next = m;
		m->next = NULL;
		s->tail = m;
	}
	s->count++;
}
void jsonc_struct_absorb(JSONC_Struct *d, JSONC_Struct *s) {
	JSONC_Member *iter;
	JSONC_Member *next = NULL;
	for (iter = s->head; iter; iter = next) {
		next = iter->next;
		jsonc_struct_addmember(d, iter);
	}
	//kill 's'?
}

char *strmergekill(char *left, char *right) {
	if (!left && !right) return NULL;
	else if (left) return left;
	else if (right) return right;
	char *s = malloc(sizeof(char) * (strlen(left) + strlen(right) + 2));
	sprintf(s, "%s%s", left, right);
	free(left);
	free(right);
	return s;
}

void jsonc_struct_pushdown(JSONC_Struct *d, JSONC_Comment *c) {
	JSONC_Member *iter;
	char *next;
	int skip = 1;
	for (iter = d->head; iter; iter = iter->next) {
		if (skip) { skip = 0; next = iter->description; continue; }
		char *save = iter->description;
		if (iter != d->tail) {
			iter->description = next;
		} else {
			iter->description = strmergekill(next, iter->description);
		}
		next = save;
	}
	d->head->description = c->buffer;
	//kill 'c'?
	jsonc_delcomment(c, 1);
}

void jsonc_array_push(JSONC_Array *s, JSONC_Value *m) {
	m->next = NULL;
	if (s->head == NULL) {
		s->head = s->tail = m;
	} else {
		s->tail->next = m;
		m->next = NULL;
		s->tail = m;
	}
	s->count++;
}
void jsonc_array_absorb(JSONC_Array *d, JSONC_Array *s) {
	JSONC_Value *iter;
	JSONC_Value *next = NULL;
	for (iter = s->head; iter; iter = next) {
		next = iter->next;
		jsonc_array_push(d, iter);
	}
	//kill 's'?
}

void jsonc_comment_absorb(JSONC_Comment *d, JSONC_Comment *s) {
	if (!s) return;
	char *nbuf = (char*)malloc(sizeof(char) * (strlen(d->buffer) + strlen(s->buffer) + 2));
	sprintf(nbuf, "%s%s", d->buffer, s->buffer);
	free(d->buffer);
	d->buffer = nbuf;
	//kill 's'?
}

/*                 *
 * ############### *
 **/             //*
 
JSONC_Array* jsonc_flatten_structs(JSONC_Value *val) {
	JSONC_Array *ret = jsonc_newarray();

	jsonc_flatten_structs_value(ret, val);

	return ret;
}

void jsonc_struct_absorb_comments(JSONC_Struct *d, JSONC_Comment *c1, JSONC_Comment *c2) {
	char *op1 = strmergekill(d->description, c1 ? c1->buffer : NULL);
	char *op2 = strmergekill(op1, c2 ? c2->buffer : NULL);

	d->description = op2;

	jsonc_delcomment(c1, 1);
	jsonc_delcomment(c2, 1);
}
void jsonc_member_absorb_comments(JSONC_Member *d, JSONC_Comment *c1, JSONC_Comment *c2) {
	char *op1 = strmergekill(d->description, c1 ? c1->buffer : NULL);
	char *op2 = strmergekill(op1, c2 ? c2->buffer : NULL);

	d->description = op2;

	jsonc_delcomment(c1, 1);
	jsonc_delcomment(c2, 1);
}


void jsonc_flatten_structs_value(JSONC_Array *ret, JSONC_Value *val) {

	if (val->jutype == JSONC_STRUCT) {
		JSONC_Struct *obj = val->sval;

		jsonc_struct_absorb_comments(obj, val->danglingL, val->danglingR);
		val->danglingL = val->danglingR = NULL;

		if (obj->description && !obj->name) {
			char *match;
			int o = minire(obj->description, "struct ", MINIRE_WORD, NULL, &match);
			if (o) {
				obj->name = malloc(sizeof(char) * (o+1));
				memcpy(obj->name, match, o);
			}
		}

		jsonc_flatten_structs_struct(ret, val->sval);

	} else if (val->jutype == JSONC_ARRAY) {

		jsonc_flatten_structs_array(ret, val->aval);

	}

}
void jsonc_flatten_structs_struct(JSONC_Array *ret, JSONC_Struct *obj) {
	JSONC_Member *member;

	JSONC_Value *tmpval = jsonc_newvalue(JSONC_STRUCT, obj);
	jsonc_array_push(ret, tmpval);

	for (member = obj->head; member; member = member->next) {

		jsonc_flatten_structs_value(ret, member->value);

		if (!member->description) {
			JSONC_Value *val = member->value;
			if (val->jutype != JSONC_STRUCT && val->jutype != JSONC_ARRAY) {

				jsonc_member_absorb_comments(member, val->danglingL, val->danglingR);
				val->danglingL = val->danglingR = NULL;

			}
		}

		if (member->description && strstr(member->description, "optional")) {
			member->optional = 1;
		}

	}
}
void jsonc_flatten_structs_array(JSONC_Array *ret, JSONC_Array *arr) {
	JSONC_Value *iter;
	for (iter = arr->head; iter; iter = iter->next) {
		jsonc_flatten_structs_value(ret, iter);
	}
}
