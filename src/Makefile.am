AM_CFLAGS = @CFLAGS@ -I$(top_srcdir) -I$(top_srcdir)/include
AM_LDFLAGS = @LDFLAGS@

.PRECIOUS: ../pub/libcompat.la

../compat/libcfcompat.la:
	$(MAKE) -C ../compat $(AM_MAKEFLAGS) libcompat.la

lib_LTLIBRARIES = libmathexpr.la
libmathexpr_la_LIBADD = ../compat/libcompat.la
libmathexpr_la_CFLAGS = $(AM_CFLAGS)
libmathexpr_la_LDFLAGS = $(AM_LDFLAGS) -lm

libmathexpr_la_SOURCES = lexer.l eval.c

BUILT_SOURCES = lexer.c lexer.h

CLEANFILES =

MOSTLYCLEANFILES = *~

DISTCLEANFILES = 

MAINTAINERCLEANFILES = Makefile.in conf.h.in stamp-h.in
