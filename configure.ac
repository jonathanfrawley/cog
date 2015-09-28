#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([cog], [0.1], [jmf@9lines.org])
AM_INIT_AUTOMAKE
AC_FILES(Makefile src/Makefile)
AC_CONFIG_SRCDIR([tests/test_map.c])
AC_CONFIG_HEADERS([config.h])
AC_OUTPUT

dist_doc_DATA = README.markdown

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_PROG_RANLIB

# Checks for header files.
AC_PATH_X
AC_CHECK_HEADERS([stdint.h stdlib.h string.h sys/time.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_INT32_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([floor gettimeofday memset sqrt])

AC_OUTPUT
