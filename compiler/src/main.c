#include <lexer.h>
#include <print.h>
#include <error.h>
#include <parser.h>

#include <stdlib.h>

int main() {
    /// Init
    print_init();
    catch_init();
    /// Source Read
    char *src;
    {
        FILE *f = fopen("../examples/test.kpl", "rb");
        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET); 

        src = malloc(fsize + 1);
        fread(src, fsize, 1, f);
        // fclose(f);

        src[fsize] = '\0';
    }
    /// Source Print
    // printf("Source Read:\n%s\n", src);
    /// Lexer
    token_t* tokens = lexer_lex(src);
    /// Parser
    ast_context_t* context = parser_parse(tokens);
    /// Parser Print
    printf("\nParser Result:\n");
    ast_context_print(0, context);
    // /// Annotation Processor
    // ap_process_context(context);
    // /// Annotation Processor Print
    // printf("\nAnnotation Processor Result:\n");
    // ast_context_print(0, context);
}