%{
#include "hrl.tab.h"
%}

%%
"setup"         return SETUP;
"main"          return MAIN;
"enum"          return ENUM;
"struct"        return STRUCT;
"behavior"      return BEHAVIOR;
"loop"          return LOOP;
"if"            return IF;
"else"          return ELSE;
"switch"        return SWITCH;
"case"          return CASE;
"const"         return CONST;
"function"      return FUNCTION;
"threadloop"    return THREADLOOP;
"break"         return BREAK;
"continue"      return CONTINUE;

"int"           return TYPE;
"float"         return TYPE;
"string"        return TYPE;
"bool"          return TYPE;
"void"          return TYPE;

[a-zA-Z_][a-zA-Z0-9_]*    return IDENTIFIER;
\"[^\"]*\"       return STRING_LITERAL;
[0-9]+          return NUMBER_LITERAL;

"=="            return EQ;
"!="            return NEQ;
":"             return COLON;
","             return COMMA;
";"             return SEMICOLON;
"="             return ASSIGN;
"{"             return LBRACE;
"}"             return RBRACE;
"("             return LPAREN;
")"             return RPAREN;
"."             return DOT;
"["             return LBRACKET;
"]"             return RBRACKET;

[ \t\n\r]+      /* ignore whitespace */;
.               return yytext[0]; /* for any other character, return it as itself */
%%

int yywrap() {
    return 1;
}
