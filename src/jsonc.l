%{
    #include<stdio.h>
    #include "jsonc.tab.h"
    char *strclone(char *str, int off, int cut);
    //yydebug = 0;
    #define YYDEBUG 1
    #define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno;
%}
%option yylineno
DIGIT1to9 [1-9]
DIGIT [0-9]
DIGITS {DIGIT}+
INT {DIGIT}|{DIGIT1to9}{DIGITS}|-{DIGIT}|-{DIGIT1to9}{DIGITS}
FRAC [.]{DIGITS}
EXP {E}{DIGITS}
E [eE][+-]?
HEX_DIGIT [0-9a-f]
NUMBER {INT}|{INT}{FRAC}|{INT}{EXP}|{INT}{FRAC}{EXP}
UNESCAPEDCHAR [ -!#-\[\]-~]
ESCAPEDCHAR \\["\\bfnrt/]
UNICODECHAR \\u{HEX_DIGIT}{HEX_DIGIT}{HEX_DIGIT}{HEX_DIGIT}
CHAR {UNESCAPEDCHAR}|{ESCAPEDCHAR}|{UNICODECHAR}
CHARS {CHAR}+
WORD [a-zA-Z_][a-zA-Z_0-9]*
DBL_QUOTE ["]
SGL_QUOTE [']
%x SC_COMMENT
%%
"/*"([^*]|\*+[^*/])*\*+"/" {
						yylval = (int)strclone(yytext, 2, 2);
    					return COMMENT; }

"//"				{ BEGIN(SC_COMMENT); }
<SC_COMMENT>"\n"	{ BEGIN(INITIAL); }
<SC_COMMENT>.*		{ yylval = (int)strclone(yytext, 0, 0);
    					return COMMENT; }

{SGL_QUOTE}{SGL_QUOTE} |
{SGL_QUOTE}[^\n\'\\]+{SGL_QUOTE} {
    yylval = (int)strclone(yytext, 1, 1);
    return STRING;
};
{DBL_QUOTE}{DBL_QUOTE} |
{DBL_QUOTE}([^\n"\\])+{DBL_QUOTE} {
	yylval = (int)strclone(yytext, 1, 1);
    return STRING;
};
{NUMBER} {
    yylval = (int)strclone(yytext, 0, 0);
    return NUMBER;
};
true {
    return true;
};
false {
    return false;
};
null {
    return null;
};
undefined {
    return undefined;
};

{WORD} {
	yylval = (int)strclone(yytext, 0, 0);
    return WORD;
};

\{ {
    return O_BEGIN;
};

\} {
    return O_END;
};

\[ {
    return A_BEGIN;
};

\] {
    return A_END;
};

, {
    return COMMA;
};
: {
    return COLON;
};

[ \t\n]+ /* ignore whitespace */;

. {
    printf("Unexpected: %c\nExiting...\n",*yytext);
    exit(0);
}
%%
char *strclone(char *str, int off, int cut)
{
	char *clone;
    int len = strlen(str);
	if (cut) str[ len - cut ] = '\0';
	len -= cut;
	len -= off;
	str += off;
    clone = (char *)malloc(sizeof(char)*(len+1));
    strcpy(clone, str);
    return clone;
}