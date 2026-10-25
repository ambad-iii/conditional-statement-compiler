
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "semantic.h"

extern int yyparse(void);
extern FILE *yyin;
extern ASTNode *ast_root;

static void usage(const char *program)
{
    fprintf(stderr, "Usage: %s <source-file>\n", program);
}

static int register_predefined_variables(void)
{
    if (!semantic_declare_variable("x", SEM_TYPE_NUMBER))
        return 0;

    if (!semantic_declare_variable("y", SEM_TYPE_NUMBER))
        return 0;

    if (!semantic_declare_variable("z", SEM_TYPE_NUMBER))
        return 0;

    if (!semantic_declare_variable("flag", SEM_TYPE_BOOL))
        return 0;

    return 1;
}

int main(int argc, char **argv)
{
    int parse_status;
    int semantic_status;
    int exit_status = EXIT_FAILURE;

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

    printf("Conditional Statement Compiler\n");

    if (!register_predefined_variables()) {
        fprintf(stderr,
                "Error: Failed to register predefined variables.\n");

        goto cleanup;
    }

    parse_status = yyparse();

    fclose(yyin);
    yyin = NULL;

    if (parse_status != 0 || ast_root == NULL) {
        fprintf(stderr,
                "\nSyntax analysis: FAILED\n");
        fprintf(stderr,
                "Please check the input for syntax errors.\n");

        goto cleanup;
    }

    printf("\nSyntax analysis: SUCCESS\n");

    printf("\nAbstract Syntax Tree (AST)\n");
    ast_print(ast_root);

    printf("\nAST JSON\n");
    ast_print_json(ast_root);
    printf("\n");

    printf("\nSemantic Analysis\n");

    semantic_status = semantic_analyze(ast_root);

    if (semantic_status) {
        printf("Semantic analysis: SUCCESS\n");
    } else {
        printf("Semantic analysis: FAILED\n");
    }

    printf("\nSymbol Table\n");
    semantic_print_symbols();

    if (semantic_status) {
        printf("\nCompilation completed successfully.\n");
        exit_status = EXIT_SUCCESS;
    } else {
        printf("\nCompilation failed during semantic analysis.\n");
    }

cleanup:
    if (yyin != NULL) {
        fclose(yyin);
        yyin = NULL;
    }

    ast_free(ast_root);
    ast_root = NULL;

    semantic_reset();

    return exit_status;
}
