#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "brainfuck.h"


//The stack will help on loop handling
struct stack {
    size_t data[2048];
    int TOP;
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

struct compilation_info {
    struct stack s;
    size_t loop_count;
    size_t dead_depth;

    uint8_t* inst_p;
    uint8_t* start;

    FILE* f;
    char metWCmd;
};

void optimize(struct compilation_info* ci, char cmd, char* buffer, char* times_is_one, char* times_is_not_one) {
    size_t times = 0;
    size_t idx = ci->inst_p - ci->start;
    
    while(ci->start[idx++] == cmd) {
        times++;
    }

    ci->inst_p += times-1;

    if(times == 1) strcpy(buffer, times_is_one);
    else sprintf(buffer, times_is_not_one, times);
}
   
void compile_cmd(struct compilation_info* ci) {
    char instruction[256] = {0};
    char cmd = *ci->inst_p;

    if(ci->dead_depth && cmd != OPEN && cmd != CLOSE) return;

    switch(cmd) {
        case INC_P:
            optimize(ci, cmd, instruction, "inc r12\n", "add r12, %ld\n");
            break;
        case DEC_P:
            optimize(ci, cmd, instruction, "dec r12\n", "sub r12, %ld\n");
            break;
        case INC_C:
            ci->metWCmd = 1;
            optimize(ci, cmd, instruction, "inc byte ptr [r12]\n", "add byte ptr [r12], %ld\n");
            break;
        case DEC_C:
            ci->metWCmd = 1;
            optimize(ci, cmd, instruction, "dec byte ptr [r12]\n", "sub byte ptr [r12], %ld\n");
            break;
        case INPUT:
            ci->metWCmd = 1;
            strcpy(instruction, "call getchar\n");
            break;
        case OUTPUT:
            strcpy(instruction, "call putchar\n");
            break;
        case OPEN:
            if(ci->dead_depth || !ci->metWCmd) {
                ci->dead_depth++;
                return;
            }

            push(&ci->s, ci->loop_count);
            sprintf(instruction, "start%ld:\ncmp byte ptr [r12], 0\nje end%ld\n", ci->loop_count, ci->loop_count);
            ci->loop_count++;
            break;
        case CLOSE:
            if(!ci->dead_depth) {
                size_t l = peek(&ci->s);
                sprintf(instruction, "cmp byte ptr [r12], 0\njne start%ld\nend%ld:\n", l, l);
                pop(&ci->s);
            } else {
                ci->dead_depth--;
            }
            break;
        default:
            break;
    }

    fwrite(instruction, strlen(instruction), 1, ci->f);
}

static const char program_basis[] = 
    ".intel_syntax noprefix\n"
    ".global _start\n"
    ".section .bss\n"
    "data_array: .zero 30000\n"     /* the main array */
    ".section .text\n"
    "putchar:\n"                    /* putchar function implementation */
    "mov rax, 1\n"                  /* write syscall */
    "mov rdi, 1\n"                  /* stdout */
    "mov rsi, r12\n"                /* buf = current cell address */
    "mov rdx, 1\n"                  /* len = 1 */
    "syscall\n"
    "ret\n"
    "getchar:\n"                    /* getchar function implementation */
    "mov rax, 0\n"                  /* read syscall */
    "mov rdi, 0\n"                  /* stdin */
    "mov rsi, r12\n"                /* buf = current cell address */
    "mov rdx, 1\n"                  /* len = 1 */
    "syscall\n"
    "ret\n"
    "exit:\n"                       /* exit function */
    "mov rax, 60\n"                 /* exit syscall */
    "xor rdi, rdi\n"                /* exit code 0 */
    "syscall\n"
    "_start:\n"
    "lea r12, [rip + data_array]\n"; /* r12 will be used as the data pointer */


void compile(uint8_t* inst) {
    struct compilation_info ci = {};
    
    ci.f = fopen("b.s", "w");
    
    ci.inst_p = inst;
    ci.start = inst;
    
    ci.loop_count = 0;
    ci.dead_depth = 0;
    ci.s.TOP = 0;
    
    ci.metWCmd = 0;

    fwrite(program_basis, strlen(program_basis), 1, ci.f);

    while(*ci.inst_p) {
        compile_cmd(&ci);
        ci.inst_p++;
    }

    fwrite("call exit\n", strlen("call exit\n"), 1, ci.f);
    fclose(ci.f);

    /* assemble -> link -> remove source and object code */
    system("as -o b.o b.s && ld -o b b.o && rm b.o");
}

