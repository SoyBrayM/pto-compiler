#include "codegenerator.h"
#include "parser.h"

void traverse_ast(ast_node *node, FILE *file) {
    if (node) {
        switch (node->type) {
            case EXPRESSION_END:
                printf("  syscall\n");
                fprintf(file, "  syscall\n");
                break;
            case PROGRAM_START:
                printf("section .text\n");
                printf("  global _start\n\n");
                printf("_start:\n");
                fprintf(file, "section .text\n");
                fprintf(file, "  global _start\n\n");
                fprintf(file, "_start:\n");
                break;
            case SYSCALL:
                printf("  mov rax, 60\n");
                fprintf(file, "  mov rax, 60\n");
                break;
            case INTEGER_ARG:
                printf("  mov rdi, %llu\n", node->left->token->value.integer);
                fprintf(file, "  mov rdi, %llu\n", node->left->token->value.integer);
                break;
            default:
                break;
        }
        traverse_ast(node->left, file);
        traverse_ast(node->right, file);
    }
}

void generate_code(abstract_syntax_tree *ast) {
    FILE *output;
    remove("generated_assembly.asm");

    output = fopen("generated_assembly.asm", "w");

    traverse_ast(ast->root, output);
}
