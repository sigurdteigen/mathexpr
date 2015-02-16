AM_CFLAGS = -g3 -O0
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include

TESTS = expr-test

check_PROGRAMS = $(TESTS)

expr_test_LDADD = $(top_srcdir)/src/libmathexpr.la
expr_test_SOURCES = expr-test.c cmockery.c cmockery.h
