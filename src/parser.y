%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

extern int yylex(void);
extern int yylineno;
void yyerror(const char *s);

ASTNode *ast_root = NULL;
%}
%code requires {
    typedef struct ASTNode ASTNode;
}

%union {
    char *text;
    double number;
    ASTNode *node;
}

%token IF ELSE
%token AND OR EQ NE GE LE GT LT NOT ASSIGN
%token PLUS MINUS MULTIPLY DIVIDE
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON
%token <number> NUMBER
%token <text> IDENTIFIER
%token INVALID

%type <node> program statement_list statement
%type <node> conditional_statement assignment_statement
%type <node> block expression

%left OR
%left AND
%left EQ NE
%left GT LT GE LE
%left PLUS MINUS
%left MULTIPLY DIVIDE
%right NOT
%right UMINUS

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%start program

%%

program
    : statement_list
      {
          ast_root = ast_program($1);
          $$ = ast_root;
      }
    ;

statement_list
    : statement
      {
          $$ = NULL;
          ast_append(&$$, $1);
      }
    | statement_list statement
      {
          $$ = $1;
          ast_append(&$$, $2);
      }
    ;

statement
    : conditional_statement { $$ = $1; }
    | assignment_statement { $$ = $1; }
    | block                { $$ = $1; }
    ;

conditional_statement
    : IF LPAREN expression RPAREN statement %prec LOWER_THAN_ELSE
      {
          $$ = ast_if($3, $5);
      }
    | IF LPAREN expression RPAREN statement ELSE statement
      {
          $$ = ast_if_else($3, $5, $7);
      }
    ;

block
    : LBRACE statement_list RBRACE
      {
          $$ = ast_block($2);
      }
    | LBRACE RBRACE
      {
          $$ = ast_block(NULL);
      }
    ;

assignment_statement
    : IDENTIFIER ASSIGN expression SEMICOLON
      {
          $$ = ast_assign($1, $3);
          free($1);
      }
    ;

expression
    : IDENTIFIER
      {
          $$ = ast_identifier($1);
          free($1);
      }
    | NUMBER
      {
          $$ = ast_number($1);
      }
    | LPAREN expression RPAREN
      {
          $$ = $2;
      }
    | expression PLUS expression
      {
          $$ = ast_binary("+", $1, $3);
      }
    | expression MINUS expression
      {
          $$ = ast_binary("-", $1, $3);
      }
    | expression MULTIPLY expression
      {
          $$ = ast_binary("*", $1, $3);
      }
    | expression DIVIDE expression
      {
          $$ = ast_binary("/", $1, $3);
      }
    | expression GT expression
      {
          $$ = ast_binary(">", $1, $3);
      }
    | expression LT expression
      {
          $$ = ast_binary("<", $1, $3);
      }
    | expression GE expression
      {
          $$ = ast_binary(">=", $1, $3);
      }
    | expression LE expression
      {
          $$ = ast_binary("<=", $1, $3);
      }
    | expression EQ expression
      {
          $$ = ast_binary("==", $1, $3);
      }
    | expression NE expression
      {
          $$ = ast_binary("!=", $1, $3);
      }
    | expression AND expression
      {
          $$ = ast_binary("&&", $1, $3);
      }
    | expression OR expression
      {
          $$ = ast_binary("||", $1, $3);
      }
    | NOT expression
      {
          $$ = ast_unary("!", $2);
      }
    | MINUS expression %prec UMINUS
      {
          $$ = ast_unary("-", $2);
      }
    ;

%%

void yyerror(const char *s)
{
    fprintf(stderr, "Syntax Error at line %d: %s\n",
            yylineno, s);
}
