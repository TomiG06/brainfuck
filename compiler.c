#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "brainfuck.h"

//The stack will help on loop handling
struct stack {
    int TOP;
    size_t data[2048];
};

void pop(struct stack* s) {
    if(s->TOP) s->TOP--;
}

void push(struct stack* s, size_t data) {
    s->data[s->TOP++] = data;
}

size_t peek(struct stack* s) {
    return s->data[s->TOP-1];
}

static size_t loop_count = 0;
static struct stack s = {0};
static uint8_t* inst_p = 0;
static uint8_t* start = 0;
static FILE* f;

void optimize(char cmd, char* buffer, char* times_is_one, char* times_is_not_one) {
    size_t times = 0;
    size_t idx = inst_p - start;
    
    while(start[idx++] == cmd) {
        times++;
    }

    inst_p += times-1;

    if(times == 1) strcpy(buffer, times_is_one);
    else sprintf(buffer, times_is_not_one, times);
}
   
void compile_cmd(char cmd) {
    char instruction[256] = {0};
    switch(cmd) {
        case INC_P:
            optimize(cmd, instruction, "inc edi\n", "add edi, %ld\n");
            break;
        case DEC_P:
            optimize(cmd, instruction, "dec edi\n", "sub edi, %ld\n");
            break;
        case INC_C:
            optimize(cmd, instruction, "inc byte[edi]\n", "add byte[edi], %ld\n");
            break;
        case DEC_C:
            optimize(cmd, instruction, "dec byte[edi]\n", "sub byte[edi], %ld\n");
            break;
        case INPUT:
            strcpy(instruction, "call getchar\n");
            break;
        case OUTPUT:
            strcpy(instruction, "call putchar\n");
            break;
        case OPEN:
            loop_count++;
            push(&s, loop_count);
            sprintf(instruction, "start%ld:\ncmp byte[edi], 0\nje end%ld\n", loop_count, loop_count);
            break;
        case CLOSE:
            sprintf(instruction, "cmp byte[edi], 0\njne start%ld\nend%ld:\n", peek(&s), peek(&s));
            pop(&s);
            break;
        default:
            break;
    }

    fwrite(instruction, strlen(instruction), 1, f);
}

static const char program_basis[] = 
    "global _start\n"
    "section .bss\n"
    "data_array resb 30000\n"   /* the main array */
    "section .text\n"
    "putchar:\n"                /*putchar function implementation*/
    "push dword[edi]\n"
    "mov ecx, esp\n"
    "mov edx, 1\n"
    "mov eax, 4\n"
    "mov ebx, 1\n"
    "int 0x80\n"
    "pop eax\n"
    "ret\n"
    "getchar:\n"                /*getchar function implementation*/
    "mov ecx, edi\n"
    "mov eax, 3\n"
    "mov ebx, 0\n"
    "mov edx, 1\n"
    "int 0x80\n"
    "ret\n"
    "exit:"                     /*exit function*/
    "mov eax, 1\n"
    "mov ebx, 0\n"
    "int 0x80\n"
    "_start:\n"
    "lea edi, [data_array]\n";  /*edi will be used as the data pointer*/


void compile(uint8_t* inst) {

    inst_p = inst;
    start = inst;

    f = fopen("b.asm", "w");

    fwrite(program_basis, strlen(program_basis), 1, f);

    while(*inst_p) {
        compile_cmd(*inst_p);
        inst_p++;
    }

    fwrite("\tcall exit\n", strlen("\tcall exit\n"), 1, f);


    fclose(f);

    /* assemble -> link -> remove source and object code */
    system("nasm -felf32 b.asm && ld -m elf_i386 -s -o b b.o && rm b.asm b.o");
}

