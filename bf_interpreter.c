#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#define INC_P       '>'     //increment pointer
#define DEC_P       '<'     //decrement pointer 
#define INC_C       '+'     //increment cell
#define DEC_C       '-'     //decrement cell
#define INPUT       ','
#define OUTPUT      '.'
#define OPEN        '['
#define CLOSE       ']'

uint8_t data[30000] = {0};
uint8_t* data_p = data;

uint8_t inst[4096] = {0};
uint8_t* inst_p = inst;

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

int main(int argc, char* argv[]) {
    if(argc != 2) {
        printf("Usage: %s [filename]", argv[0]);
        return 1;
    }

    if(access(argv[1], F_OK)) {
        printf("File '%s' not fount\n", argv[1]);
        return 1;
    }

    FILE* f = fopen(argv[1], "r");

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
        return 1;
    }

    while(*inst_p) {
        execute_cmd(*inst_p);
        inst_p++;
    }

    fclose(f);

    return 0;
}

