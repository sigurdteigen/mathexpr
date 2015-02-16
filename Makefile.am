AUTOMAKE_OPTIONS = foreign

MAKEFLAGS = $(if $(filter-out 0,$(V)),,--no-print-directory --quiet)

SUBDIRS = compat include src test

DIST_SUBDIRS = compat include src

EXTRA_DIST = ChangeLog INSTALL README.md LICENSE

MOSTLYCLEANFILES = *~

CLEANFILES = 

DISTCLEANFILES = 

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.sub \
        configure install-sh missing mkinstalldirs depcomp ylwrap \
        ltmain.sh mdate-sh

ACLOCAL_AMFLAGS = -I m4
