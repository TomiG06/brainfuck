#ifndef BACKEND_H
#define BACKEND_H

typedef struct {
    int (*init)(const char* fname);
    void (*emit_header)();
    void (*emit_inc)(size_t count);
    void (*emit_dec)(size_t count);
    void (*emit_right)(size_t count);
    void (*emit_left)(size_t count);
    void (*emit_loop_start)(size_t label);
    void (*emit_loop_end)(size_t label);
    void (*emit_output)();
    void (*emit_input)();
    void (*emit_footer)();
    void (*finalize)();
} compiler_backend;

#endif
