# configure.ac for qilex

AC_PREREQ(2.59)
AC_INIT
AC_CONFIG_SRCDIR([qilex/simulator/rchain.cpp])
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE(qilex,0.5.2)
AC_CONFIG_SRCDIR([qilex/functions.cpp])
AM_CONFIG_HEADER(config.h)

dnl ---------------------------------------------------------------------------
dnl Checks for programs.
dnl ---------------------------------------------------------------------------

AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP

AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

CPPFLAGS="$CPPFLAGS  -Wall  -Wpointer-arith  -Wstrict-prototypes -W -O2 -g0 -DQT_NO_DEBUG -DQT_SHARED -DQT_THREAD_SUPPORT"

dnl ---------------------------------------------------------------------------
dnl Check host operating system.
dnl ---------------------------------------------------------------------------

dnl AC_CANONICAL_HOST

TestDataDirectory="\$(srcdir)"

case "$host_os" in
  *linux*)
    AM_CPPFLAGS="-DLINUX"
    ;;

  *solaris*)
    AM_CPPFLAGS="-DSOLARIS"
    ;;

  *cygwin*)
    AM_CPPFLAGS="-DWIN32 -DCYGWIN -DLIBSBML_EXPORTS"
    CC = "msdev.exe"
    CXX = "msdev.exe"
	;;

  *darwin*)
    AM_CPPFLAGS="-DMACOSX"
    TestDataDirectory=".."
    ;;
esac

dnl Make build/host/target consistent again
host=${target}
host_alias=${target_alias}
host_cpu=${target_cpu}
host_os=${target_os}
host_vendor=${target_vendor}



# Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_CHECK_HEADERS([inttypes.h malloc.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset pow strdup strrchr])

# Check for Flex and Bison++
AM_PROG_LEX
if test "$LEX" != flex; then
  LEX="$SHELL $missing_dir/missing flex"
  AC_SUBST(LEX_OUTPUT_ROOT, lex.yy)
  AC_SUBST(LEXLIB, '')
fi

MN_BISON_PLUS_PLUS_CHECK


# Checks for libraries.
#Check for Qt
gw_CHECK_QT

AC_SUBST([qilex_EXTRA_CPPFLAGS], [$QT_CXXFLAGS])
AC_SUBST([qilex_EXTRA_CXXFLAGS], [$QT_CXXFLAGS])
AC_SUBST([qilex_EXTRA_LDFLAGS], [$QT_LDFLAGS])

echo "QT_CXXFLAGS=" $QT_CXXFLAGS;
echo "QT_LDADD=" $QT_LDADD;

#Check Coin
SIM_AC_HAVE_COIN_IFELSE([
    CPPFLAGS="$CPPFLAGS $sim_ac_coin_cppflags"
    LDFLAGS="$LDFLAGS $sim_ac_coin_ldflags"
    LIBS="$sim_ac_coin_libs $LIBS"
    qilex_EXTRA_CPPFLAGS="$qilex_EXTRA_CPPFLAGS $sim_ac_coin_cppflags"
    qilex_EXTRA_LDFLAGS="$qilex_EXTRA_LDFLAGS $sim_ac_coin_ldflags"
    qilex_EXTRA_LIBS="$sim_ac_coin_libs $qilex_EXTRA_LIBS"
  ], [AC_MSG_ERROR([couldn't compile and link against Coin])])

#Check SoQt
 SIM_AC_HAVE_SOQT_IFELSE([
    CPPFLAGS="$CPPFLAGS $sim_ac_soqt_cppflags"
    LDFLAGS="$LDFLAGS $sim_ac_soqt_ldflags"
    LIBS="$sim_ac_soqt_libs $LIBS"
    qilex_EXTRA_CPPFLAGS="$qilex_EXTRA_CPPFLAGS $sim_ac_soqt_cppflags"
    qilex_EXTRA_LDFLAGS="$qilex_EXTRA_LDFLAGS $sim_ac_soqt_ldflags"
    qilex_EXTRA_LIBS="$sim_ac_soqt_libs $qilex_EXTRA_LIBS"],
	[AC_MSG_ERROR([couldn't compile and link against SoQt])])
# These are used for constructing the soqt-config file.

AC_SUBST([qilex_EXTRA_CPPFLAGS], [$CPPFLAGS])
AC_SUBST([qilex_EXTRA_CFLAGS], [$CFLAGS])
AC_SUBST([qilex_EXTRA_CXXFLAGS], [$CXXFLAGS])
AC_SUBST([qilex_EXTRA_LDFLAGS], [$LDFLAGS])
AC_SUBST([qilex_EXTRA_LIBS], [$LIBS])

CFLAGS="$CFLAGS -Wall"

AC_CONFIG_FILES([Makefile
                 qilex/Makefile \
                 qilex/contrib/Makefile \
                 qilex/images/Makefile \
                 qilex/interpret/Makefile \
                 qilex/objects/Makefile \
                 qilex/robots/Makefile \
                 qilex/simulator/Makefile \
                 qilex/soqthybrid/Makefile\
					  qilex/grasp_objects/Makefile\
                 qilex/widgets/Makefile])
AC_OUTPUT

echo "Build Settings for Qilex :"
echo "----------------------------------"
echo "  host os     = $host_os"
echo "  install dir = $prefix"
echo "  CC          = $CC"
echo "  CXX         = $CXX"
echo "  CFLAGS      = $CFLAGS"
echo "  CXXFLAGS    = $CXXFLAGS"
echo "  CPPFLAGS    = $CPPFLAGS $AM_CPPFLAGS"
echo "  LDFLAGS     = $LDFLAGS"
echo "  LIBS        = $LIBS"
echo "qilex_EXTRA_CPPFLAGS="  $qilex_EXTRA_CPPFLAGS;
echo "qilex_EXTRA_CXXFLAGS="  $qilex_EXTRA_CXXFLAGS;
echo "qilex_EXTRA_LDFLAGS="   $qilex_EXTRA_LDFLAGS;
echo "qilex_EXTRA_EXTRA_LIBS=" $qilex_EXTRA_LIBS;
echo ""
