#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include "brainfuck.h"

enum {
    compile_c,
    interpret_c
};

uint8_t inst[4096] = {0};

void cleanFilename(char* fname) {
    char *dot = strrchr(fname, '.');
    if(dot) *dot = '\0';
}

int main(int argc, char* argv[]) {

    uint8_t action;

    if(argc != 3) {
        printf("Usage: %s [-i/-c] [filename]\n", argv[0]);
        return 1;
    }

    if(access(argv[2], F_OK)) {
        printf("File '%s' not fount\n", argv[2]);
        return 1;
    }

    if(!strcmp(argv[1], "-i")) {
        action = interpret_c;
    } else if(!strcmp(argv[1], "-c")) {
        action = compile_c;
    } else {
        printf("Invalid action: %s\n", argv[1]);
        return 1;
    }

    FILE* f = fopen(argv[2], "r");
    parse(f, inst);
    fclose(f);

    if(action == interpret_c) {
        interpret(inst);
    } else {
        cleanFilename(argv[2]);
        int err = compile(inst, argv[2]);
        if(err) {
             printf("Could not create output file\n");
             return 1;
        }
    }

    return 0;
}

