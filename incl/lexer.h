#ifndef _LEXER_H_
#define _LEXER_H_

#include <inttypes.h>
#include <stdio.h>

typedef struct _tok      token;
typedef struct _tok_node token_list_node;
typedef struct _toklist  token_list;

typedef enum {
    TOK_INVALID,
    TOK_START,
    TOK_INTEGER,
    TOK_SEPARATOR,
    TOK_KEYWORD,
    TOK_END
} token_type;

typedef enum {
    KEY_EXIT
} keyword_value;

typedef enum {
    SEP_OPEN_PAREN,
    SEP_CLOSE_PAREN,
    SEP_SEMICOLON
} separator_value;

struct _tok {
    token_type type;
    union {
        keyword_value   keyword;
        separator_value separator;
        uint64_t        integer;
        char            _str[8];
    } value;
};

token      *pull_token(token_list *list);
token_list *lexer(FILE *file);

#endif /* ifndef LEXER_H */
