#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

#define STACK_SIZE 128
#define PI 3.14159265358979323846
#define E 2.7182818284
#define ADDRESS 0

typedef enum {
    ADD, SUB, MUL, DIV, POW, FACT
} Op;

typedef enum {
    SIN, COS, TAN, LN, ARCSIN, ARCCOS, ARCTAN
} Func;

typedef enum {
    NUM, OP, LBRAK, RBRAK, FUNC
} TokenType;

typedef union {
    double num;
    Op op;
    Func func;
    unsigned present : 1;
} TokenVal;

typedef struct {
    TokenType type;
    TokenVal val;
} Token;

// Function prototypes
int precedence(Op op);
void append(Token token_stream[STACK_SIZE], short* size, Token token);
Token pop(Token token_stream[STACK_SIZE], short* size);
double eval(char buf[STACK_SIZE], double ans);
double factorial(double n);
double ln(double x);
double arcsin(double x);
double arccos(double x);
double arctan(double x);

#endif
