AC_INIT([ts], [0.1], [ilya.sumb@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_MACRO_DIR([m4/])
AC_PREFIX_DEFAULT([./build])

AC_PROG_CXX([mpic++])

SPECFLAGS="-std=c++11"

#########

# pthread

PTHREAD_LIBS+=-lpthread

AX_PTHREAD([],[AC_MSG_ERROR(You need the pthread libraries.)])

LIBS="$PTHREAD_LIBS $LIBS"
CXXFLAGS="$CXXFLAGS $SPECFLAGS $PTHREAD_CFLAGS"

##########

# MPI

AX_MPI([], [AC_MSG_ERROR(You need an MPI realization.)])


##########

# Output

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
