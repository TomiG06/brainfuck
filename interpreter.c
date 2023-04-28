#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include "brainfuck.h"

uint8_t data[30000] = {0};
uint8_t* data_p = data;

uint8_t* inst_p = 0;

void execute_cmd(char cmd) {
    switch(cmd) {
        case INC_P:
            data_p++;
            break;
        case DEC_P:
            data_p--;
            break;
        case INC_C:
            (*data_p)++;
            break;
        case DEC_C:
            (*data_p)--;
            break;
        case INPUT:
            *(data_p) = getchar();
            break;
        case OUTPUT:
            putchar(*data_p);
            break;
        case OPEN:
            {
                int loops = 1; //initial one because we are already in one

                if(!(*data_p)) {
                    while(loops) {
                        inst_p++;
                        if(*inst_p == OPEN) loops++;
                        else if(*inst_p == CLOSE) loops--;
                    }
                }
            }
            break;
        case CLOSE:
            {
                int loops = 1; //Same

                if(*data_p) {
                    while(loops) {
                        inst_p--;
                        if(*inst_p == CLOSE) loops++;
                        else if(*inst_p == OPEN) loops--;
                    }
                }
            }
            break;
        default:
            break;
    }

    return;
}

void interpret(uint8_t* inst) {

    inst_p = inst;

    while(*inst_p) {
        execute_cmd(*inst_p);
        inst_p++;
    }


}

