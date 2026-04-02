#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "backend.h"

static FILE* f;
static const char* fasmname;

static const char* program_header = 
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

static const char* right_one_instruction = "inc r12\n";
static const char* right_mult_instruction = "add r12, %ld\n";

static const char* left_one_instruction = "dec r12\n";
static const char* left_mult_instruction = "sub r12, %ld\n";

static const char* inc_one_instruction = "inc byte ptr [r12]\n";
static const char* inc_mult_instruction = "add byte ptr [r12], %ld\n";

static const char* dec_one_instruction = "dec byte ptr [r12]\n";
static const char* dec_mult_instruction = "sub byte ptr [r12], %ld\n";

static const char* input_instruction = "call getchar\n";
static const char* output_instruction = "call putchar\n";

static const char* open_loop_instruction = "start%ld:\ncmp byte ptr [r12], 0\nje end%ld\n";
static const char* close_loop_instruction = "cmp byte ptr [r12], 0\njne start%ld\nend%ld:\n";

static const char* program_footer = "call exit\n";

// suppress pointless warnings

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"

// as a convention we expect a *.s filename

int x86_64_init(const char* fname) {
    f = fopen(fname, "w");
    if(!f) {
        return 1;
    }

    fasmname = fname;
    fputs(program_header, f);
    return 0;
}

void x86_64_emit_inc(size_t count) {
    if(count == 1) {
        fputs(inc_one_instruction, f);
    } else {
        fprintf(f, inc_mult_instruction, count);
    }
}

void x86_64_emit_dec(size_t count) {
    if(count == 1) {
        fputs(dec_one_instruction, f);
    } else {
        fprintf(f, dec_mult_instruction, count);
    }
}


void x86_64_emit_right(size_t count) {
    if(count == 1) {
        fputs(right_one_instruction, f);
    } else {
        fprintf(f, right_mult_instruction, count);
    }
}


void x86_64_emit_left(size_t count) {
    if(count == 1) {
        fputs(left_one_instruction, f);
    } else {
        fprintf(f, left_mult_instruction, count);
    }
}

void x86_64_emit_loop_start(size_t label) {
    fprintf(f, open_loop_instruction, label, label);
}

void x86_64_emit_loop_end(size_t label) {
    fprintf(f, close_loop_instruction, label, label);
}

#pragma GCC diagnostic pop

void x86_64_emit_input() {
    fputs(input_instruction, f);
}

void x86_64_emit_output() {
    fputs(output_instruction, f);
}

void x86_64_finalize() {
    char cmd[2048];
    char obj[256];
    char out[256];

    fputs(program_footer, f);
    fclose(f);
    
    snprintf(obj, sizeof(obj), "%s.o", fasmname);

    // sth.s -> s (output executable) (.s is the convention)
    strncpy(out, fasmname, sizeof(out));
    char* dot = strrchr(out, '.'); 
    if(dot) {
        *dot = '\0';
    }

    snprintf(cmd, sizeof(cmd), "as -o %s %s && ld -o %s %s && rm %s %s", obj, fasmname, out, obj, fasmname, obj);

    system(cmd);
}

compiler_backend create_x86_64_backend() {
    return (compiler_backend) {
        .init            = x86_64_init,
        .emit_inc        = x86_64_emit_inc,
        .emit_dec        = x86_64_emit_dec,
        .emit_right      = x86_64_emit_right,
        .emit_left       = x86_64_emit_left,
        .emit_loop_start = x86_64_emit_loop_start,
        .emit_loop_end   = x86_64_emit_loop_end,
        .emit_input      = x86_64_emit_input,
        .emit_output     = x86_64_emit_output,
        .finalize        = x86_64_finalize
    };
}


