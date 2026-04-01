#ifndef BACKEND_H
#define BACKEND_H

typedef struct {
    void (*emit_header)(FILE*);
    void (*emit_inc)(FILE*, size_t count);
    void (*emit_dec)(FILE*, size_t count);
    void (*emit_right)(FILE*, size_t count);
    void (*emit_left)(FILE*, size_t count);
    void (*emit_loop_start)(FILE*, size_t label);
    void (*emit_loop_end)(FILE*, size_t label);
    void (*emit_output)(FILE*);
    void (*emit_input)(FILE*);
    void (*emit_footer)(FILE*);
} compiler_backend;

#endif
