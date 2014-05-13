AC_INIT([ts],[0.1],[ilya.sumb@gmail.com])
AM_INIT_AUTOMAKE([foreign silent-rules])
AM_SILENT_RULES([yes])

AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CXX([mpic++])

CXXFLAGS="-O0 -g -pg -DNDEBUG -std=c++0x -Wall -Wextra -Werror"

AM_PROG_LIBTOOL
AC_ENABLE_SHARED
AC_DISABLE_STATIC
LT_INIT

###############################################################################
# Checking gcc version

test "x$GCC" = "xyes" || AC_MSG_ERROR(You need GCC)
GCC_VERSION="$(gcc -dumpversion)"

GCC_VERSION_MAJOR=$(echo $GCC_VERSION | cut -d'.' -f1)
GCC_VERSION_MINOR=$(echo $GCC_VERSION | cut -d'.' -f2)

(test "$GCC_VERSION_MAJOR" = 4 && test "$GCC_VERSION_MINOR" -ge 8) || AC_MSG_ERROR(You need GCC 4.8 or higher)
(test "$GCC_VERSION_MAJOR" = 4 && test "$GCC_VERSION_MINOR" -ge 9) && CXXFLAGS+=" -fdiagnostics-color=always"

###############################################################################
# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T

################################################################################
# pthread

PTHREAD_LIBS+=-lpthread

AX_PTHREAD([],[AC_MSG_ERROR(You need the pthread libraries.)])

LIBS="$PTHREAD_LIBS $LIBS"
CXXFLAGS="$CXXFLAGS $SPECFLAGS $PTHREAD_CFLAGS"

################################################################################
# MPI

AX_MPI([], [AC_MSG_ERROR(You need an MPI realization.)])

################################################################################
# Doxygen

#AC_CHECK_PROGS([DOXYGEN], [doxygen])
#if test -z "$DOXYGEN";
#   then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
#fi

#AM_CONDITIONAL([HAVE_DOXYGEN],
#[test -n "$DOXYGEN"])AM_COND_IF([HAVE_DOXYGEN],
#                                [AC_CONFIG_FILES([docs/Doxyfile])])

################################################################################
# Output

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
