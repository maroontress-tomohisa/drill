include $(srcdir)/Makefile.common
include $(top_srcdir)/Exports.am

AUTOMAKE_OPTIONS = foreign subdir-objects

SUFFIXES = .xml .h
LIBNAME = ncc
GCOV = gcov
LN = ln -sf
LIBTOOL = env CROSS_COMPILE_PREFIX=$(DESTDIR) $(SHELL) $(top_builddir)/libtool

EOL =

warning_options = \
-Wall -Wextra -Werror $(extra_warning_options)

extra_warning_options = \
-Waddress \
-Wbad-function-cast \
-Wcast-align \
-Wcast-qual \
-Wconversion \
-Wdisabled-optimization \
-Winline \
-Wmissing-declarations	\
-Wmissing-field-initializers \
-Wmissing-format-attribute \
-Wmissing-noreturn \
-Wmissing-prototypes \
-Wnested-externs \
-Wold-style-definition \
-Woverlength-strings \
-Wpacked \
-Wpointer-arith \
-Wpointer-sign \
-Wredundant-decls \
-Wsign-compare \
-Wstrict-prototypes \
-Wunreachable-code \
-Wwrite-strings \
$(EOL)
# -Waggregate-return
# -Wpadded
# -Wshadow

optimizing_options = \
-O3 -ggdb3

main_CPPFLAGS = \
-DCTORS="" \
-DUNUSED=__attribute__\(\(__unused__\)\) \
-DWEAK=__attribute__\(\(__weak__\)\) \
-I$(local_incdir)

main_CFLAGS = \
$(warning_options) $(optimizing_options) -pipe -pthread -std=c99 $(gcov_cflags)

main_LDFLAGS = \
$(gcov_ldflags)

main_LDADD =

HDRS_SRCS = $(COMMON_HEADER_SRCS)
HDRS = $(HDRS_SRCS:.xml=.h)

main_SOURCES = $(COMMON_C_SRCS)
bin_PROGRAMS = main

if HOST_OS_FREEBSD
endif

if HOST_OS_LINUX
endif

clean-local:: $(gcov_target_clean)

all-local: $(gcov_target_all)

if ENABLE_GCOV
gcov_target_all = coverage-all-local
gcov_target_clean = coverage-clean-local
gcov_cflags = -ggdb3 -O0 -fprofile-arcs -ftest-coverage
gcov_ldflags = -Wl,-whole-archive -Wl,-lgcov -Wl,-no-whole-archive

coverage-all-local: .coveragesymlink

coverage-clean-local: coverageclean
	rm -f .coveragesymlink
	@for c in $(COMMON_C_SRCS); do \
	  c=$${c%.c}; \
	  d=$${c%/*}; \
	  b=$${c##*/}; \
	  echo rm -f $$c.gcno $$c.gcda main-$$b.gcno; \
	       rm -f $$c.gcno $$c.gcda main-$$b.gcno; \
	done

.coveragesymlink: $(lib_LTLIBRARIES)
	@for c in $(COMMON_C_SRCS); do \
	  c=$${c%.c}; \
	  b=$${c##*/} ; \
	  ln -sf main-$$b.gcno $$c.gcno; \
	  ln -sf main-$$b.gcda $$c.gcda; \
	done
	touch $@

coverage: all
	@for c in $(COMMON_C_SRCS) ; do \
	  d=$${c%/*} ; \
	  echo $(GCOV) -p -o $$d $$c ; \
	       $(GCOV) -p -o $$d $$c ; \
	done

coverageclean:
	rm -f *.gcov
	@for c in $(COMMON_C_SRCS); do \
	  c=$${c%.c}; \
	  d=$${c%/*}; \
	  b=$${c##*/}; \
	  echo rm -f main-$$b.gcda; \
	       rm -f main-$$b.gcda; \
	done
endif
