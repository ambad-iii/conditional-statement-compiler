
%{
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
void yyerror(const char *s);

extern int yylineno;
%}

/* Tokens from lexer.l */
%token IF ELSE
%token AND OR EQ NE GE LE GT LT NOT ASSIGN
%token PLUS MINUS MULTIPLY DIVIDE
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON
%token NUMBER IDENTIFIER INVALID

/* Operator precedence and associativity */
%left OR
%left AND
%left EQ NE
%left GT LT GE LE
%left PLUS MINUS
%left MULTIPLY DIVIDE
%right NOT
%right UMINUS

/* Handle the dangling-else ambiguity */
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%start program

%%

program
    : statement_list
      {
          printf("\nSyntax analysis successful!\n");
      }
    ;

statement_list
    : statement
    | statement_list statement
    ;

statement
    : conditional_statement
    | assignment_statement
    | block
    ;

conditional_statement
    : IF LPAREN expression RPAREN statement
        %prec LOWER_THAN_ELSE
    | IF LPAREN expression RPAREN statement ELSE statement
    ;

block
    : LBRACE statement_list RBRACE
    | LBRACE RBRACE
    ;

assignment_statement
    : IDENTIFIER ASSIGN expression SEMICOLON
    ;

expression
    : IDENTIFIER
    | NUMBER
    | LPAREN expression RPAREN
    | expression PLUS expression
    | expression MINUS expression
    | expression MULTIPLY expression
    | expression DIVIDE expression
    | expression GT expression
    | expression LT expression
    | expression GE expression
    | expression LE expression
    | expression EQ expression
    | expression NE expression
    | expression AND expression
    | expression OR expression
    | NOT expression
    | MINUS expression %prec UMINUS
    ;

%%

void yyerror(const char *s)
{
    fprintf(stderr, "Syntax Error at line %d: %s\n",
            yylineno, s);
}

int main(void)
{
    printf("Enter a conditional statement (Ctrl+D to finish):\n");
    return yyparse();
}
