AC_PREREQ(2.59)

define([revision], esyscmd([sh -c "(git rev-list -1 --abbrev-commit HEAD || echo unknown) | tr -d '\n'" 2>/dev/null]))dnl

AC_INIT(mathexpr, 0.1.revision)
AC_CANONICAL_TARGET
AC_CONFIG_SRCDIR([src/eval.c])

dnl update version here
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE([enable])

AC_DEFINE(BUILD_YEAR, esyscmd([date +%Y | tr -d '\n']), "Software build year")

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_MACRO_DIR([m4])

dnl ######################################################################
dnl Checks for programs.
dnl ######################################################################

AC_LANG([C])
AC_PROG_CC_C99()
AM_PROG_CC_C_O

AC_CONFIG_LIBOBJ_DIR(compat)

AM_PROG_LEX

LT_INIT([disable-static])

dnl ######################################################################
dnl Enable debugging
dnl ######################################################################

AC_ARG_ENABLE(debug,
              AS_HELP_STRING([--enable-debug], [Enable debugging]),
              [debug=$enableval],
              [debug=no])
AM_CONDITIONAL([NDEBUG], [test x"$debug" = x"no"])

if test x"$debug" = x"yes"; then
   CFLAGS="-g3 -O0 -std=gnu99"
else
   CFLAGS="-O2 -DNDEBUG -std=gnu99"
fi

dnl ######################################################################
dnl Checks for libraries.
dnl ######################################################################

dnl ######################################################################
dnl Special functions
dnl ######################################################################

dnl ######################################################################
dnl Checks for header files.
dnl ######################################################################

AC_HEADER_STDC
AC_HEADER_STDBOOL

dnl ######################################################################
dnl Checks for data types
dnl ######################################################################

AC_TYPE_SIZE_T
AC_TYPE_UINT64_T

dnl ######################################################################
dnl Collect all the options
dnl ######################################################################

CPPFLAGS="$CPPFLAGS"
CFLAGS="$CFLAGS -Wall"
LDFLAGS="$LDFLAGS"
LIBS="$LIBS"

dnl ######################################################################
dnl Summarize
dnl ######################################################################

AC_MSG_RESULT([])
AC_MSG_RESULT([Summary of options])
AC_MSG_RESULT([==================])

if test x"$debug" = x"yes"; then
   AC_MSG_RESULT([Build: DEBUG])
else
   AC_MSG_RESULT([Build: RELEASE])
fi

AC_MSG_RESULT([])

dnl ######################################################################
dnl Makefiles
dnl ######################################################################

AC_CONFIG_FILES([Makefile
	include/Makefile
	compat/Makefile
	src/Makefile
	test/Makefile])

AC_MSG_RESULT(DONE: Configuration done. Run make/gmake to build mathexpr.)

AC_OUTPUT([libmathexpr.pc])

echo "

$PACKAGE_NAME version $PACKAGE_VERSION

Prefix......: '$prefix'
Debug.......: $debug
Compiler....: '$CC $CFLAGS $CPPFLAGS'
Libraries...: '$LIBS'

";

