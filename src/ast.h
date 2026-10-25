#ifndef AST_H
#define AST_H

#include <stddef.h>

typedef enum {
    AST_PROGRAM,
    AST_BLOCK,
    AST_IF,
    AST_IF_ELSE,
    AST_ASSIGN,
    AST_BINARY_OP,
    AST_UNARY_OP,
    AST_IDENTIFIER,
    AST_NUMBER,
    AST_BOOLEAN
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    char *text;                 
    double number;              
    int boolean_value;          
    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *third;
    struct ASTNode *next;       
} ASTNode;

ASTNode *ast_program(ASTNode *statements);
ASTNode *ast_block(ASTNode *statements);
ASTNode *ast_if(ASTNode *condition, ASTNode *then_branch);
ASTNode *ast_if_else(ASTNode *condition, ASTNode *then_branch,
                     ASTNode *else_branch);
ASTNode *ast_assign(const char *name, ASTNode *expression);
ASTNode *ast_binary(const char *operator_text, ASTNode *left, ASTNode *right);
ASTNode *ast_unary(const char *operator_text, ASTNode *operand);
ASTNode *ast_identifier(const char *name);
ASTNode *ast_number(double value);
ASTNode *ast_boolean(int value);
void ast_print_json(const ASTNode *root);
void ast_append(ASTNode **list, ASTNode *node);
void ast_print(const ASTNode *root);
void ast_free(ASTNode *root);

#endif