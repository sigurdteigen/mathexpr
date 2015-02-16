# libmathexpr
This **BSD licensed**, **C library** evaluates **basic single variable infix math expressions**. Some built-in constants and functions are supported. Building depends on lex, C99. **Version 0.1, use at own risk.**

### Example
    MathExpr e;
    char errbuf[256];
    const char *expr = "sin(PI)^2 + (5 * (t - 1))"; // variable t
    if (math_expr_init(&e, expr, sizeof(expr), errbuf, sizeof(errbuf)) < 0) {
        fprintf(stderr, "Compile error: %s", errbuf);
        exit(1);
    }
    double result;
    math_expr_eval(&e, 42, &result);