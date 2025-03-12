#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser/parser.h"
#include "parser/lexer.h"
#include "parser/analyse.h"

#include "meow/meow.h"
#include "meow/co.h"

#include "mwerror.h"

typedef struct _CMDArgs {
    int interactive;
    int web;
    int argc;
    char** argv;
    char* src;
} CMDArgs;

void interpret_src(const char* src) {
    init_mwerrors();
    Token** tokens = lex(src);
    ASTNode** nodes = parse(tokens);
    analyse_ast(nodes);
    if (size_mwerrors() > 0) {
        dump_mwerrors();
        free_mwerrors();
        return;
    }
    
    MeoWMCodeObject* co = co_new(nodes);
    // co_disasm(co);
    // exit(0);

    MeoWM* vm = meowm_new(co);
    int err = meowm_exec(vm);
    if (err) {}

    meowm_free(vm);
}

char* read_file(const char* path, size_t* length) {
    FILE* f;
    errno_t err = fopen_s(&f, path, "rb");
    if (err) {
        printf("Failed to open file.\n");
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    int fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (length)
        *length = fsize;

    char* buffer = (char*)malloc(fsize + 1);
    fread(buffer, 1, fsize, f);
    buffer[fsize] = '\0';

    fclose(f);

    return buffer;
}

void parse_args(CMDArgs* args, int argc, char** argv) {
    args->interactive = argc == 1;
    args->web = 0;
    args->argc = argc;
    args->argv = argv;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (argv[i][1] == 'w')
                args->web = 1;
        } else {
            args->src = argv[i];
        }
    }
}

int main(int argc, char* argv[]) {
    CMDArgs args;
    parse_args(&args, argc, argv);

    if (args.interactive) {
        printf("MeoWM v0.1\n");
        printf("Type \"exit\" to exit.\n\n");

        printf("Interactive mode is not implemented yet.\n");
        printf("Todo: Implement interactive mode.\n");
        return 0;

    } else if (args.src) {
        char* src = read_file(args.src, NULL);
        if (!src)
            return 1;

        interpret_src(src);
        free(src);
    } else if (args.web) {
        // Emscripten specific
    }  else {
        printf("Usage: meow [-w] [file]\n");
        return 1;
    }

    return 0;
}