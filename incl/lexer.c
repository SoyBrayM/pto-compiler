#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

struct _tok_node {
    token_list_node *next;
    token           *token;
};

struct _toklist {
    token_list_node *head;
    token_list_node *tail;
    uint16_t         size;
};

void _print_tokens(token_list *tokens) {
    token_list_node *current = tokens->head;
    while (current) {
        printf("Token: ");
        switch (current->token->type) {
            case TOK_START:
                printf("-=- STARTING TOKEN -=-\n");
                break;
            case TOK_INTEGER:
                printf("INTEGER TOKEN -> %llu\n", current->token->value.integer);
                break;
            case TOK_SEPARATOR:
                printf("SEPARATOR TOKEN -> \"");
                switch (current->token->value.separator) {
                    case SEP_OPEN_PAREN:
                        printf("(");
                        break;
                    case SEP_CLOSE_PAREN:
                        printf(")");
                        break;
                    case SEP_SEMICOLON:
                        printf(";");
                        break;
                    default:
                        printf("UNEXPECTED %8s", current->token->value._str);
                        break;
                }
                printf("\"\n");
                break;
            case TOK_KEYWORD:
                printf("KEYWORD TOKEN -> \"");
                switch (current->token->value.keyword) {
                    case KEY_EXIT:
                        printf("exit");
                        break;
                    default:
                        printf("UNEXPECTED %8s", current->token->value._str);
                        break;
                }
                printf("\"\n");
                break;
            case TOK_END:
                printf("-=- ENDING TOKEN -=-\n");
                break;
            case TOK_INVALID:
            default:
                printf("INVALID TOKEN -> %8s\n", current->token->value._str);
                break;
        }
        current = current->next;
    }
}

token *integer_token(char **buffer) {
    token *new_token = malloc(sizeof *new_token);
    char  *str       = malloc(64 * sizeof *str);

    sscanf(*buffer, "%63[0-9]", str);
    *buffer += strlen(str);

    if (strlen(str)) {
        new_token->type = TOK_INTEGER;
        sscanf(str, "%llu", &new_token->value.integer);
    } else {
        new_token->type = TOK_INVALID;
        strncpy(new_token->value._str, str, 7);
    }

    free(str);
    return new_token;
}

token *keyword_token(char **buffer) {
    token *new_token = malloc(sizeof *new_token);
    char  *str       = malloc(64 * sizeof *str);

    sscanf(*buffer, "%63[a-zA-Z]", str);
    *buffer += strlen(str);

    /* The search could be optimized with a hash map of all posible keywords */
    if (!strcmp(str, "exit")) {
        new_token->type          = TOK_KEYWORD;
        new_token->value.keyword = KEY_EXIT;
        goto out;
    }

    /* More keywords would follow here. */

    new_token->type = TOK_INVALID;
    strncpy(new_token->value._str, str, 7);

out:
    free(str);
    return new_token;
}

token *separator_token(char **buffer) {
    token *new_token = malloc(sizeof *new_token);

    switch (**buffer) {
        case '(':
            new_token->type            = TOK_SEPARATOR;
            new_token->value.separator = SEP_OPEN_PAREN;
            break;
        case ')':
            new_token->type            = TOK_SEPARATOR;
            new_token->value.separator = SEP_CLOSE_PAREN;
            break;
        case ';':
            new_token->type            = TOK_SEPARATOR;
            new_token->value.separator = SEP_SEMICOLON;
            break;
        default:
            new_token->type = TOK_INVALID;
            strncpy(new_token->value._str, *buffer, 1);
            break;
    }

    *buffer += 1;
    return new_token;
}

uint8_t append_to_token_list(token_list *list, token *tok) {
    if (list || tok) {
        token_list_node *new_node = malloc(sizeof *new_node);

        new_node->token = tok;

        if (list->size) {
            list->tail->next = new_node;
        } else {
            list->head = new_node;
        }

        list->tail = new_node;
        list->size++;

        return 1;
    }
    return 0;
}

token_list *lexer(FILE *file) {
    token_list *tokens  = calloc(1, sizeof *tokens);
    token      *current = malloc(sizeof *current);
    char       *start, *buffer;

    current->type = TOK_START;

    append_to_token_list(tokens, current);

    start = buffer = malloc(1024);

    while (fgets(buffer, 1024, file)) {
        while (*buffer) {
            switch (*buffer) {
                case ';':
                case '(':
                case ')':
                    current = separator_token(&buffer);

                    append_to_token_list(tokens, current);
                    break;
                default:
                    if (isdigit(*buffer)) {
                        current = integer_token(&buffer);

                        append_to_token_list(tokens, current);
                    } else if (isalpha(*buffer)) {
                        current = keyword_token(&buffer);

                        append_to_token_list(tokens, current);
                    } else {
                        buffer++;
                    }
                    break;
            }
        }
        buffer = start;
    }

    _print_tokens(tokens);
    return tokens;
}

token *pull_token(token_list *list) {
    if (list && list->head) {
        token_list_node *head_node;
        token           *tok;

        if (list->size == 1) {
            list->tail = NULL;
        }

        head_node       = list->head;
        list->head      = head_node->next;
        head_node->next = NULL;

        tok = head_node->token;

        free(head_node);

        return tok;
    }
    return NULL;
}
