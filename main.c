#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantic.h"

extern int yyparse(void);
extern FILE *yyin;
extern ASTNode *ast_root;

static void usage(const char *program)
{
    fprintf(stderr, "Usage: %s <source-file>\n", program);
}

int main(int argc, char **argv)
{
    int parse_status;
    int semantic_status;

    if (argc != 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    yyin = fopen(argv[1], "r");

    if (yyin == NULL) {
        perror(argv[1]);
        return EXIT_FAILURE;
    }

    ast_root = NULL;
    semantic_reset();

    /*
     * Temporary predefined variables.
     * Replace these with declaration handling if the language
     * is extended to support variable declarations.
     */
    semantic_declare_variable("x", SEM_TYPE_NUMBER);
    semantic_declare_variable("y", SEM_TYPE_NUMBER);
    semantic_declare_variable("z", SEM_TYPE_NUMBER);
    semantic_declare_variable("flag", SEM_TYPE_BOOL);

    printf("=== Conditional Statement Compiler ===\n");
    printf("Input file: %s\n\n", argv[1]);

    parse_status = yyparse();

    fclose(yyin);
    yyin = NULL;

    if (parse_status != 0 || ast_root == NULL) {
        fprintf(stderr,
                "\nSyntax analysis failed. Please check the input.\n");

        ast_free(ast_root);
        ast_root = NULL;
        semantic_reset();

        return EXIT_FAILURE;
    }

    printf("\nSyntax analysis: SUCCESS\n");

    printf("\n=== Abstract Syntax Tree (AST) ===\n");
    ast_print(ast_root);

    printf("\n=== Semantic Analysis ===\n");

    semantic_status = semantic_analyze(ast_root);

    if (semantic_status) {
        printf("Semantic analysis: SUCCESS\n");
    } else {
        printf("Semantic analysis: FAILED\n");
    }

    printf("\n=== Symbol Table ===\n");
    semantic_print_symbols();

    ast_free(ast_root);
    ast_root = NULL;

    semantic_reset();

    return semantic_status ? EXIT_SUCCESS : EXIT_FAILURE;
}
