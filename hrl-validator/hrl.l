%{
#include "hrl.tab.h"
%}

%%

"setup"                   { return SETUP; }
"main"                    { return MAIN; }
"enum"                    { return ENUM; }
"struct"                  { return STRUCT; }
"behavior"                { return BEHAVIOR; }
"while"                   { return WHILE; }
"if"                      { return IF; }
"else"                    { return ELSE; }
"const"                   { return CONST; }
"function"                { return FUNCTION; }
"threadloop"              { return THREADLOOP; }
"break"                   { return BREAK; }
"continue"                { return CONTINUE; }
"return"                  { return RETURN; }
"=="                      { return EQ; }
"!="                      { return NEQ; }
"<="                      { return LEQ; }
">="                      { return GEQ; }
"<"                       { return LT; }
">"                       { return GT; }
":"                       { return COLON; }
","                       { return COMMA; }
"="                       { return ASSIGN; }
";"                       { return SEMICOLON; }
"{"                       { return LBRACE; }
"}"                       { return RBRACE; }
"("                       { return LPAREN; }
")"                       { return RPAREN; }
"."                       { return DOT; }
"["                       { return LBRACKET; }
"]"                       { return RBRACKET; }
"+"                       { return PLUS; }
"-"                       { return MINUS; }
"*"                       { return MULT; }
"/"                       { return DIV; }
"%"                       { return MOD; }
"&&"                      { return AND; }
"||"                      { return OR; }
"!"                       { return NOT; }
".."                      { return CONCAT; }
[a-zA-Z_][a-zA-Z0-9_]*    { yylval.str = strdup(yytext); return IDENTIFIER; }
\"([^\\\n]|\\.)*?\"       { yylval.str = strdup(yytext); return STRING_LITERAL; }
[0-9]+                    { yylval.num = atoi(yytext); return NUMBER_LITERAL; }
[ \t\n\r]+                { /* ignore whitespace */ }
.                         { printf("Unexpected character: %c\n", *yytext); }

%%

int yywrap() {
    return 1;
}
