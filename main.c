#include "incl/codegenerator.h"
#include "incl/lexer.h"
#include "incl/parser.h"
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Error: Argument expected.\nExpected Syntax: %s <filename>.pto\n", argv[0]);
        exit(1);
    } else if (*(strstr(argv[1], ".pto") + 4) != 0) {
        printf("Error: Wrong file format.\nExpected <filename>.pto");
    } else {
        FILE *file;

        file = fopen(argv[1], "r");

        if (!file) {
            printf("ERROR: File not found\n");
            exit(1);
        } else {
            token_list           *tokens;
            abstract_syntax_tree *ast;
            printf("\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
            printf("1st Step: Lexical analisis and tokenization...\n\n");
            tokens = lexer(file);
            printf("\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
            printf("\n\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
            printf("2nd Step: Syntax Verification and AST Generation...\n\n");
            ast = parse(tokens);
            printf("\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
            printf("\n\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
            printf("3rd Step: Output Code Generation...\n\n");
            generate_code(ast);
            printf("\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
        }
    }
}
