#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"

typedef enum {
    SEM_TYPE_UNKNOWN,
    SEM_TYPE_NUMBER,
    SEM_TYPE_BOOL
} SemanticType;

/* Clear the symbol table before analyzing a new input program. */
void semantic_reset(void);

/* Register a variable. The current language has no declaration syntax, so
   the driver or a future declaration rule must register names explicitly. */
int semantic_declare_variable(const char *name, SemanticType type);

/* Return 1 when semantic checks pass; otherwise return 0. */
int semantic_analyze(const ASTNode *root);
void semantic_print_symbols(void);

#endif
