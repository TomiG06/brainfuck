#ifndef BRAINFUCK_H
#define BRAINFUCK_H

#include <stdio.h>
#include <stdint.h>

#define INC_P   '>'     /* Increment Pointer    */
#define DEC_P   '<'     /* Decrement Pointer    */
#define INC_C   '+'     /* Increment Cell       */
#define DEC_C   '-'     /* Decrement Cell       */
#define INPUT   ','
#define OUTPUT  '.'
#define OPEN    '['
#define CLOSE   ']'

void parse(FILE* f, uint8_t* inst);

void compile(uint8_t* inst);

void interpret(uint8_t* inst);

#endif
