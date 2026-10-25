
#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"

typedef enum {
    SEM_TYPE_UNKNOWN,
    SEM_TYPE_NUMBER,
    SEM_TYPE_BOOL
} SemanticType;

/* Clear all registered variables and semantic errors. */
void semantic_reset(void);

/* Register a variable with its data type. */
int semantic_declare_variable(const char *name, SemanticType type);

/* Analyze the AST and return 1 if successful, otherwise 0. */
int semantic_analyze(const ASTNode *root);

/* Display the registered variables and their types. */
void semantic_print_symbols(void);

#endif
