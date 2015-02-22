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

#include "expr.h"
#include "lexer.h"

#include <math.h>
#include <assert.h>

static int arity(int op) {
    switch (op) {
    case MATH_EXPR_NT_NUMBER:
    case MATH_EXPR_NT_VAR:
    case MATH_EXPR_NT_RANDOM:
        return 0;

    case UNARY_NEG:
    case MATH_EXPR_NT_ABS:
    case MATH_EXPR_NT_ACOS:
    case MATH_EXPR_NT_ASIN:
    case MATH_EXPR_NT_ATAN:
    case MATH_EXPR_NT_CEIL:
    case MATH_EXPR_NT_COS:
    case MATH_EXPR_NT_EXP:
    case MATH_EXPR_NT_FLOOR:
    case MATH_EXPR_NT_LOG:
    case MATH_EXPR_NT_ROUND:
    case MATH_EXPR_NT_SIN:
    case MATH_EXPR_NT_SQRT:
    case MATH_EXPR_NT_TAN:
        return 1;

    case '+':
    case '-':
    case '*':
    case '/':
    case '^':
    case MATH_EXPR_NT_ATAN2:
    case MATH_EXPR_NT_MAX:
    case MATH_EXPR_NT_MIN:
    case MATH_EXPR_NT_POW:
        return 2;

    default:
        assert(false && "Unhandled operator");
        return 0;
    }
}

static int arity_check(const MathExprNode *nodes, size_t nodes_len, char *errbuf, size_t errbuf_size) {
    int stack_len = 0;
    for (size_t i = 0; i < nodes_len; i++) {
        stack_len += 1 - arity(nodes[i].type);
        if (stack_len < 0) {
            snprintf(errbuf, errbuf_size, "Operator '%c' requires %d args", nodes[i].type, arity(nodes[i].type));
            return -1;
        }
    }

    assert(stack_len == 1);
    return 0;
}

static double eval_op0(int op) {
    assert(arity(op) == 0);

    switch (op) {
    case MATH_EXPR_NT_RANDOM: return (double)rand()/(double)RAND_MAX;

    default:
        assert(false && "Never reach");
        return 0;
    }
}

static double eval_op1(int op, double arg1) {
    assert(arity(op) == 1);

    switch (op) {
    case UNARY_NEG:         return -arg1;
    case MATH_EXPR_NT_ABS:     return fabs(arg1);
    case MATH_EXPR_NT_ACOS:    return acos(arg1);
    case MATH_EXPR_NT_ASIN:    return asin(arg1);
    case MATH_EXPR_NT_ATAN:    return atan(arg1);
    case MATH_EXPR_NT_CEIL:    return ceil(arg1);
    case MATH_EXPR_NT_COS:     return cos(arg1);
    case MATH_EXPR_NT_EXP:     return exp(arg1);
    case MATH_EXPR_NT_FLOOR:   return floor(arg1);
    case MATH_EXPR_NT_LOG:     return log(arg1);
    case MATH_EXPR_NT_ROUND:   return round(arg1);
    case MATH_EXPR_NT_SIN:     return sin(arg1);
    case MATH_EXPR_NT_SQRT:    return sqrt(arg1);
    case MATH_EXPR_NT_TAN:     return tan(arg1);

    default:
        assert(false && "Never reach");
        return 0;
    }
}

static double eval_op2(int op, double arg1, double arg2) {
    assert(arity(op) == 2);

    switch (op) {
    case '+':             return arg1 + arg2;
    case '-':             return arg1 - arg2;
    case '*':             return arg1 * arg2;
    case '/':             return arg1 / arg2;
    case '^':             return pow(arg1, arg2);
    case MATH_EXPR_NT_ATAN2: return atan2(arg1, arg2);
    case MATH_EXPR_NT_MAX:   return fmax(arg1, arg2);
    case MATH_EXPR_NT_MIN:   return fmin(arg1, arg2);
    case MATH_EXPR_NT_POW:   return pow(arg1, arg2);

    default:
        assert(false && "Never reach");
        return 0;
    }
}

static int constant_fold(MathExprNode *nodes, size_t nodes_len, MathExprNode *nodes_out, size_t *nodes_out_len) {
    MathExprNode stack[nodes_len];
    size_t stack_len = 0;

    if (nodes_len == 1) {
        nodes_out[0] = nodes[0];
        *nodes_out_len = 1;
        return 0;
    }

    *nodes_out_len = 0;

    for (size_t i = 0; i < nodes_len; i++) {
        if ((nodes[i].type == MATH_EXPR_NT_NUMBER) || (nodes[i].type == MATH_EXPR_NT_VAR)) {
            stack[stack_len] = nodes[i];
            stack_len++;
            continue;
        }

        switch (arity(nodes[i].type)) {
        case 0:
            for (size_t j = 0; j < stack_len; j++) {
                nodes_out[*nodes_out_len] = stack[j]; (*nodes_out_len)++;
            }
            nodes_out[*nodes_out_len] = nodes[i]; (*nodes_out_len)++;
            stack_len = 0;
            break;

        case 1:
            if (stack_len > 0 && stack[stack_len - 1].type == MATH_EXPR_NT_NUMBER) {
                stack[stack_len - 1].value = eval_op1(nodes[i].type, stack[stack_len - 1].value);
            } else {
                for (size_t j = 0; j < stack_len; j++) {
                    nodes_out[*nodes_out_len] = stack[j]; (*nodes_out_len)++;
                }
                nodes_out[*nodes_out_len] = nodes[i]; (*nodes_out_len)++;
                stack_len = 0;
            }
            break;

        case 2:
            if (stack_len > 1 && stack[stack_len - 1].type == MATH_EXPR_NT_NUMBER && stack[stack_len - 2].type == MATH_EXPR_NT_NUMBER) {
                stack[stack_len - 2].value = eval_op2(nodes[i].type, stack[stack_len - 2].value, stack[stack_len - 1].value);
                stack_len--;
            } else {
                for (size_t j = 0; j < stack_len; j++) {
                    nodes_out[*nodes_out_len] = stack[j]; (*nodes_out_len)++;
                }
                nodes_out[*nodes_out_len] = nodes[i]; (*nodes_out_len)++;
                stack_len = 0;
            }
            break;

        default:
            assert(false && "Never reach");
        }
    }

    assert(stack_len < 2);
    if (stack_len == 1) {
        nodes_out[*nodes_out_len] = stack[0];
        (*nodes_out_len)++;
    }

    return 0;
}

int math_expr_init(MathExpr *e, const char *expr, size_t expr_len, char *errbuf, size_t errbuf_size) {
    if (expr_len == 0) {
        snprintf(errbuf, errbuf_size, "Expression is empty");
        return -1;
    }

    ReaderState s = {
        .nodes_len = 0,
        .op_stack_len = 0,
        .errbuf = errbuf,
        .errbuf_size = errbuf_size,
        .enable_binary_neg = false,
    };

    yyscan_t scanner = NULL;
    yylex_init_extra(&s, &scanner);

    yy_scan_bytes(expr, expr_len, scanner); {
        int c;
        while ((c = yylex(scanner)) > 0);
        if (c < 0) {
            yylex_destroy(scanner);
            return c;
        }
    }

    yylex_destroy(scanner);

    while (s.op_stack_len > 0) {
        const int op = s.op_stack[s.op_stack_len - 1];
        if (op == '(' || op == ')') {
            snprintf(errbuf, errbuf_size, "Mismatched parentheses");
            return -1;
        }

        s.nodes[s.nodes_len].type = op;
        s.nodes_len++;
        s.op_stack_len--;
    }

    if (s.nodes_len == 0) {
        snprintf(errbuf, errbuf_size, "Expression reduced to nothing");
        return -1;
    }

    if (arity_check(s.nodes, s.nodes_len, errbuf, errbuf_size) < 0) {
        return -1;
    }
    if (constant_fold(s.nodes, s.nodes_len, e->nodes, &e->nodes_len) < 0) {
        return -1;
    }


    return 0;
}

int math_expr_eval(MathExpr *e, double x, double *y_out) {
    double stack[e->nodes_len];
    size_t stack_len = 0;

    for (size_t i = 0; i < e->nodes_len; i++) {
        if (e->nodes[i].type == MATH_EXPR_NT_NUMBER) {
            stack[stack_len] = e->nodes[i].value;
            stack_len++;
            continue;
        } else if (e->nodes[i].type == MATH_EXPR_NT_VAR) {
            stack[stack_len] = x;
            stack_len++;
            continue;
        }

        switch (arity(e->nodes[i].type)) {
        case 0:
            stack[stack_len] = eval_op0(e->nodes[i].type);
            stack_len++;
            break;

        case 1:
            stack[stack_len - 1] = eval_op1(e->nodes[i].type, stack[stack_len - 1]);
            break;

        case 2:
            stack[stack_len - 2] = eval_op2(e->nodes[i].type, stack[stack_len - 2], stack[stack_len - 1]);
            stack_len--;
            break;

        default:
            assert(false && "Never reach");
        }

    }

    assert(stack_len == 1);
    *y_out = stack[0];
    return 0;
}
