
%{
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
                    yylval.number = atof(yytext);
                    return NUMBER;
                  }

[a-zA-Z_][a-zA-Z0-9_]* {
                          yylval.text = malloc(yyleng + 1);
                          if (yylval.text == NULL) {
                              fprintf(stderr, "Memory allocation failed\n");
                              exit(EXIT_FAILURE);
                          }
                          strcpy(yylval.text, yytext);
                          return IDENTIFIER;
                        }

[ \t\r]+        { /* Ignore whitespace */ }
\n              { /* Line number tracked by Flex */ }

.               {
                    fprintf(stderr,
                            "Lexical Error at line %d: Invalid character '%s'\n",
                            yylineno, yytext);
                    return INVALID;
                }

%%
