%{
#include "hrl.tab.h"
%}

%%

"setup"               { return SETUP; }
"main"                { return MAIN; }
"enum"                { return ENUM; }
"struct"              { return STRUCT; }
"behavior"            { return BEHAVIOR; }
"loop"                { return LOOP; }
"if"                  { return IF; }
"else"                { return ELSE; }
"switch"              { return SWITCH; }
"case"                { return CASE; }
"const"               { return CONST; }
"function"            { return FUNCTION; }
"threadloop"          { return THREADLOOP; }
"break"               { return BREAK; }
"continue"            { return CONTINUE; }
"return"              { return RETURN; }

[a-zA-Z_][a-zA-Z0-9_]* { yylval.str = strdup(yytext); return IDENTIFIER; }
\"([^\\\n]|\\.)*?\"    { yylval.str = strdup(yytext); return STRING_LITERAL; }
[0-9]+                { yylval.num = atoi(yytext); return NUMBER_LITERAL; }

"=="                  { return EQ; }
"!="                  { return NEQ; }
"<"                   { return LT; }
">"                   { return GT; }
"<="                  { return LEQ; }
">="                  { return GEQ; }
":"                   { return COLON; }
","                   { return COMMA; }
";"                   { return SEMICOLON; }
"="                   { return ASSIGN; }
"{"                   { return LBRACE; }
"}"                   { return RBRACE; }
"("                   { return LPAREN; }
")"                   { return RPAREN; }
"."                   { return DOT; }
"["                   { return LBRACKET; }
"]"                   { return RBRACKET; }

[ \t\n\r]+            { /* Ignore whitespace */ }
.                     { printf("Unexpected character: %c\n", *yytext); }

%%

int yywrap() {
    return 1;
}
