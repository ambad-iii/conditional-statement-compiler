#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *copy_string(const char *source)
{
    size_t length;
    char *copy;

    if (source == NULL)
        return NULL;

    length = strlen(source) + 1;
    copy = (char *)malloc(length);

    if (copy != NULL)
        memcpy(copy, source, length);

    return copy;
}

static ASTNode *new_node(ASTNodeType type, const char *text,
                         ASTNode *left, ASTNode *right, ASTNode *third)
{
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

ASTNode *ast_program(ASTNode *statements)
{
    return new_node(AST_PROGRAM, "PROGRAM", statements, NULL, NULL);
}

ASTNode *ast_block(ASTNode *statements)
{
    return new_node(AST_BLOCK, "BLOCK", statements, NULL, NULL);
}

ASTNode *ast_if(ASTNode *condition, ASTNode *then_branch)
{
    return new_node(AST_IF, "IF", condition, then_branch, NULL);
}

ASTNode *ast_if_else(ASTNode *condition, ASTNode *then_branch,
                     ASTNode *else_branch)
{
    return new_node(AST_IF_ELSE, "IF-ELSE", condition,
                    then_branch, else_branch);
}

ASTNode *ast_assign(const char *name, ASTNode *expression)
{
    return new_node(AST_ASSIGN, name, expression, NULL, NULL);
}

ASTNode *ast_binary(const char *operator_text,
                    ASTNode *left, ASTNode *right)
{
    return new_node(AST_BINARY_OP, operator_text, left, right, NULL);
}

ASTNode *ast_unary(const char *operator_text, ASTNode *operand)
{
    return new_node(AST_UNARY_OP, operator_text, operand, NULL, NULL);
}

ASTNode *ast_identifier(const char *name)
{
    return new_node(AST_IDENTIFIER, name, NULL, NULL, NULL);
}

ASTNode *ast_number(double value)
{
    ASTNode *node = new_node(AST_NUMBER, NULL, NULL, NULL, NULL);
    node->number = value;
    return node;
}

ASTNode *ast_boolean(int value)
{
    ASTNode *node = new_node(AST_BOOLEAN, NULL, NULL, NULL, NULL);
    node->boolean_value = value != 0;
    return node;
}

void ast_append(ASTNode **list, ASTNode *node)
{
    ASTNode *tail;

    if (list == NULL || node == NULL)
        return;

    if (*list == NULL) {
        *list = node;
        return;
    }

    tail = *list;

    while (tail->next != NULL)
        tail = tail->next;

    tail->next = node;
}

static const char *type_name(ASTNodeType type)
{
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
        default: return "UNKNOWN";
    }
}

/* Terminal tree printer */

static void print_tree(const ASTNode *node, const char *prefix,
                       int is_last, int is_root);

static void print_labeled_subtree(const char *label,
                                  const ASTNode *child,
                                  const char *prefix,
                                  int is_last)
{
    char child_prefix[1024];

    printf("%s%s%s\n", prefix,
           is_last ? "└── " : "├── ", label);

    snprintf(child_prefix, sizeof(child_prefix), "%s%s",
             prefix, is_last ? "    " : "│   ");

    print_tree(child, child_prefix, 1, 0);
}

static void print_tree(const ASTNode *node, const char *prefix,
                       int is_last, int is_root)
{
    char child_prefix[1024];
    const ASTNode *item;

    if (node == NULL)
        return;

    if (!is_root)
        printf("%s%s", prefix, is_last ? "└── " : "├── ");

    printf("%s", type_name(node->type));

    if (node->type == AST_NUMBER)
        printf(": %.15g", node->number);
    else if (node->type == AST_BOOLEAN)
        printf(": %s", node->boolean_value ? "true" : "false");
    else if (node->text != NULL)
        printf(": %s", node->text);

    putchar('\n');

    if (is_root)
        snprintf(child_prefix, sizeof(child_prefix), "%s", prefix);
    else
        snprintf(child_prefix, sizeof(child_prefix), "%s%s",
                 prefix, is_last ? "    " : "│   ");

    switch (node->type) {
        case AST_PROGRAM:
        case AST_BLOCK:
            item = node->left;

            while (item != NULL) {
                print_tree(item, child_prefix,
                           item->next == NULL, 0);
                item = item->next;
            }
            break;

        case AST_IF:
            print_labeled_subtree("CONDITION", node->left,
                                  child_prefix, 0);
            print_labeled_subtree("THEN", node->right,
                                  child_prefix, 1);
            break;

        case AST_IF_ELSE:
            print_labeled_subtree("CONDITION", node->left,
                                  child_prefix, 0);
            print_labeled_subtree("THEN", node->right,
                                  child_prefix, 0);
            print_labeled_subtree("ELSE", node->third,
                                  child_prefix, 1);
            break;

        case AST_ASSIGN:
            print_tree(node->left, child_prefix, 1, 0);
            break;

        case AST_BINARY_OP:
            print_tree(node->left, child_prefix,
                       node->right == NULL, 0);
            print_tree(node->right, child_prefix, 1, 0);
            break;

        case AST_UNARY_OP:
            print_tree(node->left, child_prefix, 1, 0);
            break;

        case AST_IDENTIFIER:
        case AST_NUMBER:
        case AST_BOOLEAN:
            break;
    }
}

void ast_print(const ASTNode *root)
{
    print_tree(root, "", 1, 1);
}

/* JSON printer for the Flask GUI */

static void json_string(const char *text)
{
    const unsigned char *p;

    putchar('"');

    if (text != NULL) {
        for (p = (const unsigned char *)text; *p; p++) {
            switch (*p) {
                case '"':
                    printf("\\\"");
                    break;

                case '\\':
                    printf("\\\\");
                    break;

                case '\n':
                    printf("\\n");
                    break;

                case '\r':
                    printf("\\r");
                    break;

                case '\t':
                    printf("\\t");
                    break;

                default:
                    if (*p < 32)
                        printf("\\u%04x", *p);
                    else
                        putchar(*p);
            }
        }
    }

    putchar('"');
}

static const char *json_type_name(ASTNodeType type)
{
    return type_name(type);
}

static void json_node(const ASTNode *node);

static void json_children(const ASTNode *node)
{
    int comma = 0;
    const ASTNode *child;

    printf("[");

    if (node->type == AST_PROGRAM || node->type == AST_BLOCK) {
        for (child = node->left; child != NULL; child = child->next) {
            if (comma)
                printf(",");

            json_node(child);
            comma = 1;
        }
    } else {
        const ASTNode *children[3] = {NULL, NULL, NULL};
        int count = 0;
        int i;

        switch (node->type) {
            case AST_IF:
                children[count++] = node->left;
                children[count++] = node->right;
                break;

            case AST_IF_ELSE:
                children[count++] = node->left;
                children[count++] = node->right;
                children[count++] = node->third;
                break;

            case AST_ASSIGN:
            case AST_UNARY_OP:
                children[count++] = node->left;
                break;

            case AST_BINARY_OP:
                children[count++] = node->left;
                children[count++] = node->right;
                break;

            default:
                break;
        }

        for (i = 0; i < count; i++) {
            if (children[i] == NULL)
                continue;

            if (comma)
                printf(",");

            json_node(children[i]);
            comma = 1;
        }
    }

    printf("]");
}

static void json_node(const ASTNode *node)
{
    if (node == NULL) {
        printf("null");
        return;
    }

    printf("{\"type\":");
    json_string(json_type_name(node->type));

    printf(",\"text\":");
    json_string(node->text);

    printf(",\"number\":%.15g", node->number);

    printf(",\"boolean\":%s",
           node->boolean_value ? "true" : "false");

    printf(",\"children\":");
    json_children(node);

    printf("}");
}

void ast_print_json(const ASTNode *root)
{
    json_node(root);
    putchar('\n');
}

/* Free the AST */

void ast_free(ASTNode *root)
{
    if (root == NULL)
        return;

    ast_free(root->left);
    ast_free(root->right);
    ast_free(root->third);
    ast_free(root->next);

    free(root->text);
    free(root);
}
