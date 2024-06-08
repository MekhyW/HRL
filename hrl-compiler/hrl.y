%{
#include <stdio.h>
#include <stdlib.h>
#include "hrl.tab.h"

void yyerror(const char *s);
int yylex(void);
%}

%token SETUP MAIN ENUM STRUCT BEHAVIOR LOOP IF ELSE SWITCH CASE CONST FUNCTION THREADLOOP BREAK CONTINUE
%token IDENTIFIER STRING_LITERAL NUMBER_LITERAL TYPE
%token EQ NEQ ASSIGN COLON COMMA SEMICOLON LBRACE RBRACE LPAREN RPAREN DOT LBRACKET RBRACKET

%start program

%%

program:
    setup main
;

setup:
    SETUP LBRACE statements RBRACE
;

main:
    MAIN LBRACE statements RBRACE
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
    | loop_statement
    | if_statement
    | switch_statement
    | function_call_statement SEMICOLON
    | function_declaration
    | const_declaration SEMICOLON
    | threadloop_statement SEMICOLON
    | break_statement SEMICOLON
    | continue_statement SEMICOLON
;

variable_declaration:
    const_opt IDENTIFIER COLON TYPE ASSIGN expression
;

const_opt:
    /* empty */
    | CONST
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
    | variable_declarations COMMA variable_declaration
;

behavior_declaration:
    BEHAVIOR IDENTIFIER LPAREN parameters RPAREN block
;

parameters:
    /* empty */
    | parameter
    | parameters COMMA parameter
;

parameter:
    IDENTIFIER COLON TYPE
;

block:
    LBRACE statements RBRACE
;

loop_statement:
    LOOP block
;

if_statement:
    IF LPAREN expression RPAREN block else_opt
;

else_opt:
    /* empty */
    | ELSE block
;

switch_statement:
    SWITCH expression LBRACE case_blocks RBRACE
;

case_blocks:
    case_block
    | case_blocks case_block
;

case_block:
    CASE expression COLON statements
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
    FUNCTION IDENTIFIER LPAREN parameters RPAREN COLON TYPE block
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

expression:
    simple_expression
    | simple_expression relational_operator simple_expression
;

relational_operator:
    EQ
    | NEQ
;

simple_expression:
    IDENTIFIER
    | STRING_LITERAL
    | NUMBER_LITERAL
    | LPAREN expression RPAREN
    | function_call_statement
    | array_access
    | member_access
;

array_access:
    IDENTIFIER LBRACKET expression RBRACKET
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
