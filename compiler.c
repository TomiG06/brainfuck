#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "brainfuck.h"

#include "backend.h"
#include "x86_64_backend.h"


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
    compiler_backend backend;
    size_t loop_count;
    size_t dead_depth;

    uint8_t* inst_p;
    uint8_t* start;

    FILE* f;
    char metWCmd;
};

size_t count_cmd(struct compilation_info* ci, char cmd) {
    size_t times = 0;
    size_t idx = ci->inst_p - ci->start;
    
    while(ci->start[idx++] == cmd) {
        times++;
    }

    ci->inst_p += times-1;

    return times;
}
   
void compile_cmd(struct compilation_info* ci) {
    size_t times = 0;
    char cmd = *ci->inst_p;

    if(ci->dead_depth && cmd != OPEN && cmd != CLOSE) return;

    switch(cmd) {
        case INC_P:
            times = count_cmd(ci, cmd);
            ci->backend.emit_right(times);
            break;
        case DEC_P:
            times = count_cmd(ci, cmd);
            ci->backend.emit_left(times);
            break;
        case INC_C:
            ci->metWCmd = 1;
            times = count_cmd(ci, cmd);
            ci->backend.emit_inc(times);
            break;
        case DEC_C:
            ci->metWCmd = 1;
            times = count_cmd(ci, cmd);
            ci->backend.emit_dec(times);
            break;
        case INPUT:
            ci->metWCmd = 1;
            ci->backend.emit_input();
            break;
        case OUTPUT:
            ci->backend.emit_output();
            break;
        case OPEN:
            if(ci->dead_depth || !ci->metWCmd) {
                ci->dead_depth++;
                return;
            }

            push(&ci->s, ci->loop_count);
            ci->backend.emit_loop_start(ci->loop_count);
            ci->loop_count++;
            break;
        case CLOSE:
            if(!ci->dead_depth) {
                size_t l = peek(&ci->s);
                ci->backend.emit_loop_end(l);
                pop(&ci->s);
            } else {
                ci->dead_depth--;
            }
            break;
        default:
            break;
    }
}

int compile(uint8_t* inst, const char* fname) {
    struct compilation_info ci = {};
    
    ci.f = fopen(fname, "w");
    ci.inst_p = inst;
    ci.start = inst;
    ci.loop_count = 0;
    ci.dead_depth = 0;
    ci.s.TOP = 0;
    ci.metWCmd = 0;
    ci.backend = create_x86_64_backend();

    if(ci.backend.init(fname)) {
        return 1;
    }

    while(*ci.inst_p) {
        compile_cmd(&ci);
        ci.inst_p++;
    }

    ci.backend.finalize();

    return 0;
}

