#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include <math.h>

#include <mathexpr.h>

#include "cmockery.h"

static char errbuf[1024];

#define assert_double_close(e, a) _assert_double_close(e, a, __FILE__, __LINE__)
void _assert_double_close(double expected, double actual, const char *const file, const int line)
{
    if (fabs(expected - actual) > 1e-8) {
        print_error("%f != %.8f (+- 1e-8)\n", expected, actual);
        _fail(file, line);
    }
}

#define assert_expr_ok(e, x, y) _assert_expr_ok(e, x, y, __FILE__, __LINE__)
static void _assert_expr_ok(const char *expr, double x, double expected_y, const char *const file, const int line) {
    MathExpr e;
    size_t expr_len = strlen(expr);
    if (math_expr_init(&e, expr, expr_len, errbuf, sizeof(errbuf)) != 0) {
        print_error("%s\n", errbuf);
        _fail(file, line);
    }

    double y;
    if (math_expr_eval(&e, x, &y) != 0) {
        print_error("Eval error\n");
        _fail(file, line);
    }

    _assert_double_close(expected_y, y, file, line);
}

#define assert_expr_fail(e) _assert_expr_fail(e, __FILE__, __LINE__)
static void _assert_expr_fail(const char *expr, const char *const file, const int line) {
    MathExpr e;
    size_t expr_len = strlen(expr);
    if (math_expr_init(&e, expr, expr_len, errbuf, sizeof(errbuf)) == 0) {
        print_error("Expected error\n");
        _fail(file, line);
    }
}

static void test_simple(void **state) {
    assert_expr_ok("42", 0, 42);
    assert_expr_ok("PI", 0, 3.141592653);

    assert_expr_ok("t", 42, 42);
    assert_expr_ok("t * 1", 42, 42);
    assert_expr_ok("2 * t", 42, 84);
    assert_expr_ok("t + 1", 42, 43);
    assert_expr_ok("t - 1", 42, 41);
    assert_expr_ok("t / 1", 42, 42);
    assert_expr_ok("t / 2", 42, 21);
    assert_expr_ok("2 ^ 3", 0, 8);
    assert_expr_ok("2 ^ t", 2, 4);
    assert_expr_ok("0^2", 0, 0);
    assert_expr_ok("1/0", 0, INFINITY);
}

static void test_unary(void **state) {
    assert_expr_ok("-42", 0, -42);

}


static void test_assoc(void **state) {
    assert_expr_ok("1 + 2 + t", 3, 6);
    assert_expr_ok("1 + 2 * t", 3, 7);
    assert_expr_ok("1 + 2 ^ t", 3, 9);
    assert_expr_ok("1 * 1 * t * 1 * 1", 2, 2);
}

static void test_parens(void **state) {
    assert_expr_ok("(1 + 2) * t", 3, 9);
    assert_expr_ok("(((1)))", 0, 1);
    assert_expr_ok("(1 + (2 - 3))", 0, 0);

    assert_expr_fail("()");
    assert_expr_fail("1 + (3 - 4");
}

static void test_funcs(void **state) {
    assert_expr_ok("sin(PI)", 0, 0);
    assert_expr_ok("cos(0)", 0, 1);
    assert_expr_ok("sin(PI)^2", 0, 0);
    assert_expr_ok("sin(PI)^2 + (5 * (t - 1))", 42, 205);

    srandom(0);
    assert_expr_ok("random()", 0, 0.84018772);
    assert_expr_ok("1 + random() + 5", 0, 6.39438293);
}


static void test_evil(void **state) {
    assert_expr_ok("2 / - 7 + ( - 9 * 8 ) * 2 ^ - 9 - 5", 0, -5.426339286);
    assert_expr_ok("2/-7+(-9*8)*2^-t-5", 9, -5.426339286);

    assert_expr_fail("wtf");
}


int main(int argc, const char **argv) {
    const UnitTest tests[] = {
        unit_test(test_simple),
        unit_test(test_unary),
        unit_test(test_assoc),
        unit_test(test_parens),
        unit_test(test_funcs),
        unit_test(test_evil),
     };
     return run_tests(tests);

}
