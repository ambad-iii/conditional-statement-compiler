%{
#include "parser.tab.h"
#include <stdio.h>
%}

%option noyywrap yylineno

%%

"if"            { return IF; }
"else"          { return ELSE; }

"&&"            { return AND; }
"||"            { return OR; }
"=="            { return EQ; }
"!="            { return NE; }
">="            { return GE; }
"<="            { return LE; }

">"             { return GT; }
"<"             { return LT; }
"!"             { return NOT; }
"="             { return ASSIGN; }

"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return MULTIPLY; }
"/"             { return DIVIDE; }

"("             { return LPAREN; }
")"             { return RPAREN; }
"{"             { return LBRACE; }
"}"             { return RBRACE; }
";"             { return SEMICOLON; }

[0-9]+(\.[0-9]+)? {
                    return NUMBER;
                  }

[a-zA-Z_][a-zA-Z0-9_]* {
                          return IDENTIFIER;
                        }

[ \t\r]+        { /* Ignore spaces and tabs */ }
\n              { /* Track line numbers */ }

.               {
                    printf("Lexical Error at line %d: Invalid character '%s'\n",
                           yylineno, yytext);
                    return INVALID;
                }

%%
