#include "ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *copy_string(const char *source) {
    size_t length;
    char *copy;
    if (source == NULL) return NULL;
    length = strlen(source) + 1;
    copy = (char *)malloc(length);
    if (copy != NULL) memcpy(copy, source, length);
    return copy;
}

static ASTNode *new_node(ASTNodeType type, const char *text,
                         ASTNode *left, ASTNode *right, ASTNode *third) {
    ASTNode *node = (ASTNode *)calloc(1, sizeof(ASTNode));
    if (node == NULL) {
        fprintf(stderr, "AST error: out of memory.\n");
        exit(EXIT_FAILURE);
    }
    node->type = type;
    node->text = copy_string(text);
    node->left = left;
    node->right = right;
    node->third = third;
    return node;
}

ASTNode *ast_program(ASTNode *statements) {
    return new_node(AST_PROGRAM, "PROGRAM", statements, NULL, NULL);
}

ASTNode *ast_block(ASTNode *statements) {
    return new_node(AST_BLOCK, "BLOCK", statements, NULL, NULL);
}

ASTNode *ast_if(ASTNode *condition, ASTNode *then_branch) {
    return new_node(AST_IF, "IF", condition, then_branch, NULL);
}

ASTNode *ast_if_else(ASTNode *condition, ASTNode *then_branch,
                     ASTNode *else_branch) {
    return new_node(AST_IF_ELSE, "IF-ELSE", condition, then_branch,
                    else_branch);
}

ASTNode *ast_assign(const char *name, ASTNode *expression) {
    return new_node(AST_ASSIGN, name, expression, NULL, NULL);
}

ASTNode *ast_binary(const char *operator_text, ASTNode *left, ASTNode *right) {
    return new_node(AST_BINARY_OP, operator_text, left, right, NULL);
}

ASTNode *ast_unary(const char *operator_text, ASTNode *operand) {
    return new_node(AST_UNARY_OP, operator_text, operand, NULL, NULL);
}

ASTNode *ast_identifier(const char *name) {
    return new_node(AST_IDENTIFIER, name, NULL, NULL, NULL);
}

ASTNode *ast_number(double value) {
    ASTNode *node = new_node(AST_NUMBER, NULL, NULL, NULL, NULL);
    node->number = value;
    return node;
}

ASTNode *ast_boolean(int value) {
    ASTNode *node = new_node(AST_BOOLEAN, NULL, NULL, NULL, NULL);
    node->boolean_value = value != 0;
    return node;
}

void ast_append(ASTNode **list, ASTNode *node) {
    ASTNode *tail;
    if (list == NULL || node == NULL) return;
    if (*list == NULL) {
        *list = node;
        return;
    }
    tail = *list;
    while (tail->next != NULL) tail = tail->next;
    tail->next = node;
}

static const char *type_name(ASTNodeType type) {
    switch (type) {
        case AST_PROGRAM: return "PROGRAM";
        case AST_BLOCK: return "BLOCK";
        case AST_IF: return "IF";
        case AST_IF_ELSE: return "IF-ELSE";
        case AST_ASSIGN: return "ASSIGN";
        case AST_BINARY_OP: return "BINARY_OP";
        case AST_UNARY_OP: return "UNARY_OP";
        case AST_IDENTIFIER: return "IDENTIFIER";
        case AST_NUMBER: return "NUMBER";
        case AST_BOOLEAN: return "BOOLEAN";
    }
    return "UNKNOWN";
}

static void print_tree(const ASTNode *node, int depth) {
    int i;
    if (node == NULL) return;
    for (i = 0; i < depth; ++i) printf("  ");
    printf("%s", type_name(node->type));
    if (node->type == AST_NUMBER) printf(": %.15g", node->number);
    else if (node->type == AST_BOOLEAN)
        printf(": %s", node->boolean_value ? "true" : "false");
    else if (node->text != NULL) printf(": %s", node->text);
    putchar('\n');

    switch (node->type) {
        case AST_PROGRAM:
        case AST_BLOCK:
            print_tree(node->left, depth + 1);
            break;
        case AST_IF:
            for (i = 0; i < depth + 1; ++i) printf("  ");
            puts("CONDITION");
            print_tree(node->left, depth + 2);
            for (i = 0; i < depth + 1; ++i) printf("  ");
            puts("THEN");
            print_tree(node->right, depth + 2);
            break;
        case AST_IF_ELSE:
            for (i = 0; i < depth + 1; ++i) printf("  ");
            puts("CONDITION");
            print_tree(node->left, depth + 2);
            for (i = 0; i < depth + 1; ++i) printf("  ");
            puts("THEN");
            print_tree(node->right, depth + 2);
            for (i = 0; i < depth + 1; ++i) printf("  ");
            puts("ELSE");
            print_tree(node->third, depth + 2);
            break;
        case AST_ASSIGN:
            print_tree(node->left, depth + 1);
            break;
        case AST_BINARY_OP:
            print_tree(node->left, depth + 1);
            print_tree(node->right, depth + 1);
            break;
        case AST_UNARY_OP:
            print_tree(node->left, depth + 1);
            break;
        case AST_IDENTIFIER:
        case AST_NUMBER:
        case AST_BOOLEAN:
            break;
    }
    if (node->type == AST_PROGRAM || node->type == AST_BLOCK) {
        const ASTNode *item = node->left;
        if (item != NULL) {
            item = item->next;
            while (item != NULL) {
                print_tree(item, depth + 1);
                item = item->next;
            }
        }
    }
}

void ast_print(const ASTNode *root) {
    print_tree(root, 0);
}

void ast_free(ASTNode *root) {
    if (root == NULL) return;
    /* Children are owned by the parent; next is the statement-list link. */
    ast_free(root->left);
    ast_free(root->right);
    ast_free(root->third);
    ast_free(root->next);
    free(root->text);
    free(root);
}