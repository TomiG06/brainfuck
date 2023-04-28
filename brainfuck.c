#include "brainfuck.h"
#include <stdlib.h>

void parse(FILE* f, uint8_t* inst) {
    char c;

    /*
       increment every time '[' is met
       decrement every time ']' is met

       when parsing is finished it must be 0
    */
    char bracks = 0;
    uint16_t idx = 0;

    while((c = fgetc(f)) != EOF) {
        if(
            c != INC_P && c != DEC_P  &&
            c != INC_C && c != DEC_C  &&
            c != INPUT && c != OUTPUT &&
            c != OPEN  && c != CLOSE
        ) continue;

        inst[idx++] = c;
        if(c == OPEN) bracks++;
        if(c == CLOSE) bracks--;
    }

    inst[idx] = 0;

    if(bracks) {
        printf("Parsing failed\n");
        exit(1);
    }

}
