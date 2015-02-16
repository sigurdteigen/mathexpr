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

#ifndef MATH_EXPR_H
#define MATH_EXPR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#define MATH_EXPR_VAR_NAME "t"
#define MATH_EXPR_MAX_NODES 64

typedef enum {
    MATH_EXPR_NT_NUMBER = 258,
    MATH_EXPR_NT_VAR,

    // functions
    MATH_EXPR_NT_ABS,
    MATH_EXPR_NT_ACOS,
    MATH_EXPR_NT_ASIN,
    MATH_EXPR_NT_ATAN,
    MATH_EXPR_NT_ATAN2,
    MATH_EXPR_NT_CEIL,
    MATH_EXPR_NT_COS,
    MATH_EXPR_NT_EXP,
    MATH_EXPR_NT_FLOOR,
    MATH_EXPR_NT_LOG,
    MATH_EXPR_NT_MAX,
    MATH_EXPR_NT_MIN,
    MATH_EXPR_NT_POW,
    MATH_EXPR_NT_RANDOM,
    MATH_EXPR_NT_ROUND,
    MATH_EXPR_NT_SIN,
    MATH_EXPR_NT_SQRT,
    MATH_EXPR_NT_TAN
} MathExprNodeType;

typedef struct {
    int type;
    double value;
} MathExprNode;

typedef struct {
    MathExprNode nodes[MATH_EXPR_MAX_NODES];
    size_t nodes_len;
} MathExpr;

int math_expr_init(MathExpr *e, const char *expr, size_t expr_len, char *errbuf, size_t errbuf_size);

int math_expr_eval(MathExpr *e, double x, double *y_out);



#ifdef __cplusplus
}
#endif

#endif
