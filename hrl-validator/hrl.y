%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void yyerror(const char *s);
int yylex(void);

typedef struct {
    char *str;
    int num;
    double real;
} YYSTYPE;

#define YYSTYPE_IS_DECLARED 1
%}

%union {
    char *str;
    int num;
    double real;
}

%token <str> IDENTIFIER STRING_LITERAL
%token <num> NUMBER_LITERAL
%token SETUP MAIN ENUM STRUCT BEHAVIOR WHILE IF ELSE CONST FUNCTION THREADLOOP BREAK CONTINUE RETURN TYPE
%token EQ NEQ LT GT LEQ GEQ ASSIGN COLON COMMA SEMICOLON LBRACE RBRACE LPAREN RPAREN DOT LBRACKET RBRACKET
%token PLUS MINUS MULT DIV MOD AND OR NOT CONCAT

%start program

%%

program:
    setup main
;

setup:
    SETUP block
;

main:
    MAIN block
;

block:
    LBRACE statements_opt RBRACE
;

statements_opt:
    /* empty */
    | statements
;

statements:
    statement
    | statements statement
;

statement:
    variable_declaration SEMICOLON
    | enum_declaration SEMICOLON
    | struct_declaration SEMICOLON
    | behavior_declaration
    | while_statement
    | if_statement
    | function_call_statement SEMICOLON
    | function_declaration
    | const_declaration SEMICOLON
    | threadloop_statement SEMICOLON
    | break_statement SEMICOLON
    | continue_statement SEMICOLON
    | return_statement SEMICOLON
;

while_statement:
    WHILE LPAREN expression RPAREN block
;

variable_declaration:
    IDENTIFIER COLON TYPE ASSIGN expression
    | IDENTIFIER COLON TYPE array_initializer
;

array_initializer:
    LBRACKET expression_list RBRACKET
;

expression_list:
    expression
    | expression_list COMMA expression
;

enum_declaration:
    ENUM IDENTIFIER LBRACE enum_members RBRACE
;

enum_members:
    enum_member
    | enum_members COMMA enum_member
;

enum_member:
    IDENTIFIER
;

struct_declaration:
    STRUCT IDENTIFIER LBRACE variable_declarations RBRACE
;

variable_declarations:
    variable_declaration
    | variable_declarations variable_declaration
;

behavior_declaration:
    BEHAVIOR IDENTIFIER LPAREN parameters_opt RPAREN block
;

parameters_opt:
    /* empty */
    | parameters
;

parameters:
    parameter
    | parameters COMMA parameter
;

parameter:
    IDENTIFIER COLON TYPE
;

if_statement:
    IF LPAREN expression RPAREN block else_opt
;

else_opt:
    /* empty */
    | ELSE block
;

function_call_statement:
    IDENTIFIER LPAREN arguments_opt RPAREN
;

arguments_opt:
    /* empty */
    | arguments
;

arguments:
    expression
    | arguments COMMA expression
;

function_declaration:
    FUNCTION IDENTIFIER LPAREN parameters_opt RPAREN COLON TYPE block
;

const_declaration:
    CONST IDENTIFIER ASSIGN expression
;

threadloop_statement:
    THREADLOOP IDENTIFIER LPAREN arguments_opt RPAREN
;

break_statement:
    BREAK
;

continue_statement:
    CONTINUE
;

return_statement:
    RETURN expression_opt
;

expression_opt:
    /* empty */
    | expression
;

expression:
    logical_term
    | logical_term OR expression
;

logical_term:
    logical_factor
    | logical_factor AND logical_term
;

logical_factor:
    NOT logical_factor
    | relational_expression
;

relational_expression:
    concat_expression
    | concat_expression relational_operator concat_expression
;

relational_operator:
    EQ
    | NEQ
    | LT
    | GT
    | LEQ
    | GEQ
;

concat_expression:
    arithmetic_expression
    | arithmetic_expression CONCAT concat_expression
;

arithmetic_expression:
    term
    | term PLUS arithmetic_expression
    | term MINUS arithmetic_expression
;

term:
    factor
    | factor MULT term
    | factor DIV term
    | factor MOD term
;

factor:
    PLUS factor
    | MINUS factor
    | NOT factor
    | simple_expression
;

simple_expression:
    IDENTIFIER
    | STRING_LITERAL
    | NUMBER_LITERAL
    | LPAREN expression RPAREN
    | function_call_statement
    | array_access
    | enum_access
    | member_access
;

array_access:
    IDENTIFIER LBRACKET expression RBRACKET
;

enum_access:
    IDENTIFIER COLON IDENTIFIER
;

member_access:
    IDENTIFIER DOT IDENTIFIER
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}

int main(int argc, char **argv) {
    return yyparse();
}
