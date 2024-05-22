#include <stdlib.h>

#include "lexer.h"
#include "parser.h"

void _print_ast(ast_node *node, uint8_t depth, char *identifier) {
    if (node) {
        uint8_t i;
        for (i = 0; i < depth * 2; ++i) {
            printf("| ");
        }
        printf("|-%s> ", identifier);
        switch (node->token->type) {
            case TOK_INVALID:
                printf("Invalid token\n");
                break;
            case TOK_START:
                printf("Start token\n");
                break;
            case TOK_INTEGER:
                printf("Integer Token: %llu\n", node->token->value.integer);
                break;
            case TOK_SEPARATOR:
                printf("Separator token: ");
                switch (node->token->value.separator) {
                    case SEP_OPEN_PAREN:
                        printf("\"(\"\n");
                        break;
                    case SEP_CLOSE_PAREN:
                        printf("\")\"\n");
                        break;
                    case SEP_SEMICOLON:
                        printf("\";\"\n");
                        break;
                }
                break;
            case TOK_KEYWORD:
                printf("Keyword token: ");
                switch (node->token->value.keyword) {
                    case KEY_EXIT:
                        printf("\"exit\"\n");
                        break;
                }
                break;
            case TOK_END:
                printf("End token\n");
                break;
        }

        _print_ast(node->left, depth + 1, "left");
        _print_ast(node->right, depth + 1, "right");
    }
}

void print_ast(abstract_syntax_tree *ast) {
    _print_ast(ast->root, 0, "root");
}

ast_node *build_tree(ast_node *parent, token_list *list) {
    ast_node *current_node = malloc(sizeof *current_node);

    current_node->parent = parent;
    current_node->token  = pull_token(list);
    current_node->left = current_node->right = NULL;

    if (current_node->token) {
        switch (current_node->token->type) {
            case TOK_INVALID:
                current_node->type = INVALID;
                break;
            case TOK_START:
                current_node->type = PROGRAM_START;
                current_node->left = build_tree(current_node, list);
                break;
            case TOK_INTEGER:
                current_node->type = TERMINAL_INTEGER;
                break;
            case TOK_SEPARATOR:
                switch (current_node->token->value.separator) {
                    case SEP_OPEN_PAREN:

                        current_node->left  = build_tree(current_node, list);
                        current_node->right = build_tree(current_node, list);
                        if (current_node->left->type == TERMINAL_INTEGER &&
                            current_node->right->type == TERMINAL_CLOSEPAREN) {
                            current_node->type = INTEGER_ARG;
                        }
                        break;
                    case SEP_CLOSE_PAREN:
                        current_node->type = TERMINAL_CLOSEPAREN;
                        break;
                    case SEP_SEMICOLON:
                        current_node->type = EXPRESSION_END;
                        break;
                }
                break;
            case TOK_KEYWORD:
                switch (current_node->token->value.keyword) {
                    case KEY_EXIT:
                        current_node->left  = build_tree(current_node, list);
                        current_node->right = build_tree(current_node, list);
                        if (current_node->left->type == INTEGER_ARG) {
                            current_node->type = SYSCALL;
                        } else {
                            current_node->type = INVALID;
                        }
                        break;
                }
                break;
            case TOK_END:
                current_node->type = PROGRAM_END;
                break;
        }
    }

    return current_node;
}

void syntax_verification(ast_node *root) {
    switch (root->type) {
        case OPEN_PAREN:
            printf("Open parentesis didn't found matching closing parentesis\n");
            return;
            break;
        case INTEGER_ARG:
            if (root->parent->type != SYSCALL) {
                printf("No system call to pass as an argument\n");
            }
            break;
        case EXPRESSION_END:
            if (root->parent->type != SYSCALL) {
                printf("Expression can't end without a system call\n");
            }
            break;
        case PROGRAM_START:
            if (!root->parent) {
                syntax_verification(root->left);
            } else {
                printf("Expected programm start\n");
                return;
            }
            break;
        case SYSCALL:
            if (root->parent->type == PROGRAM_START) {
                syntax_verification(root->left);
                if (root->left->type != INTEGER_ARG) {
                    printf("System call needs integer as an argument\n");
                    return;
                }
                syntax_verification(root->right);
                if (root->right->type != EXPRESSION_END) {
                    printf("System call expression cannot end without an argument\n");
                    return;
                }
            } else {
                printf("There can only be a system call at start of the programm\n");
            }
            break;
        default:
            break;
    }
}

abstract_syntax_tree *parse(token_list *tokens) {
    abstract_syntax_tree *ast = malloc(sizeof *ast);

    ast->root = build_tree(NULL, tokens);

    syntax_verification(ast->root);

    print_ast(ast);

    return ast;
}
