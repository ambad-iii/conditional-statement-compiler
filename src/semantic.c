
#include "semantic.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct Symbol {
    char *name;
    SemanticType type;
    struct Symbol *next;
} Symbol;

static Symbol *symbols = NULL;
static int error_count = 0;

static char *copy_text(const char *source)
{
    size_t length;
    char *copy;

    if (source == NULL)
        return NULL;

    length = strlen(source) + 1;
    copy = malloc(length);

    if (copy != NULL)
        memcpy(copy, source, length);

    return copy;
}

static const char *type_name(SemanticType type)
{
    switch (type) {
        case SEM_TYPE_NUMBER:
            return "number";

        case SEM_TYPE_BOOL:
            return "bool";

        default:
            return "unknown";
    }
}

static Symbol *find_symbol(const char *name)
{
    Symbol *current;

    if (name == NULL)
        return NULL;

    for (current = symbols; current != NULL;
         current = current->next) {
        if (strcmp(current->name, name) == 0)
            return current;
    }

    return NULL;
}

void semantic_reset(void)
{
    Symbol *current = symbols;

    while (current != NULL) {
        Symbol *next = current->next;

        free(current->name);
        free(current);

        current = next;
    }

    symbols = NULL;
    error_count = 0;
}

int semantic_declare_variable(const char *name, SemanticType type)
{
    Symbol *entry;

    if (name == NULL || name[0] == '\0')
        return 0;

    if (type == SEM_TYPE_UNKNOWN)
        return 0;

    if (find_symbol(name) != NULL)
        return 0;

    entry = calloc(1, sizeof(Symbol));

    if (entry == NULL)
        return 0;

    entry->name = copy_text(name);

    if (entry->name == NULL) {
        free(entry);
        return 0;
    }

    entry->type = type;
    entry->next = symbols;
    symbols = entry;

    return 1;
}

static void semantic_error(const char *message,
                           const char *detail)
{
    error_count++;

    if (detail != NULL) {
        fprintf(stderr, "Semantic Error: %s '%s'.\n",
                message, detail);
    } else {
        fprintf(stderr, "Semantic Error: %s.\n", message);
    }
}

static SemanticType expression_type(const ASTNode *node);

static int is_numeric(SemanticType type)
{
    return type == SEM_TYPE_NUMBER;
}

static int is_boolean(SemanticType type)
{
    return type == SEM_TYPE_BOOL;
}

/* Evaluate expressions made entirely from numeric constants. */
static int evaluate_constant_number(const ASTNode *node,
                                    double *result)
{
    double left;
    double right;

    if (node == NULL || result == NULL)
        return 0;

    switch (node->type) {
        case AST_NUMBER:
            *result = node->number;
            return 1;

        case AST_UNARY_OP:
            if (!evaluate_constant_number(node->left, &left))
                return 0;

            if (node->text == NULL)
                return 0;

            if (strcmp(node->text, "+") == 0) {
                *result = left;
                return 1;
            }

            if (strcmp(node->text, "-") == 0) {
                *result = -left;
                return 1;
            }

            return 0;

        case AST_BINARY_OP:
            if (node->text == NULL)
                return 0;

            if (!evaluate_constant_number(node->left, &left))
                return 0;

            if (!evaluate_constant_number(node->right, &right))
                return 0;

            if (strcmp(node->text, "+") == 0)
                *result = left + right;
            else if (strcmp(node->text, "-") == 0)
                *result = left - right;
            else if (strcmp(node->text, "*") == 0)
                *result = left * right;
            else if (strcmp(node->text, "/") == 0) {
                if (right == 0.0)
                    return 0;

                *result = left / right;
            } else {
                return 0;
            }

            return isfinite(*result);

        default:
            return 0;
    }
}

static SemanticType condition_type(const ASTNode *node)
{
    SemanticType type = expression_type(node);

    if (type != SEM_TYPE_BOOL &&
        type != SEM_TYPE_UNKNOWN) {
        semantic_error(
            "if-condition must be Boolean; use a comparison or logical expression",
            NULL
        );
    }

    return type;
}

static SemanticType expression_type(const ASTNode *node)
{
    SemanticType left;
    SemanticType right;
    Symbol *symbol;
    const char *op;

    if (node == NULL)
        return SEM_TYPE_UNKNOWN;

    switch (node->type) {
        case AST_NUMBER:
            return SEM_TYPE_NUMBER;

        case AST_BOOLEAN:
            return SEM_TYPE_BOOL;

        case AST_IDENTIFIER:
            symbol = find_symbol(node->text);

            if (symbol == NULL) {
                semantic_error(
                    "use of undeclared or unregistered variable",
                    node->text
                );
                return SEM_TYPE_UNKNOWN;
            }

            return symbol->type;

        case AST_UNARY_OP:
            op = node->text != NULL ? node->text : "";
            left = expression_type(node->left);

            if (strcmp(op, "!") == 0) {
                if (left != SEM_TYPE_UNKNOWN &&
                    !is_boolean(left)) {
                    semantic_error(
                        "logical NOT requires a Boolean operand",
                        NULL
                    );
                }

                return SEM_TYPE_BOOL;
            }

            if (strcmp(op, "+") == 0 ||
                strcmp(op, "-") == 0) {
                if (left != SEM_TYPE_UNKNOWN &&
                    !is_numeric(left)) {
                    semantic_error(
                        "unary plus or minus requires a numeric operand",
                        NULL
                    );
                }

                return SEM_TYPE_NUMBER;
            }

            semantic_error("unsupported unary operator", op);
            return SEM_TYPE_UNKNOWN;

        case AST_BINARY_OP:
            op = node->text != NULL ? node->text : "";

            left = expression_type(node->left);
            right = expression_type(node->right);

            if (strcmp(op, "+") == 0 ||
                strcmp(op, "-") == 0 ||
                strcmp(op, "*") == 0 ||
                strcmp(op, "/") == 0) {

                if (left != SEM_TYPE_UNKNOWN &&
                    !is_numeric(left)) {
                    semantic_error(
                        "arithmetic operator requires numeric operands",
                        op
                    );
                }

                if (right != SEM_TYPE_UNKNOWN &&
                    !is_numeric(right)) {
                    semantic_error(
                        "arithmetic operator requires numeric operands",
                        op
                    );
                }

                if (strcmp(op, "/") == 0) {
                    double divisor;

                    if (evaluate_constant_number(
                            node->right, &divisor) &&
                        divisor == 0.0) {
                        semantic_error(
                            "division by zero",
                            NULL
                        );
                    }
                }

                return SEM_TYPE_NUMBER;
            }

            if (strcmp(op, "<") == 0 ||
                strcmp(op, "<=") == 0 ||
                strcmp(op, ">") == 0 ||
                strcmp(op, ">=") == 0) {

                if (left != SEM_TYPE_UNKNOWN &&
                    !is_numeric(left)) {
                    semantic_error(
                        "relational operator requires numeric operands",
                        op
                    );
                }

                if (right != SEM_TYPE_UNKNOWN &&
                    !is_numeric(right)) {
                    semantic_error(
                        "relational operator requires numeric operands",
                        op
                    );
                }

                return SEM_TYPE_BOOL;
            }

            if (strcmp(op, "==") == 0 ||
                strcmp(op, "!=") == 0) {

                if (left != SEM_TYPE_UNKNOWN &&
                    right != SEM_TYPE_UNKNOWN &&
                    left != right) {
                    semantic_error(
                        "equality operands must have matching types",
                        op
                    );
                }

                return SEM_TYPE_BOOL;
            }

            if (strcmp(op, "&&") == 0 ||
                strcmp(op, "||") == 0) {

                if (left != SEM_TYPE_UNKNOWN &&
                    !is_boolean(left)) {
                    semantic_error(
                        "logical operator requires Boolean operands",
                        op
                    );
                }

                if (right != SEM_TYPE_UNKNOWN &&
                    !is_boolean(right)) {
                    semantic_error(
                        "logical operator requires Boolean operands",
                        op
                    );
                }

                return SEM_TYPE_BOOL;
            }

            semantic_error("unsupported binary operator", op);
            return SEM_TYPE_UNKNOWN;

        default:
            semantic_error(
                "statement node used where an expression was expected",
                NULL
            );
            return SEM_TYPE_UNKNOWN;
    }
}

static void analyze_statement(const ASTNode *node)
{
    const ASTNode *item;
    Symbol *target;
    SemanticType rhs;

    if (node == NULL)
        return;

    switch (node->type) {
        case AST_PROGRAM:
        case AST_BLOCK:
            for (item = node->left;
                 item != NULL;
                 item = item->next) {
                analyze_statement(item);
            }
            break;

        case AST_IF:
            condition_type(node->left);
            analyze_statement(node->right);
            break;

        case AST_IF_ELSE:
            condition_type(node->left);
            analyze_statement(node->right);
            analyze_statement(node->third);
            break;

        case AST_ASSIGN:
            rhs = expression_type(node->left);
            target = find_symbol(node->text);

            if (target == NULL) {
                semantic_error(
                    "assignment to undeclared or unregistered variable",
                    node->text
                );
            } else if (rhs != SEM_TYPE_UNKNOWN &&
                       target->type != rhs) {
                fprintf(
                    stderr,
                    "Semantic Error: cannot assign %s to '%s' of type %s.\n",
                    type_name(rhs),
                    node->text,
                    type_name(target->type)
                );

                error_count++;
            }
            break;

        case AST_IDENTIFIER:
        case AST_NUMBER:
        case AST_BOOLEAN:
        case AST_BINARY_OP:
        case AST_UNARY_OP:
            (void)expression_type(node);
            break;

        default:
            semantic_error("unknown AST node type", NULL);
            break;
    }
}

int semantic_analyze(const ASTNode *root)
{
    error_count = 0;

    if (root == NULL) {
        semantic_error("no valid syntax tree to analyze", NULL);
    } else {
        analyze_statement(root);
    }

    if (error_count == 0) {
        puts("Semantic analysis successful: no semantic errors found.");
        return 1;
    }

    printf("Semantic analysis failed: %d error(s) found.\n",
           error_count);

    return 0;
}

void semantic_print_symbols(void)
{
    const Symbol *current;

    puts("Symbol table:");

    if (symbols == NULL) {
        puts("  No variables registered.");
        return;
    }

    for (current = symbols;
         current != NULL;
         current = current->next) {
        printf("  %-16s %s\n",
               current->name,
               type_name(current->type));
    }
}
