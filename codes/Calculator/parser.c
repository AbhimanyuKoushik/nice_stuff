#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <avr/eeprom.h>

// Factorial function
double factorial(double n) {
    if (n < 0) return 0;  
    if (n == 0 || n == 1) return 1;
    double result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

// Natural logarithm
double ln(double x) {
    return log(x);
}

// Inverse trigonometric functions
double arcsin(double x) {
    return asin(x);
}

double arccos(double x) {
    return acos(x);
}

double arctan(double x) {
    return atan(x);
}

// Get operator precedence
int precedence(Op op) {
    if (op == ADD || op == SUB) return 1;
    if (op == MUL || op == DIV) return 2;
    if (op == POW) return 3;
    return -1;
}

// Append token to the stream
void append(Token token_stream[STACK_SIZE], short* size, Token token) {
    token_stream[*size] = token;
    (*size)++;
}

// Pop token from the stream
Token pop(Token token_stream[STACK_SIZE], short* size) {
    Token popped_token = token_stream[*size - 1];
    (*size)--;
    return popped_token;
}

// Main expression evaluation function
double eval(char buf[STACK_SIZE], double ans) {
    Token token_stream[STACK_SIZE];
    short size = 0;
    short i = 0;

    // Tokenization
    while (i < STACK_SIZE && buf[i] != '\0') {
        Token token;
        char ch = buf[i];
        int skip = 0;

        if (ch == '(') {
            token.type = LBRAK;
            token.val.present = 0;
            skip = 1;
        }
        else if (ch == ')') {
            token.type = RBRAK;
            token.val.present = 0;
            skip = 1;
        }
        else if (ch == '+') {
            token.type = OP;
            token.val.op = ADD;
            skip = 1;
        }
        else if (ch == '-') {
            token.type = OP;
            token.val.op = SUB;
            skip = 1;
        }
        else if (ch == '*') {
            token.type = OP;
            token.val.op = MUL;
            skip = 1;
        }
        else if (ch == '/') {
            token.type = OP;
            token.val.op = DIV;
            skip = 1;
        }
        else if (ch == '^') {
            token.type = OP;
            token.val.op = POW;
            skip = 1;
        }
        else if (ch == '!') {
            token.type = OP;
            token.val.op = FACT;
            skip = 1;
        }
        else if (ch == 'p') {
            token.type = NUM;
            token.val.num = PI;
            skip = 1;
        }
        else if (ch == 'e') {
            token.type = NUM;
            token.val.num = E;
            skip = 1;
        }
        else if (ch == 's') {
            token.type = FUNC;
            token.val.func = SIN;
            skip = 1;
        }
        else if (ch == 'c') {
            token.type = FUNC;
            token.val.func = COS;
            skip = 1;
        }
        else if (ch == 't') {
            token.type = FUNC;
            token.val.func = TAN;
            skip = 1;
        }
        else if (ch == 'l') {
            token.type = FUNC;
            token.val.func = LN;
            skip = 1;
        }
        else if (ch == '@') {
            token.type = FUNC;
            token.val.func = ARCSIN;
            skip = 1;
        }
        else if (ch == '#') {
            token.type = FUNC;
            token.val.func = ARCCOS;
            skip = 1;
        }
        else if (ch == '$') {
            token.type = FUNC;
            token.val.func = ARCTAN;
            skip = 1;
        }
        else if (ch == 'M') {
            double mem = 0;
            eeprom_read_block((void*)&mem, (const void*)ADDRESS, sizeof(double));
            token.type = NUM;
            token.val.num = mem;
            skip = 1;
        }
        else {
            double val;
            sscanf(buf + i, "%lf %n", &val, &skip);
            token.type = NUM;
            token.val.num = val;
        }

        append(token_stream, &size, token);
        i += skip;
    }

    // Conversion to RPN using Shunting-yard algorithm
    Token output_stack[STACK_SIZE];
    short output_size = 0;
    Token operator_stack[STACK_SIZE];
    short operator_size = 0;

    for (int i = 0; i < size; i++) {
        Token token = token_stream[i];

        if (token.type == NUM) {
            append(output_stack, &output_size, token);
        }
        else if (token.type == OP) {
            while (operator_size > 0 && precedence(token.val.op) <= precedence(operator_stack[operator_size - 1].val.op)) {
                append(output_stack, &output_size, pop(operator_stack, &operator_size));
            }
            append(operator_stack, &operator_size, token);
        }
        else if (token.type == LBRAK) {
            append(operator_stack, &operator_size, token);
        }
        else if (token.type == RBRAK) {
            while (operator_size > 0 && operator_stack[operator_size - 1].type != LBRAK) {
                append(output_stack, &output_size, pop(operator_stack, &operator_size));
            }
            pop(operator_stack, &operator_size);  // Remove '('
        }
        else if (token.type == FUNC) {
            append(operator_stack, &operator_size, token);
        }
    }

    while (operator_size > 0) {
        append(output_stack, &output_size, pop(operator_stack, &operator_size));
    }

    // RPN Evaluation
    Token res_stack[STACK_SIZE];
    short res_size = 0;

    for (int i = 0; i < output_size; i++) {
        Token token = output_stack[i];

        if (token.type == NUM) {
            append(res_stack, &res_size, token);
        }
        else if (token.type == OP) {
            Token right = pop(res_stack, &res_size);
            Token left = pop(res_stack, &res_size);

            double res = 0;
            if (token.val.op == ADD) res = left.val.num + right.val.num;
            else if (token.val.op == SUB) res = left.val.num - right.val.num;
            else if (token.val.op == MUL) res = left.val.num * right.val.num;
            else if (token.val.op == DIV) res = left.val.num / right.val.num;
            else if (token.val.op == POW) res = pow(left.val.num, right.val.num);

            Token res_token = {NUM, .val.num = res};
            append(res_stack, &res_size, res_token);
        }
    }

    return res_stack[0].val.num;
}
