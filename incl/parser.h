#ifndef _PARSER_H_
#define _PARSER_H_

#include "lexer.h"

typedef struct _nast ast_node;

typedef enum {
    TERMINAL_CLOSEPAREN,
    TERMINAL_INTEGER,
    OPEN_PAREN,
    INTEGER_ARG,
    EXPRESSION_END,
    PROGRAM_START,
    PROGRAM_END,
    INVALID,
    SYSCALL
} node_type;

struct _nast {
    ast_node *parent;
    ast_node *left;
    ast_node *right;
    node_type type;
    token    *token;
};

typedef struct _ast {
    ast_node *root;
} abstract_syntax_tree;

abstract_syntax_tree *parse(token_list *);

#endif /* ifndef _PARSER_H_ */
