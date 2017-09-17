#ifndef __JSONC_STRUCT_H__
#define __JSONC_STRUCT_H__

typedef struct JSONC_Member JSONC_Member;
typedef struct JSONC_Struct JSONC_Struct;
typedef struct JSONC_Array JSONC_Array;
typedef struct JSONC_Value JSONC_Value;
typedef struct JSONC_Comment JSONC_Comment;

typedef enum {
	JSONC_UNDEFINED,
	JSONC_NULL,
	JSONC_BOOL,
	JSONC_FLOAT,
	JSONC_STRING,
	JSONC_STRUCT,
	JSONC_ARRAY,
	
	JSONC__TOTAL,
} JSONC_JUTYPE;

struct JSONC_Value {

	JSONC_JUTYPE jutype;
	union {
		JSONC_Array  *aval;
		JSONC_Struct *sval;
		double fvalue;
		const char *strval;
	};

	JSONC_Comment *danglingL;
	JSONC_Comment *danglingR;

	JSONC_Value *next;
};

struct JSONC_Member {

	char *name;
	char *description;
	int optional;

	JSONC_Value *value;

	JSONC_Member *next;
};

struct JSONC_Struct {

	char *name;
	char *description;

	JSONC_Member *head;
	JSONC_Member *tail;
	int count;

	int calign;
};

struct JSONC_Array {

	char *description;
	JSONC_Value *head;
	JSONC_Value *tail;
	int count;

};

struct JSONC_Comment {
	char *buffer;
};

extern JSONC_Value* jsonc_newvalue(int jutype, const void *s);
extern JSONC_Struct* jsonc_newstruct();
extern JSONC_Array* jsonc_newarray();
extern JSONC_Member* jsonc_newmember(
	const char *name, const char *desc, JSONC_Value *s);
extern JSONC_Comment *jsonc_newcomment(const char *str);

extern void jsonc_delcomment(JSONC_Comment*, int absorbed);

extern void jsonc_struct_addmember(JSONC_Struct *d, JSONC_Member *m);
extern void jsonc_struct_absorb(JSONC_Struct *d, JSONC_Struct *s);
extern void jsonc_array_push(JSONC_Array *d, JSONC_Value *m);
extern void jsonc_array_absorb(JSONC_Array *d, JSONC_Array *s);
extern void jsonc_comment_absorb(JSONC_Comment *d, JSONC_Comment *s);

extern JSONC_Array* jsonc_flatten_structs(JSONC_Value *val);
extern void jsonc_flatten_structs_value(JSONC_Array *ret, JSONC_Value *val);
extern void jsonc_flatten_structs_struct(JSONC_Array *ret, JSONC_Struct *obj);
extern void jsonc_flatten_structs_array(JSONC_Array *ret, JSONC_Array *arr);

/* ^[O]     [O]^ */
#define JSONC_TYPE(A) (\
((A) >= 0 && (A) < JSONC__TOTAL) \
? JSONC_TypeNames[(A)] : "/err/" )
/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */
extern char* JSONC_TypeNames[];
/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

#endif /*__JSONC_STRUCT_H__*/