%{
/*
    Copyright (c) 2015, Sigurd Teigen
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

     * Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of  nor the names of its contributors may be used to
       endorse or promote products derived from this software without specific
       prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/



#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <assert.h>

#include "expr.h"

#define YY_EXTRA_TYPE ReaderState*
#define YY_NO_INPUT

#define PUSH_OP(op) {\
    ReaderState *s = yyextra;\
    s->op_stack[s->op_stack_len] = op;\
    s->op_stack_len++;\
}

#define APPEND_NODE(t, v){\
    ReaderState *s = yyextra;\
    s->nodes[s->nodes_len].type = t;\
    s->nodes[s->nodes_len].value = v;\
    s->nodes_len++;\
}

#define STACK_TOP(s) s->op_stack[s->op_stack_len - 1]

#define IS_FUNC(t) (t >= MATH_EXPR_NT_ABS && t <= MATH_EXPR_NT_TAN)
#define IS_OP(t) (t == '+' || t == '-' || t == '*' || t == '/' || t == '^' || t == UNARY_NEG)

static int op_precedence(int o) {
    assert(IS_OP(o));

    switch (o) {
    case '+':
    case '-':
        return 2;
    case '*':
    case '/':
        return 3;
    case '^':
        return 4;
    case UNARY_NEG:
        return 5;

    default:
        assert(0 && "Never reach");
        return 0;
    }
}

static bool op_should_pop(int o1, int o2) {
    assert(IS_OP(o1));
    assert(IS_OP(o2));

    if (o1 == '^' || o1 == UNARY_NEG) {
        // right assoc
        return op_precedence(o1) < op_precedence(o2);
    } else {
        // left assoc
        return op_precedence(o1) <= op_precedence(o2);
    }
}

#define PROC_OP(o) {\
    ReaderState *s = yyextra;\
    while (s->op_stack_len > 0 && IS_OP(STACK_TOP(s))) {\
        if (op_should_pop(o, STACK_TOP(s))) {\
            APPEND_NODE(STACK_TOP(s), 0);\
            s->op_stack_len--;\
        } else {\
            break; \
        }\
    }\
    PUSH_OP(o);\
}\

%}

%option reentrant noyywrap nounput never-interactive warn
%option header-file="lexer.h"

WS [ \t\r\n]+
NUM ([0-9]+|[0-9]+\.[0-9]*|[0-9]*\.[0-9]+)([Ee][+\-]?[0-9]+)?

%%

{WS}

{NUM}       { APPEND_NODE(MATH_EXPR_NT_NUMBER, atof(yytext)); yyextra->enable_binary_neg = true; }

"t"         { APPEND_NODE(MATH_EXPR_NT_VAR, 0); yyextra->enable_binary_neg = true; }

"E"         { APPEND_NODE(MATH_EXPR_NT_NUMBER, 2.718281828); yyextra->enable_binary_neg = true; }
"LN2"       { APPEND_NODE(MATH_EXPR_NT_NUMBER, 0.693147181); yyextra->enable_binary_neg = true; }
"LN10"      { APPEND_NODE(MATH_EXPR_NT_NUMBER, 2.302585093); yyextra->enable_binary_neg = true; }
"LOG2E"     { APPEND_NODE(MATH_EXPR_NT_NUMBER, 1.442695040); yyextra->enable_binary_neg = true; }
"LOG10E"    { APPEND_NODE(MATH_EXPR_NT_NUMBER, 0.434294481); yyextra->enable_binary_neg = true; }
"PI"        { APPEND_NODE(MATH_EXPR_NT_NUMBER, 3.141592653); yyextra->enable_binary_neg = true; }
"SQRT1_2"   { APPEND_NODE(MATH_EXPR_NT_NUMBER, 0.707106781); yyextra->enable_binary_neg = true; }
"SQRT2"     { APPEND_NODE(MATH_EXPR_NT_NUMBER, 1.414213562); yyextra->enable_binary_neg = true; }


"abs"       { PUSH_OP(MATH_EXPR_NT_ABS); yyextra->enable_binary_neg = false; }
"acos"      { PUSH_OP(MATH_EXPR_NT_ACOS); yyextra->enable_binary_neg = false; }
"asin"      { PUSH_OP(MATH_EXPR_NT_ASIN); yyextra->enable_binary_neg = false; }
"atan"      { PUSH_OP(MATH_EXPR_NT_ATAN); yyextra->enable_binary_neg = false; }
"atan2"     { PUSH_OP(MATH_EXPR_NT_ATAN2); yyextra->enable_binary_neg = false; }
"ceil"      { PUSH_OP(MATH_EXPR_NT_CEIL); yyextra->enable_binary_neg = false; }
"cos"       { PUSH_OP(MATH_EXPR_NT_COS); yyextra->enable_binary_neg = false; }
"exp"       { PUSH_OP(MATH_EXPR_NT_EXP); yyextra->enable_binary_neg = false; }
"floor"     { PUSH_OP(MATH_EXPR_NT_FLOOR); yyextra->enable_binary_neg = false; }
"log"       { PUSH_OP(MATH_EXPR_NT_LOG); yyextra->enable_binary_neg = false; }
"max"       { PUSH_OP(MATH_EXPR_NT_MAX); yyextra->enable_binary_neg = false; }
"min"       { PUSH_OP(MATH_EXPR_NT_MIN); yyextra->enable_binary_neg = false; }
"pow"       { PUSH_OP(MATH_EXPR_NT_POW); yyextra->enable_binary_neg = false; }
"random"    { PUSH_OP(MATH_EXPR_NT_RANDOM); yyextra->enable_binary_neg = false; }
"round"     { PUSH_OP(MATH_EXPR_NT_ROUND); yyextra->enable_binary_neg = false; }
"sin"       { PUSH_OP(MATH_EXPR_NT_SIN); yyextra->enable_binary_neg = false; }
"sqrt"      { PUSH_OP(MATH_EXPR_NT_SQRT); yyextra->enable_binary_neg = false; }
"tan"       { PUSH_OP(MATH_EXPR_NT_TAN); yyextra->enable_binary_neg = false; }


"(" { PUSH_OP('('); yyextra->enable_binary_neg = false; }
")" {
    ReaderState *s = yyextra;
    while (s->op_stack_len > 0) {
        if (STACK_TOP(s) != '(') {
            APPEND_NODE(STACK_TOP(s), 0);
            s->op_stack_len--;
        } else {
            break;
        }
    }

    if (s->op_stack_len == 0) {
        snprintf(s->errbuf, s->errbuf_size, "Right parenthesis does not have a left parenthesis");
        return -1;
    }

    assert(STACK_TOP(s) == '(');
    s->op_stack_len--;

    if (s->op_stack_len > 0 && IS_FUNC(STACK_TOP(s))) {
        APPEND_NODE(STACK_TOP(s), 0);
        s->op_stack_len--;
    }

    yyextra->enable_binary_neg = true;
}

"," {
    ReaderState *s = yyextra;
    while (s->op_stack_len > 0) {
        if (STACK_TOP(s) != '(') {
            APPEND_NODE(STACK_TOP(s), 0);
            s->op_stack_len--;
        } else {
            break;
        }
    }

    if (s->op_stack_len == 0) {
        snprintf(s->errbuf, s->errbuf_size, "Comma does not follow an open parenthesis");
        return -1;
    }

    s->op_stack_len--;

    if ((s->op_stack_len > 0) && IS_FUNC(STACK_TOP(s))) {
        APPEND_NODE(STACK_TOP(s), 0);
        s->op_stack_len--;
    }

    yyextra->enable_binary_neg = false;
}


"+" { PROC_OP('+'); yyextra->enable_binary_neg = false; }
"-" {
    if (yyextra->enable_binary_neg) {
        PROC_OP('-');
    } else {
        PROC_OP(UNARY_NEG);
    }

    yyextra->enable_binary_neg = false;
}
"*" { PROC_OP('*'); yyextra->enable_binary_neg = false; }
"/" { PROC_OP('/'); yyextra->enable_binary_neg = false; }
"^" { PROC_OP('^'); yyextra->enable_binary_neg = false; }

. {
    ReaderState *s = yyextra;
    snprintf(s->errbuf, s->errbuf_size, "Invalid character '%c' found just before '%s'", *yytext, yytext+1);
    return -1;
}

%%

