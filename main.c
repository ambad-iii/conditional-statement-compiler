#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "semantic.h"

/*
 * Integration entry point for Member 5.
 *
 * AGREEMENT WITH parser.y:
 *   1. parser.y should include "ast.h".
 *   2. parser.y should define: ASTNode *ast_root = NULL;
 *   3. The start rule should assign the finished tree to ast_root.
 *
 * If your parser uses a different root variable name, change the extern below.
 */
extern int yyparse(void);
extern FILE *yyin;
extern ASTNode *ast_root;

static void usage(const char *program) {
    fprintf(stderr, "Usage: %s <source-file>\n", program);
}

int main(int argc, char **argv) {
    int parse_status;

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
     * Register names used by the starter test suite.
     * This is only a temporary bridge because the current language design
     * has no declaration statement. Replace this with real declaration
     * handling if declarations are added to the grammar.
     */
    semantic_declare_variable("x", SEM_TYPE_NUMBER);
    semantic_declare_variable("y", SEM_TYPE_NUMBER);
    semantic_declare_variable("z", SEM_TYPE_NUMBER);
    semantic_declare_variable("flag", SEM_TYPE_BOOL);

    printf("=== Conditional Statement Compiler ===\n");
    printf("Input file: %s\n\n", argv[1]);

    parse_status = yyparse();
    fclose(yyin);

    if (parse_status != 0 || ast_root == NULL) {
        fprintf(stderr, "\nSyntax analysis failed. Please check the input.\n");
        ast_free(ast_root);
        semantic_reset();
        return EXIT_FAILURE;
    }

    printf("Syntax analysis: SUCCESS\n\n");
    printf("=== Abstract Syntax Tree (AST) ===\n");
    ast_print(ast_root);

    printf("\n=== Semantic Analysis ===\n");
    if (semantic_analyze(ast_root)) {
        printf("Semantic analysis: SUCCESS\n");
    } else {
        printf("Semantic analysis: FAILED\n");
    }

    printf("\n=== Symbol Table ===\n");
    semantic_print_symbols();

    ast_free(ast_root);
    ast_root = NULL;
    semantic_reset();

    return EXIT_SUCCESS;
}
