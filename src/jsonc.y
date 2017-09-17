%{
    #include<stdio.h>
    #include<string.h>
    #include<stdlib.h>
    #define YYSTYPE void*
    #define YYERROR_VERBOSE
    #include "json.struct.h"
    #include "output.h"
    void yyerror(JSONC_Value** retval, char const *s);
%}
%debug
%locations
%parse-param {JSONC_Value **retval}
%token NUMBER
%token STRING
%token COMMENT
%token WORD
%token true false null undefined
%left O_BEGIN O_END A_BEGIN A_END
%left COMMA
%left COLON
%%
START: CVALUE {
	$$ = $1;
	*retval = (JSONC_Value*)$1;
}
;

OBJECT: O_BEGIN O_END {
    $$=(void*) jsonc_newstruct();
  }
| O_BEGIN MEMBERS O_END {
	$$=(void*) $2; //JSONC_Struct
  }
| O_BEGIN COMMENTS MEMBERS O_END {
	$$=(void*) $3; //JSONC_Struct
	jsonc_struct_pushdown($$, $2);
  }
;

MEMBERS: PAIR {
	$$=(void*) jsonc_newstruct();
	jsonc_struct_addmember((JSONC_Struct*)$$, (JSONC_Member*)$1);
  }
| PAIR MEMBERS {
	$$=(void*) jsonc_newstruct();
	jsonc_struct_addmember((JSONC_Struct*)$$, (JSONC_Member*)$1);
	jsonc_struct_absorb((JSONC_Struct*)$$, (JSONC_Struct*)$2);
  }
;
PAIR: NAME COLON CVALUE {
	$$=(void*) jsonc_newmember($1, NULL, (JSONC_Value*)$3);
  }
| NAME COLON CVALUE COMMA {
	$$=(void*) jsonc_newmember($1, NULL, (JSONC_Value*)$3);
  }
| NAME COLON CVALUE COMMA COMMENTS {
	$$=(void*) jsonc_newmember($1, NULL, (JSONC_Value*)$3);
	jsonc_member_absorb_comments((JSONC_Member*)$$, (JSONC_Comment*)$5, NULL);
  }
;

ARRAY: A_BEGIN A_END {
    $$=(void*) jsonc_newarray();
  }
| A_BEGIN ELEMENTS A_END {
	$$=(void*) $2;
}
;
ELEMENTS: CVALUE {
    $$=(void*) jsonc_newarray();
    jsonc_array_push((JSONC_Array*)$$, (JSONC_Value*)$1);
  }
| CVALUE COMMA {
	$$=(void*) jsonc_newarray();
    jsonc_array_push((JSONC_Array*)$$, (JSONC_Value*)$1);
  }
| CVALUE COMMA COMMENTS {
	$$=(void*) jsonc_newarray();
	jsonc_comment_absorb(((JSONC_Value*)$1)->danglingR, (JSONC_Comment*)$3);
    jsonc_array_push((JSONC_Array*)$$, (JSONC_Value*)$1);
  }
| CVALUE COMMA ELEMENTS {
	$$=(void*) jsonc_newarray();
	jsonc_array_push((JSONC_Array*)$$, (JSONC_Value*)$1);
    jsonc_array_absorb((JSONC_Array*)$$, (JSONC_Array*)$3);
  }
;

COMMENTS: COMMENT COMMENTS {
	$$=(void*) jsonc_newcomment($1);
	jsonc_comment_absorb((JSONC_Comment*)$$, (JSONC_Comment*)$2);
}
|
COMMENT {
	$$=(void*) jsonc_newcomment($1);
}
;
VALUE: STRING {
	$$=(void*)jsonc_newvalue(JSONC_STRING, yylval);
  }
| NUMBER {
	$$=(void*)jsonc_newvalue(JSONC_FLOAT, yylval);
  }
| OBJECT {
	$$=(void*)jsonc_newvalue(JSONC_STRUCT, $1);
  }
| ARRAY {
	$$=(void*)jsonc_newvalue(JSONC_ARRAY, $1);
  }
| true {
	$$=(void*)jsonc_newvalue(JSONC_BOOL, (void*)0x01);
  }
| false {
	$$=(void*)jsonc_newvalue(JSONC_BOOL, NULL);
  }
| null {
	$$=(void*)jsonc_newvalue(JSONC_NULL, NULL);
}
| undefined {
	$$=(void*)jsonc_newvalue(JSONC_UNDEFINED, NULL);
}

;
CVALUE: VALUE { 
	$$=$1; 
  }
| VALUE COMMENTS {
	((JSONC_Value*)$1)->danglingR = (JSONC_Comment*)$2;
	$$=$1;
  }
| COMMENTS VALUE {
	((JSONC_Value*)$2)->danglingL = (JSONC_Comment*)$1;
	$$=$2;
  }
| COMMENTS VALUE COMMENTS {
	((JSONC_Value*)$2)->danglingL = (JSONC_Comment*)$1;
	((JSONC_Value*)$2)->danglingR = (JSONC_Comment*)$3;
	$$=$2;
  }
;
NAME: STRING {
	$$ = $1;
}
| WORD {
	$$ = $1;
}
%%
void print_version() {
	fprintf(stderr, "jsonc v0.0.1\n");
}
void show_usage() {
	print_version();
	printf("Usage: jsonc [OPTIONS] [SOURCE] > OUTPUT\n");
	printf(" SOURCE:\n");
	printf("   An annotated JSON file. If ommited, data is read from stdin.\n");
	printf(" OUTPUT:\n");
	printf("   Output file. By default, stdout is used.\n");
	printf(" OPTIONS:\n");
	printf("   --version  Show version and exit.\n");
	printf("   --help     Show this message and exit.\n");
	printf("   --echo     Output text (CLI).\n");
	printf("   --flat     Output flat text (CLI).\n"); 
	printf("   --html     Output HTML.\n");
	printf("   --xml      Output XML.\n");
	printf("   --md       Output Markdown.\n");
	printf("   --doxygen  Output Doxygen-compatible C.\n");
	printf("   --fragment HTML: only output relevant code.\n");
	printf("   --page     HTML: output full page.\n");
	printf("   --css      HTML: output CSS only.\n");
	exit(0);
}
void show_version() {
	print_version();
	exit(0);
}
extern FILE* yyin;
extern FILE* yyout;
int main(int argc, char *argv[])
{
	int mode = JSONC_OUT_ECHO;
	int htmlpage = 1;
	int i;

	FILE *in = stdin;
	FILE *out = stdout;
	JSONC_Value *val;
	int fail;

	for (i = 1; i < argc; i++) {
		if (!strcasecmp(argv[i], "--usage")) show_usage();
		else if (!strcasecmp(argv[i], "--help")) show_usage();
		else if (!strcasecmp(argv[i], "--version")) show_version();
		else if (!strcasecmp(argv[i], "--fragment")) htmlpage = 0;
		else if (!strcasecmp(argv[i], "--page")) htmlpage = 1;
		else if (!strcasecmp(argv[i], "--echo")) mode = JSONC_OUT_ECHO;
		else if (!strcasecmp(argv[i], "--flat")) mode = JSONC_OUT_FLAT;
		else if (!strcasecmp(argv[i], "--html")) mode = JSONC_OUT_HTML;
		else if (!strcasecmp(argv[i], "--css")) mode = JSONC_OUT_HTML_CSSONLY;
		else if (!strcasecmp(argv[i], "--xml")) mode = JSONC_OUT_XML;
		else if (!strcasecmp(argv[i], "--md")) mode = JSONC_OUT_MD;
		else if (!strcasecmp(argv[i], "--doxygen")) mode = JSONC_OUT_DOXYGEN;
		else {
			in = fopen(argv[i], "r");
			if (!in) {
				fprintf(stderr, "Unable to open file '%s'\n", argv[i]);
				exit(-1);
			}
		}
	}

	yyin = in;
	yyout = out;
	fail = yyparse(&val);
	if (!fail) {
		JSONC_Array *flat = jsonc_flatten_structs(val);
		switch (mode) {
			case JSONC_OUT_ECHO:
				jsonc_print_value(val);
			break;
			case JSONC_OUT_FLAT:
				jsonc_print_flat(flat);
			break;
			case JSONC_OUT_HTML:
				if (htmlpage) jsonc_print_html_header();
				jsonc_print_html(flat);
				if (htmlpage) jsonc_print_html_footer();
			break;
			case JSONC_OUT_HTML_CSSONLY:
				jsonc_print_html_css();
			break;
			case JSONC_OUT_XML:
				if (htmlpage) puts("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
				jsonc_print_xml(flat);
			break;			
			case JSONC_OUT_MD:
				jsonc_print_md(flat);
			break;			
			case JSONC_OUT_DOXYGEN:
				jsonc_print_doxygen(flat);
			break;
		}
	}
	return fail;
}
int yywrap()
{
	return 1;
}
void yyerror (JSONC_Value** retval, char const *s) {
	fprintf (stderr, "%s on line %d\n", s, yylloc.first_line);
}
