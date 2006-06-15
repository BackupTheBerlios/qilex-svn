AC_DEFUN([MN_BISON_PLUS_PLUS_CHECK],
[ bison_plusplus_path=""
        AC_ARG_WITH(bisonpp,
        [  --with-bisonpp=<path>   specify the directory of bison++],
        [ bison_plusplus_path="$with_bisonpp"
        ])

        mn_save_path=$PATH
        PATH="$PATH:$bison_plusplus_path"
        AC_PATH_PROG(BISONXX,bison++,no)
   PATH="$mn_save_path"
])

AC_DEFUN([MN_BISON_PLUS_PLUS_EVAL],
[ if test "x$BISONXX" = "xno"; then
                AC_MSG_ERROR([Can't find bison++. Bison++ is required to
build qilex.])
                mn_all_tests=bad
        fi
])
# Check for Qt compiler flags, linker flags, and binary packages
AC_DEFUN([gw_CHECK_QT],
[
AC_REQUIRE([AC_PROG_CXX])
AC_REQUIRE([AC_PATH_X])

AC_MSG_CHECKING([QTDIR])
AC_ARG_WITH([qtdir], [  --with-qtdir=DIR        Qt installation directory [default=$QTDIR]], QTDIR=$withval)
# Check that QTDIR is defined or that --with-qtdir given
if test x"$QTDIR" = x ; then
    QT_SEARCH="/usr/lib/qt31 /usr/local/qt31 /usr/lib/qt3 /usr/local/qt3 /usr/lib/qt2 /usr/local/qt2 /usr/lib/qt /usr/local/qt"
    for i in $QT_SEARCH; do
        if test -f $i/include/qglobal.h -a x$QTDIR = x; then QTDIR=$i; fi
    done
fi
if test x"$QTDIR" = x ; then
    AC_MSG_ERROR([*** QTDIR must be defined, or --with-qtdir option given])
fi
AC_MSG_RESULT([$QTDIR])

# Change backslashes in QTDIR to forward slashes to prevent escaping
# problems later on in the build process, mainly for Cygwin build
# environment using MSVC as the compiler
# TODO: Use sed instead of perl
QTDIR=`echo $QTDIR | perl -p -e 's/\\\\/\\//g'`

# Figure out which version of Qt we are using
AC_MSG_CHECKING([Qt version])
QT_VER=`grep 'define.*QT_VERSION_STR' $QTDIR/include/qglobal.h | perl -p -e 's/\D//g'`
case "${QT_VER}" in
    2*)
        QT_MAJOR="2"
    ;;
    3*)
        QT_MAJOR="3"
    ;;
    *)
        #AC_MSG_ERROR([*** Don't know how to handle this Qt major version])
    ;;
esac
AC_MSG_RESULT([$QT_VER ($QT_MAJOR)])

# Check that moc is in path
AC_PATH_PROG(MOC, moc-qt3, , $PATH:$QTDIR/bin)
if test x$MOC = x ; then
        AC_MSG_ERROR([*** moc must be in path])
fi

# uic is the Qt user interface compiler
AC_PATH_PROG(UIC, uic, , $PATH:$QTDIR/bin)
if test x$UIC = x ; then
        AC_MSG_ERROR([*** uic must be in path])
fi

# qembed is the Qt data embedding utility.
# It is located in $QTDIR/tools/qembed, and must be compiled and installed
# manually, we'll let it slide if it isn't present
AC_CHECK_PROG(QEMBED, qembed, qembed)


# Calculate Qt include path
QT_CXXFLAGS="-I$QTDIR/include"

QT_IS_EMBEDDED="no"
# On unix, figure out if we're doing a static or dynamic link
case "${host}" in
    *-cygwin)
	AC_DEFINE_UNQUOTED(WIN32, "", Defined if on Win32 platform)
        if test -f "$QTDIR/lib/qt.lib" ; then
            QT_LIB="qt.lib"
            QT_IS_STATIC="yes"
            QT_IS_MT="no"
        elif test -f "$QTDIR/lib/qt-mt.lib" ; then
            QT_LIB="qt-mt.lib" 
            QT_IS_STATIC="yes"
            QT_IS_MT="yes"
        elif test -f "$QTDIR/lib/qt$QT_VER.lib" ; then
            QT_LIB="qt$QT_VER.lib"
            QT_IS_STATIC="no"
            QT_IS_MT="no"
        elif test -f "$QTDIR/lib/qt-mt$QT_VER.lib" ; then
            QT_LIB="qt-mt$QT_VER.lib"
            QT_IS_STATIC="no"
            QT_IS_MT="yes"
        fi
        ;;

    *)
        QT_IS_STATIC=`ls $QTDIR/lib/*.a 2> /dev/null`
        if test "x$QT_IS_STATIC" = x; then
            QT_IS_STATIC="no"
        else
            QT_IS_STATIC="yes"
        fi
        if test x$QT_IS_STATIC = xno ; then
            QT_IS_DYNAMIC=`ls $QTDIR/lib/*.so 2> /dev/null` 
            if test "x$QT_IS_DYNAMIC" = x;  then
                AC_MSG_ERROR([*** Couldn't find any Qt libraries])
            fi
        fi

#        if test "x`ls $QTDIR/lib/libqt.* 2> /dev/null`" != x ; then
#            QT_LIB="-lqt"
#            QT_IS_MT="no"
#        elif test "x`ls $QTDIR/lib/libqt-mt.* 2> /dev/null`" != x ; then
#            QT_LIB="-lqt-mt"
#            QT_IS_MT="yes"
        if test "x`ls $QTDIR/lib/libqt-mt.* 2> /dev/null`" != x ; then
            QT_LIB="-lqt-mt"
            QT_IS_MT="yes"
        elif test "x`ls $QTDIR/lib/libqt.* 2> /dev/null`" != x ; then
            QT_LIB="-lqt"
            QT_IS_MT="no"
        elif test "x`ls $QTDIR/lib/libqte.* 2> /dev/null`" != x ; then
            QT_LIB="-lqte"
            QT_IS_MT="no"
            QT_IS_EMBEDDED="yes"
        elif test "x`ls $QTDIR/lib/libqte-mt.* 2> /dev/null`" != x ; then
            QT_LIB="-lqte-mt"
            QT_IS_MT="yes"
            QT_IS_EMBEDDED="yes"
        fi
        ;;
esac
AC_MSG_CHECKING([if Qt is static])
AC_MSG_RESULT([$QT_IS_STATIC])
AC_MSG_CHECKING([if Qt is multithreaded])
AC_MSG_RESULT([$QT_IS_MT])
AC_MSG_CHECKING([if Qt is embedded])
AC_MSG_RESULT([$QT_IS_EMBEDDED])

QT_GUILINK=""
QASSISTANTCLIENT_LDADD="-lqassistantclient"
case "${host}" in
    *irix*)
        QT_LIBS="$QT_LIB"
        if test $QT_IS_STATIC = yes ; then
            QT_LIBS="$QT_LIBS -L$x_libraries -lXext -lX11 -lm -lSM -lICE"
        fi
        ;;

    *linux*)
        QT_LIBS="$QT_LIB"
        if test $QT_IS_STATIC = yes && test $QT_IS_EMBEDDED = no; then
            QT_LIBS="$QT_LIBS -L$x_libraries -lXext -lX11 -lm -lSM -lICE -ldl -ljpeg"
        fi
        ;;


    *osf*) 
        # Digital Unix (aka DGUX aka Tru64)
        QT_LIBS="$QT_LIB"
        if test $QT_IS_STATIC = yes ; then
            QT_LIBS="$QT_LIBS -L$x_libraries -lXext -lX11 -lm -lSM -lICE"
        fi
        ;;

    *solaris*)
        QT_LIBS="$QT_LIB"
        if test $QT_IS_STATIC = yes ; then
            QT_LIBS="$QT_LIBS -L$x_libraries -lXext -lX11 -lm -lSM -lICE -lresolv -lsocket -lnsl"
        fi
        ;;


    *win*)
        # linker flag to suppress console when linking a GUI app on Win32
        QT_GUILINK="/subsystem:windows"

	if test $QT_MAJOR = "3" ; then
	    if test $QT_IS_MT = yes ; then
        	QT_LIBS="/nodefaultlib:libcmt"
            else
            	QT_LIBS="/nodefaultlib:libc"
            fi
        fi

        if test $QT_IS_STATIC = yes ; then
            QT_LIBS="$QT_LIBS $QT_LIB kernel32.lib user32.lib gdi32.lib comdlg32.lib ole32.lib shell32.lib imm32.lib advapi32.lib wsock32.lib winspool.lib winmm.lib netapi32.lib"
            if test $QT_MAJOR = "3" ; then
                QT_LIBS="$QT_LIBS qtmain.lib"
            fi
        else
            QT_LIBS="$QT_LIBS $QT_LIB"        
            if test $QT_MAJOR = "3" ; then
                QT_CXXFLAGS="$QT_CXXFLAGS -DQT_DLL"
                QT_LIBS="$QT_LIBS qtmain.lib qui.lib user32.lib netapi32.lib"
            fi
        fi
        QASSISTANTCLIENT_LDADD="qassistantclient.lib"
        ;;

esac


if test x"$QT_IS_EMBEDDED" = "xyes" ; then
        QT_CXXFLAGS="-DQWS $QT_CXXFLAGS"
fi

if test x"$QT_IS_MT" = "xyes" ; then
        QT_CXXFLAGS="$QT_CXXFLAGS -D_REENTRANT -DQT_THREAD_SUPPORT"
fi

QT_LDADD="-L$QTDIR/lib $QT_LIBS"

if test x$QT_IS_STATIC = xyes ; then
    OLDLIBS="$LIBS"
    LIBS="$QT_LDADD"
    AC_CHECK_LIB(Xft, XftFontOpen, QT_LDADD="$QT_LDADD -lXft")
    LIBS="$LIBS"
fi

AC_MSG_CHECKING([QT_CXXFLAGS])
AC_MSG_RESULT([$QT_CXXFLAGS])
AC_MSG_CHECKING([QT_LDADD])
AC_MSG_RESULT([$QT_LDADD])

AC_SUBST(QT_CXXFLAGS)
AC_SUBST(QT_LDADD)
AC_SUBST(QT_GUILINK)
AC_SUBST(QASSISTANTCLIENT_LDADD)

])


# **************************************************************************
# SIM_AC_SETUP_MSVC_IFELSE( IF-FOUND, IF-NOT-FOUND )
#
# This macro invokes IF-FOUND if the wrapmsvc wrapper can be run, and
# IF-NOT-FOUND if not.
#
# Authors:
#   Morten Eriksen <mortene@coin3d.org>
#   Lars J. Aas <larsa@coin3d.org>

# **************************************************************************

AC_DEFUN([SIM_AC_SETUP_MSVC_IFELSE],
[#
**************************************************************************
# If the Microsoft Visual C++ cl.exe compiler is available, set us up for
# compiling with it and to generate an MSWindows .dll file.

: ${BUILD_WITH_MSVC=false}
sim_ac_wrapmsvc=d $srcdir; pwd/cfg/m4/wrapmsvc.exe
if test -z "$CC" -a -z "$CXX" && $sim_ac_wrapmsvc >/dev/null 2>&1; then
  m4_ifdef([$0_VISITED],
    [AC_FATAL([Macro $0 invoked multiple times])])
  m4_define([$0_VISITED], 1)
  CC=$sim_ac_wrapmsvc
  CXX=$sim_ac_wrapmsvc
  export CC CXX
  BUILD_WITH_MSVC=true
fi
AC_SUBST(BUILD_WITH_MSVC)

case $CXX in
*wrapmsvc.exe)
  BUILD_WITH_MSVC=true
  $1
  ;;
*)
  BUILD_WITH_MSVC=false
  $2
  ;;
esac
]) # SIM_AC_SETUP_MSVC_IFELSE

# **************************************************************************
# SIM_AC_SETUP_MSVCRT
#
# This macro sets up compiler flags for the MS Visual C++ C library of
# choice.

AC_DEFUN([SIM_AC_SETUP_MSVCRT],
[sim_ac_msvcrt_LDFLAGS=""
sim_ac_msvcrt_LIBS=""

AC_ARG_WITH([msvcrt],
  [AC_HELP_STRING([--with-msvcrt=<crt>],
                  [set which C run-time library to build against])],
  [case cho "$withval" | tr "[A-Z]" "[a-z]" in
  default | singlethread-static | ml | /ml | libc | libc\.lib )
    sim_ac_msvcrt=singlethread-static
    sim_ac_msvcrt_CFLAGS="/ML"
    sim_ac_msvcrt_CXXFLAGS="/ML"
    ;;
  default-debug | singlethread-static-debug | mld | /mld | libcd |
libcd\.lib )
    sim_ac_msvcrt=singlethread-static-debug
    sim_ac_msvcrt_CFLAGS="/MLd"
    sim_ac_msvcrt_CXXFLAGS="/MLd"
    ;;
  multithread-static | mt | /mt | libcmt | libcmt\.lib )
    sim_ac_msvcrt=multithread-static
    sim_ac_msvcrt_CFLAGS="/MT"
    sim_ac_msvcrt_CXXFLAGS="/MT"
    ;;
  multithread-static-debug | mtd | /mtd | libcmtd | libcmtd\.lib )
    sim_ac_msvcrt=multithread-static-debug
    sim_ac_msvcrt_CFLAGS="/MTd"
    sim_ac_msvcrt_CXXFLAGS="/MTd"
    ;;
  multithread-dynamic | md | /md | msvcrt | msvcrt\.lib )
    sim_ac_msvcrt=multithread-dynamic
    sim_ac_msvcrt_CFLAGS="/MD"
    sim_ac_msvcrt_CXXFLAGS="/MD"
    ;;
  multithread-dynamic-debug | mdd | /mdd | msvcrtd | msvcrtd\.lib )
    sim_ac_msvcrt=multithread-dynamic-debug
    sim_ac_msvcrt_CFLAGS="/MDd"
    sim_ac_msvcrt_CXXFLAGS="/MDd"
    ;;
  *)
    SIM_AC_ERROR([invalid-msvcrt])
    ;;
  esac],
  [sim_ac_msvcrt=singlethread-static])

AC_MSG_CHECKING([MSVC++ C library choice])
AC_MSG_RESULT([$sim_ac_msvcrt])

$1
]) # SIM_AC_SETUP_MSVCRT


# **************************************************************************
# SIM_AC_SETUP_MSVC_IFELSE( IF-FOUND, IF-NOT-FOUND )
#
# This macro invokes IF-FOUND if the wrapmsvc wrapper can be run, and
# IF-NOT-FOUND if not.
#
# Authors:
#   Morten Eriksen <mortene@coin3d.org>
#   Lars J. Aas <larsa@coin3d.org>

# **************************************************************************

AC_DEFUN([SIM_AC_SETUP_MSVC_IFELSE],
[#
**************************************************************************
# If the Microsoft Visual C++ cl.exe compiler is available, set us up for
# compiling with it and to generate an MSWindows .dll file.

: ${BUILD_WITH_MSVC=false}
sim_ac_wrapmsvc=d $srcdir; pwd/cfg/m4/wrapmsvc.exe
if test -z "$CC" -a -z "$CXX" && $sim_ac_wrapmsvc >/dev/null 2>&1; then
  m4_ifdef([$0_VISITED],
    [AC_FATAL([Macro $0 invoked multiple times])])
  m4_define([$0_VISITED], 1)
  CC=$sim_ac_wrapmsvc
  CXX=$sim_ac_wrapmsvc
  export CC CXX
  BUILD_WITH_MSVC=true
fi
AC_SUBST(BUILD_WITH_MSVC)

case $CXX in
*wrapmsvc.exe)
  BUILD_WITH_MSVC=true
  $1
  ;;
*)
  BUILD_WITH_MSVC=false
  $2
  ;;
esac
]) # SIM_AC_SETUP_MSVC_IFELSE

# **************************************************************************
# SIM_AC_SETUP_MSVCRT
#
# This macro sets up compiler flags for the MS Visual C++ C library of
# choice.

AC_DEFUN([SIM_AC_SETUP_MSVCRT],
[sim_ac_msvcrt_LDFLAGS=""
sim_ac_msvcrt_LIBS=""

AC_ARG_WITH([msvcrt],
  [AC_HELP_STRING([--with-msvcrt=<crt>],
                  [set which C run-time library to build against])],
  [case cho "$withval" | tr "[A-Z]" "[a-z]" in
  default | singlethread-static | ml | /ml | libc | libc\.lib )
    sim_ac_msvcrt=singlethread-static
    sim_ac_msvcrt_CFLAGS="/ML"
    sim_ac_msvcrt_CXXFLAGS="/ML"
    ;;
  default-debug | singlethread-static-debug | mld | /mld | libcd |
libcd\.lib )
    sim_ac_msvcrt=singlethread-static-debug
    sim_ac_msvcrt_CFLAGS="/MLd"
    sim_ac_msvcrt_CXXFLAGS="/MLd"
    ;;
  multithread-static | mt | /mt | libcmt | libcmt\.lib )
    sim_ac_msvcrt=multithread-static
    sim_ac_msvcrt_CFLAGS="/MT"
    sim_ac_msvcrt_CXXFLAGS="/MT"
    ;;
  multithread-static-debug | mtd | /mtd | libcmtd | libcmtd\.lib )
    sim_ac_msvcrt=multithread-static-debug
    sim_ac_msvcrt_CFLAGS="/MTd"
    sim_ac_msvcrt_CXXFLAGS="/MTd"
    ;;
  multithread-dynamic | md | /md | msvcrt | msvcrt\.lib )
    sim_ac_msvcrt=multithread-dynamic
    sim_ac_msvcrt_CFLAGS="/MD"
    sim_ac_msvcrt_CXXFLAGS="/MD"
    ;;
  multithread-dynamic-debug | mdd | /mdd | msvcrtd | msvcrtd\.lib )
    sim_ac_msvcrt=multithread-dynamic-debug
    sim_ac_msvcrt_CFLAGS="/MDd"
    sim_ac_msvcrt_CXXFLAGS="/MDd"
    ;;
  *)
    SIM_AC_ERROR([invalid-msvcrt])
    ;;
  esac],
  [sim_ac_msvcrt=singlethread-static])

AC_MSG_CHECKING([MSVC++ C library choice])
AC_MSG_RESULT([$sim_ac_msvcrt])

$1
]) # SIM_AC_SETUP_MSVCRT


# **************************************************************************
# gendsp.m4
#
# macros:
#   SIM_AC_MSVC_DSP_ENABLE_OPTION
#   SIM_AC_MSVC_DSP_SETUP(PROJECT, Project, project, extra-args)
#
# authors:
#   Lars J. Aas <larsa@coin3d.org>

# **************************************************************************
AC_DEFUN([SIM_AC_MSVC_DSP_ENABLE_OPTION], [
AC_ARG_ENABLE([msvcdsp],
  [AC_HELP_STRING([--enable-msvcdsp], [build .dsp, not library])],
  [case $enableval in
  no | false) sim_ac_make_dsp=false ;;
  *)          sim_ac_make_dsp=true ;;
  esac],
  [sim_ac_make_dsp=false])

if $sim_ac_make_dsp; then
  enable_dependency_tracking=no
  enable_libtool_lock=no
fi
]) # SIM_AC_MSVC_DSP_ENABLE_OPTION

# **************************************************************************
AC_DEFUN([SIM_AC_MSVC_DSP_SETUP], [
AC_REQUIRE([SIM_AC_MSVC_DSP_ENABLE_OPTION])
$1_DSP_LIBDIRS=
$1_DSP_LIBS=
$1_DSP_INCS=
$1_DSP_DEFS=

if $sim_ac_make_dsp; then
  SIM_AC_CONFIGURATION_SETTING([$2 build type], [msvc .dsp])

  # -DHAVE_CONFIG_H is set up in $DEFS too late for use to use, and some
  # include directives are usually set up in the Makefile.am files
  for arg in -DHAVE_CONFIG_H $4 $CPPFLAGS $LDFLAGS $LIBS; do
    case $arg in
    -L* )
      libdir=`echo $arg | cut -c3-`
      $1_DSP_LIBDIRS="[$]$1_DSP_LIBDIRS $libdir"
      ;;
    -l* )
      libname=`echo $arg | cut -c3-`
      for libdir in [$]$1_DSP_LIBDIRS; do
        if test -f $libdir/$libname.lib; then
          # lib is not in any standard location - use full path
          libname=`cygpath -w "$libdir/$libname" 2>/dev/null || echo "$libdir/$libname"`
          break
        fi
      done
      if test x"[$]$1_DSP_LIBS" = x""; then
        $1_DSP_LIBS="$libname.lib"
      else
        $1_DSP_LIBS="[$]$1_DSP_LIBS $libname.lib"
      fi
      ;;
    -I* )
      incdir=`echo $arg | cut -c3-`
      incdir=`cygpath -w "$incdir" 2>/dev/null || echo "$incdir"`
      if test x"[$]$1_DSP_INCS" = x""; then
        $1_DSP_INCS="/I \"$incdir\""
      else
        $1_DSP_INCS="[$]$1_DSP_INCS /I \"$incdir\""
      fi
      ;;
    -D$1_DEBUG* | -DNDEBUG )
      # Defines that vary between release/debug configurations can't be
      # set up dynamically in <lib>_DSP_DEFS - they must be static in the
      # gendsp.sh script.  We therefore catche them here so we can ignore
      # checking for them below.
      ;;
    -D*=* | -D* )
      define=`echo $arg | cut -c3-`
      if test x"[$]$1_DSP_DEFS" = x""; then
        $1_DSP_DEFS="/D \"$define\""
      else
        $1_DSP_DEFS="[$]$1_DSP_DEFS /D \"$define\""
      fi
      ;;
    esac
  done

  CC=[$]$3_src_dir/cfg/m4/gendsp.sh
  CXX=[$]$3_src_dir/cfg/m4/gendsp.sh
  CXXLD=[$]$3_src_dir/cfg/m4/gendsp.sh
  # Yes, this is totally bogus stuff, but don't worry about it.  As long
  # as gendsp.sh recognizes it...  20030219 larsa
  CPPFLAGS="$CPPFLAGS -Ddspfile=[$]$3_build_dir/$3[$]$1_MAJOR_VERSION.dsp"
  LDFLAGS="$LDFLAGS -Wl,-Ddspfile=[$]$3_build_dir/$3[$]$1_MAJOR_VERSION.dsp"
  LIBFLAGS="$LIBFLAGS -o $3[$]$1_MAJOR_VERSION.so.0"

  # this can't be set up at the point the libtool script is generated
  mv libtool libtool.bak
  sed -e "s%^CC=\"gcc\"%CC=\"[$]$3_src_dir/cfg/m4/gendsp.sh\"%" \
      -e "s%^CC=\".*/wrapmsvc.exe\"%CC=\"[$]$3_src_dir/cfg/m4/gendsp.sh\"%" \
      <libtool.bak >libtool
  rm -f libtool.bak
  chmod 755 libtool
fi

AC_SUBST([$1_DSP_LIBS])
AC_SUBST([$1_DSP_INCS])
AC_SUBST([$1_DSP_DEFS])
])


# **************************************************************************
# configuration_summary.m4
#
# This file contains some utility macros for making it easy to have a short
# summary of the important configuration settings printed at the end of the
# configure run.
#
# Authors:
#   Lars J. Aas <larsa@sim.no>
#

# **************************************************************************
# SIM_AC_CONFIGURATION_SETTING( DESCRIPTION, SETTING )
#
# This macro registers a configuration setting to be dumped by the
# SIM_AC_CONFIGURATION_SUMMARY macro.

AC_DEFUN([SIM_AC_CONFIGURATION_SETTING],
[if test x${sim_ac_configuration_settings+set} != xset; then
  sim_ac_configuration_settings="$1:$2"
else
  sim_ac_configuration_settings="$sim_ac_configuration_settings|$1:$2"
fi
]) # SIM_AC_CONFIGURATION_SETTING

# **************************************************************************
# SIM_AC_CONFIGURATION_WARNING( WARNING )
#
# This macro registers a configuration warning to be dumped by the
# SIM_AC_CONFIGURATION_SUMMARY macro.

AC_DEFUN([SIM_AC_CONFIGURATION_WARNING],
[if test x${sim_ac_configuration_warnings+set} != xset; then
  sim_ac_configuration_warnings="$1"
else
  sim_ac_configuration_warnings="$sim_ac_configuration_warnings|$1"
fi
]) # SIM_AC_CONFIGURATION_WARNING

# **************************************************************************
# SIM_AC_CONFIGURATION_SUMMARY
#
# This macro dumps the settings and warnings summary.

AC_DEFUN([SIM_AC_CONFIGURATION_SUMMARY],
[sim_ac_settings=$sim_ac_configuration_settings
sim_ac_num_settings=`echo "$sim_ac_settings" | tr -d -c "|" | wc -c`
sim_ac_maxlength=0
while test $sim_ac_num_settings -ge 0; do
  sim_ac_description=`echo "$sim_ac_settings" | cut -d: -f1`
  sim_ac_length=`echo "$sim_ac_description" | wc -c`
  if test $sim_ac_length -gt $sim_ac_maxlength; then
    sim_ac_maxlength=`expr $sim_ac_length + 0`
  fi
  sim_ac_settings=`echo $sim_ac_settings | cut -d"|" -f2-`
  sim_ac_num_settings=`expr $sim_ac_num_settings - 1`
done

sim_ac_maxlength=`expr $sim_ac_maxlength + 3`
sim_ac_padding=`echo "                                             " |
  cut -c1-$sim_ac_maxlength`

sim_ac_num_settings=`echo "$sim_ac_configuration_settings" | tr -d -c "|" | wc -c`
echo ""
echo "$PACKAGE configuration settings:"
while test $sim_ac_num_settings -ge 0; do
  sim_ac_setting=`echo $sim_ac_configuration_settings | cut -d"|" -f1`
  sim_ac_description=`echo "$sim_ac_setting" | cut -d: -f1`
  sim_ac_status=`echo "$sim_ac_setting" | cut -d: -f2-`
  # hopefully not too many terminals are too dumb for this
  echo -e "$sim_ac_padding $sim_ac_status\r  $sim_ac_description:"
  sim_ac_configuration_settings=`echo $sim_ac_configuration_settings | cut -d"|" -f2-`
  sim_ac_num_settings=`expr $sim_ac_num_settings - 1`
done

if test x${sim_ac_configuration_warnings+set} = xset; then
sim_ac_num_warnings=`echo "$sim_ac_configuration_warnings" | tr -d -c "|" | wc -c`
echo ""
echo "$PACKAGE configuration warnings:"
while test $sim_ac_num_warnings -ge 0; do
  sim_ac_warning=`echo "$sim_ac_configuration_warnings" | cut -d"|" -f1`
  echo "  * $sim_ac_warning"
  sim_ac_configuration_warnings=`echo $sim_ac_configuration_warnings | cut -d"|" -f2-`
  sim_ac_num_warnings=`expr $sim_ac_num_warnings - 1`
done
fi
]) # SIM_AC_CONFIGURATION_SUMMARY


# **************************************************************************
# SIM_AC_SETUP_MSVC_IFELSE( IF-FOUND, IF-NOT-FOUND )
#
# This macro invokes IF-FOUND if the wrapmsvc wrapper can be run, and
# IF-NOT-FOUND if not.
#
# Authors:
#   Morten Eriksen <mortene@coin3d.org>
#   Lars J. Aas <larsa@coin3d.org>

# **************************************************************************

AC_DEFUN([SIM_AC_SETUP_MSVC_IFELSE],
[# **************************************************************************
# If the Microsoft Visual C++ cl.exe compiler is available, set us up for
# compiling with it and to generate an MSWindows .dll file.

: ${BUILD_WITH_MSVC=false}
sim_ac_wrapmsvc=`cd $srcdir; pwd`/cfg/m4/wrapmsvc.exe
if test -z "$CC" -a -z "$CXX" && $sim_ac_wrapmsvc >/dev/null 2>&1; then
  m4_ifdef([$0_VISITED],
    [AC_FATAL([Macro $0 invoked multiple times])])
  m4_define([$0_VISITED], 1)
  CC=$sim_ac_wrapmsvc
  CXX=$sim_ac_wrapmsvc
  export CC CXX
  BUILD_WITH_MSVC=true
fi
AC_SUBST(BUILD_WITH_MSVC)

case $CXX in
*wrapmsvc.exe)
  BUILD_WITH_MSVC=true
  $1
  ;;
*)
  BUILD_WITH_MSVC=false
  $2
  ;;
esac
]) # SIM_AC_SETUP_MSVC_IFELSE

# **************************************************************************
# SIM_AC_SETUP_MSVCRT
#
# This macro sets up compiler flags for the MS Visual C++ C library of
# choice.

AC_DEFUN([SIM_AC_SETUP_MSVCRT],
[sim_ac_msvcrt_LDFLAGS=""
sim_ac_msvcrt_LIBS=""

AC_ARG_WITH([msvcrt],
  [AC_HELP_STRING([--with-msvcrt=<crt>],
                  [set which C run-time library to build against])],
  [case `echo "$withval" | tr "[A-Z]" "[a-z]"` in
  default | singlethread-static | ml | /ml | libc | libc\.lib )
    sim_ac_msvcrt=singlethread-static
    sim_ac_msvcrt_CFLAGS="/ML"
    sim_ac_msvcrt_CXXFLAGS="/ML"
    ;;
  default-debug | singlethread-static-debug | mld | /mld | libcd | libcd\.lib )
    sim_ac_msvcrt=singlethread-static-debug
    sim_ac_msvcrt_CFLAGS="/MLd"
    sim_ac_msvcrt_CXXFLAGS="/MLd"
    ;;
  multithread-static | mt | /mt | libcmt | libcmt\.lib )
    sim_ac_msvcrt=multithread-static
    sim_ac_msvcrt_CFLAGS="/MT"
    sim_ac_msvcrt_CXXFLAGS="/MT"
    ;;
  multithread-static-debug | mtd | /mtd | libcmtd | libcmtd\.lib )
    sim_ac_msvcrt=multithread-static-debug
    sim_ac_msvcrt_CFLAGS="/MTd"
    sim_ac_msvcrt_CXXFLAGS="/MTd"
    ;;
  multithread-dynamic | md | /md | msvcrt | msvcrt\.lib )
    sim_ac_msvcrt=multithread-dynamic
    sim_ac_msvcrt_CFLAGS="/MD"
    sim_ac_msvcrt_CXXFLAGS="/MD"
    ;;
  multithread-dynamic-debug | mdd | /mdd | msvcrtd | msvcrtd\.lib )
    sim_ac_msvcrt=multithread-dynamic-debug
    sim_ac_msvcrt_CFLAGS="/MDd"
    sim_ac_msvcrt_CXXFLAGS="/MDd"
    ;;
  *)
    SIM_AC_ERROR([invalid-msvcrt])
    ;;
  esac],
  [sim_ac_msvcrt=singlethread-static])

AC_MSG_CHECKING([MSVC++ C library choice])
AC_MSG_RESULT([$sim_ac_msvcrt])

$1
]) # SIM_AC_SETUP_MSVCRT

# EOF **********************************************************************

# **************************************************************************
# SIM_AC_ERROR_MESSAGE_FILE( FILENAME )
#   Sets the error message file.  Default is $ac_aux_dir/m4/errors.txt.
#
# SIM_AC_ERROR( ERROR [, ERROR ...] )
#   Fetches the error messages from the error message file and displays
#   them on stderr.
#
# SIM_AC_WITH_ERROR( WITHARG )
#   Invokes AC_MSG_ERROR in a consistent way for problems with the --with-*
#   $withval argument.
#
# SIM_AC_ENABLE_ERROR( ENABLEARG )
#   Invokes AC_MSG_ERROR in a consistent way for problems with the --enable-*
#   $enableval argument.
#
# Authors:
#   Lars J. Aas <larsa@sim.no>

AC_DEFUN([SIM_AC_ERROR_MESSAGE_FILE], [
sim_ac_message_file=$1
]) # SIM_AC_ERROR_MESSAGE_FILE

AC_DEFUN([SIM_AC_ONE_MESSAGE], [
: ${sim_ac_message_file=$ac_aux_dir/m4/errors.txt}
if test -f $sim_ac_message_file; then
  sim_ac_message="`sed -n -e '/^!$1$/,/^!/ { /^!/ d; p; }' <$sim_ac_message_file`"
  if test x"$sim_ac_message" = x""; then
    AC_MSG_ERROR([no message named '$1' in '$sim_ac_message_file' - notify the $PACKAGE_NAME maintainer(s)])
  else
    eval "echo >&2 \"$sim_ac_message\""
  fi
else
  AC_MSG_ERROR([file '$sim_ac_message_file' not found - notify the $PACKAGE_NAME maintainer(s)])
fi
]) # SIM_AC_ONE_MESSAGE

AC_DEFUN([_SIM_AC_ERROR], [
SIM_AC_ONE_MESSAGE([$1])
ifelse([$2], , , [
echo >&2 ""
_SIM_AC_ERROR(m4_shift($@))])
]) # _SIM_AC_ERROR

AC_DEFUN([SIM_AC_ERROR], [
echo >&2 ""
_SIM_AC_ERROR($@)
echo >&2 ""
AC_MSG_ERROR([aborting])
]) # SIM_AC_ERROR

AC_DEFUN([SIM_AC_WITH_ERROR], [
AC_MSG_ERROR([invalid value "${withval}" for "$1" configure argument])
]) # SIM_AC_WITH_ERROR

AC_DEFUN([SIM_AC_ENABLE_ERROR], [
AC_MSG_ERROR([invalid value "${enableval}" for "$1" configure argument])
]) # SIM_AC_ENABLE_ERROR



# **************************************************************************
# SIM_AC_MAC_CPP_ADJUSTMENTS
#
# Add --no-cpp-precomp if necessary. Without this option, the
# Apple preprocessor is used on Mac OS X platforms, and it is
# known to be very buggy.  It's better to use this option, so
# the GNU preprocessor is preferred.
#


AC_DEFUN([SIM_AC_MAC_CPP_ADJUSTMENTS],
[case $host_os in
darwin*)
  if test x"$GCC" = x"yes"; then
    # FIXME: create a SIM_AC_CPP_OPTION macro
    SIM_AC_CC_COMPILER_OPTION([-no-cpp-precomp], [CPPFLAGS="$CPPFLAGS -no-cpp-precomp"])
  fi
  ;;
esac
]) # SIM_AC_MAC_CPP_ADJUSTMENTS

# **************************************************************************
# This macro sets up the MAC_FRAMEWORK automake conditional, depending on
# the host OS and whether $sim_ac_prefer_framework has been overridden or
# not.

AC_DEFUN([SIM_AC_MAC_FRAMEWORK],
[case $host_os in
darwin*)
  : ${sim_ac_prefer_framework=true}
  ;;
esac
: ${sim_ac_prefer_framework=false}
# This AM_CONDITIONAL can be used to make Mac OS X specific make-rules
# related to installing proper Frameworks instead.
AM_CONDITIONAL([MAC_FRAMEWORK], [$sim_ac_prefer_framework])

if $sim_ac_prefer_framework; then
  ifelse([$1], , :, [$1])
else
  ifelse([$2], , :, [$2])
fi
]) # SIM_AC_MAC_FRAMEWORK


#   Use this file to store miscellaneous macros related to checking
#   compiler features.

# Usage:
#   SIM_AC_CC_COMPILER_OPTION(OPTION-TO-TEST, ACTION-IF-TRUE [, ACTION-IF-FALSE])
#   SIM_AC_CXX_COMPILER_OPTION(OPTION-TO-TEST, ACTION-IF-TRUE [, ACTION-IF-FALSE])
#
# Description:
#
#   Check whether the current C or C++ compiler can handle a
#   particular command-line option.
#
#
# Author: Morten Eriksen, <mortene@sim.no>.
#
#   * [mortene:19991218] improve macros by catching and analyzing
#     stderr (at least to see if there was any output there)?
#

AC_DEFUN([SIM_AC_COMPILER_OPTION], [
sim_ac_save_cppflags=$CPPFLAGS
CPPFLAGS="$CPPFLAGS $1"
AC_TRY_COMPILE([], [], [sim_ac_accept_result=yes], [sim_ac_accept_result=no])
AC_MSG_RESULT([$sim_ac_accept_result])
CPPFLAGS=$sim_ac_save_cppflags
# This need to go last, in case CPPFLAGS is modified in $2 or $3.
if test $sim_ac_accept_result = yes; then
  ifelse($2, , :, $2)
else
  ifelse($3, , :, $3)
fi
])

AC_DEFUN([SIM_AC_CC_COMPILER_OPTION], [
AC_LANG_SAVE
AC_LANG_C
AC_MSG_CHECKING([whether $CC accepts $1])
SIM_AC_COMPILER_OPTION($1, $2, $3)
AC_LANG_RESTORE
])

AC_DEFUN([SIM_AC_CXX_COMPILER_OPTION], [
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_MSG_CHECKING([whether $CXX accepts $1])
SIM_AC_COMPILER_OPTION($1, $2, $3)
AC_LANG_RESTORE
])

#
# This file contains misc "macro-containers" for stuff that is
# common between the various configure.ac files of the So* libraries.
#
# Note: none of the macros in this file is likely to be well designed
# and made for generic use in any project.
#

# Usage:
#   SIM_AC_SOGUI_SETUP_DOXYGEN( LIBRARY-NAME )
#
# Description:
#   Do the necessary configurations for setting up HTML and man-page
#   building by the Doxygen documentation system.
#
# Authors:
#   Morten Eriksen, <mortene@sim.no>
#   Lars Jørgen Aas, <larsa@sim.no>


AC_DEFUN([SIM_AC_SOGUI_SETUP_DOXYGEN],
[
AC_ARG_VAR([htmldir],
           [destination for HTML docs (default ${datadir}/$1/html)])

AC_ARG_ENABLE(html,
  AC_HELP_STRING([--enable-html], [build and install $1 HTML documentation]),
  [case $enableval in
    yes | true) want_html=yes ;;
    *)          want_html=no ;;
  esac],
  [want_html=no])

case $htmldir in
"")
  htmldir="$datadir/$1/html"
  ;;
/*)
  # do nothing - absolute path
  ;;
*)
  htmldir="\${prefix}/$htmldir"
  ;;
esac

AC_SUBST(htmldir)

AC_ARG_ENABLE(man,
  AC_HELP_STRING([--enable-man], [build and install $1 man pages]),
  [case $enableval in
    yes | true) want_man=yes ;;
    *)          want_man=no ;;
  esac],
  [want_man=no])

# Used in the Doxygen parameter file.
AC_SUBST([SOGUI_DOC_HTML], [`echo $want_html | tr '[a-z]' '[A-Z]'`])
AC_SUBST([SOGUI_DOC_MAN], [`echo $want_man | tr '[a-z]' '[A-Z]'`])

AC_SUBST([sogui_build_dir], [`pwd`])
AC_SUBST([sogui_src_dir], [`cd $srcdir; pwd`])
AC_SUBST([sogui_html_dir], [`pwd`/html])
AC_SUBST([sogui_man_dir], [`pwd`/man])

AM_CONDITIONAL(BUILD_MANPAGES, test x"$want_man" = x"yes")
AM_CONDITIONAL(BUILD_HTMLPAGES, test x"$want_html" = x"yes")

if test x"$want_man" = x"yes"; then
  SIM_AC_CONFIGURATION_SETTING([manpage installation], [$mandir])
fi

if test x"$want_html" = x"yes"; then
  SIM_AC_CONFIGURATION_SETTING([HTML installation], [$htmldir])
fi

if test x"$want_html" != xno -o x"$want_man" != xno; then
  SIM_AC_DOXYGEN_TOOL([], [SIM_AC_ERROR([no-doxygen])])

  AC_PATH_PROG(sim_ac_perl_exe, perl, false, $PATH)
  if test x"$sim_ac_perl_exe" = xfalse; then
    AC_MSG_WARN(Could not find the Perl executable)
  fi
fi

# path_tag is used to identify paths in docs/coin.doxygen that needs to be
# transformed using cygpath under cygwin.

case $host in
*-cygwin) path_tag="<PATH>" ;;
*)        path_tag= ;;
esac

AC_SUBST(path_tag)

]) # SIM_AC_SOGUI_SETUP_DOXYGEN()

# Usage:
#  SIM_AC_DOXYGEN_TOOL([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
# Description:
#   This macro locates the doxygen executable. If it is found, the
#   variable $sim_ac_doxygen_exe is set to the full path- and
#   executable name (if not found, it is set to "false") and
#   $sim_ac_doxygen_avail is set to the version number (if not
#   found, it is set to "no").
#
# Author: Morten Eriksen, <mortene@sim.no>.

AC_DEFUN(SIM_AC_DOXYGEN_TOOL, [
AC_ARG_WITH(
  [doxygen],
  AC_HELP_STRING([--with-doxygen=DIR],
                 [DIR is the directory where the doxygen executable resides]),
  [],
  [with_doxygen=yes])

sim_ac_doxygen_avail=no

if test x"$with_doxygen" != xno; then
  sim_ac_path=$PATH
  if test x"$with_doxygen" != xyes; then
    sim_ac_path=${with_doxygen}:$PATH
  fi

  AC_PATH_PROG(sim_ac_doxygen_exe, doxygen, false, $sim_ac_path)
  if test x"$sim_ac_doxygen_exe" = xfalse; then
    ifelse([$2], , :, [$2])
  else
    sim_ac_doxygen_avail=`$sim_ac_doxygen_exe -help 2> /dev/null | head -1 | sed 's%[[^ ]]\+ [[^ ]]\+ %%'`
    $1
  fi
fi
])


# SIM_AC_CHECK_DL([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
# ----------------------------------------------------------
#
#  Try to find the dynamic link loader library. If it is found, these
#  shell variables are set:
#
#    $sim_ac_dl_cppflags (extra flags the compiler needs for dl lib)
#    $sim_ac_dl_ldflags  (extra flags the linker needs for dl lib)
#    $sim_ac_dl_libs     (link libraries the linker needs for dl lib)
#
#  The CPPFLAGS, LDFLAGS and LIBS flags will also be modified accordingly.
#
# Author: Morten Eriksen, <mortene@sim.no>.

AC_DEFUN([SIM_AC_CHECK_DL], [
AC_ARG_WITH(
  [dl],
  [AC_HELP_STRING(
    [--with-dl=DIR],
    [include support for the dynamic link loader library [default=yes]])],
  [],
  [with_dl=yes])

if test x"$with_dl" != xno; then
  if test x"$with_dl" != xyes; then
    sim_ac_dl_cppflags="-I${with_dl}/include"
    sim_ac_dl_ldflags="-L${with_dl}/lib"
  fi

  sim_ac_save_cppflags=$CPPFLAGS
  sim_ac_save_ldflags=$LDFLAGS
  sim_ac_save_libs=$LIBS

  CPPFLAGS="$CPPFLAGS $sim_ac_dl_cppflags"
  LDFLAGS="$LDFLAGS $sim_ac_dl_ldflags"

  # Use SIM_AC_CHECK_HEADERS instead of .._HEADER to get the
  # HAVE_DLFCN_H symbol set up in config.h automatically.
  AC_CHECK_HEADERS([dlfcn.h])

  sim_ac_dl_avail=false

  AC_MSG_CHECKING([for the dl library])
  # At least under FreeBSD, dlopen() et al is part of the C library.
  for sim_ac_dl_libcheck in "" "-ldl"; do
    if ! $sim_ac_dl_avail; then
      LIBS="$sim_ac_dl_libcheck $sim_ac_save_libs"
      AC_TRY_LINK([
#if HAVE_DLFCN_H
#include <dlfcn.h>
#endif /* HAVE_DLFCN_H */
],
                  [(void)dlopen(0L, 0); (void)dlsym(0L, "Gunners!"); (void)dlclose(0L);],
                  [sim_ac_dl_avail=true
                   sim_ac_dl_libs="$sim_ac_dl_libcheck"
                  ])
    fi
  done

  if $sim_ac_dl_avail; then
    if test x"$sim_ac_dl_libs" = x""; then
      AC_MSG_RESULT(yes)
    else
      AC_MSG_RESULT($sim_ac_dl_cppflags $sim_ac_dl_ldflags $sim_ac_dl_libs)
    fi
  else
    AC_MSG_RESULT(not available)
  fi

  if $sim_ac_dl_avail; then
    ifelse([$1], , :, [$1])
  else
    CPPFLAGS=$sim_ac_save_cppflags
    LDFLAGS=$sim_ac_save_ldflags
    LIBS=$sim_ac_save_libs
    ifelse([$2], , :, [$2])
  fi
fi
])

# SIM_AC_CHECK_LOADLIBRARY([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
# -------------------------------------------------------------------
#
#  Try to use the Win32 dynamic link loader methods LoadLibrary(),
#  GetProcAddress() and FreeLibrary().
#
# Author: Morten Eriksen, <mortene@sim.no>.

AC_DEFUN([SIM_AC_CHECK_LOADLIBRARY], [
AC_ARG_ENABLE(
  [loadlibrary],
  [AC_HELP_STRING([--disable-loadlibrary], [don't use run-time link bindings under Win32])],
  [case $enableval in
  yes | true ) sim_ac_win32_loadlibrary=true ;;
  *) sim_ac_win32_loadlibrary=false ;;
  esac],
  [sim_ac_win32_loadlibrary=true])

if $sim_ac_win32_loadlibrary; then
  # Use SIM_AC_CHECK_HEADERS instead of .._HEADER to get the
  # HAVE_DLFCN_H symbol set up in config.h automatically.
  AC_CHECK_HEADERS([windows.h])

  AC_CACHE_CHECK([whether the Win32 LoadLibrary() method is available],
    sim_cv_lib_loadlibrary_avail,
    [AC_TRY_LINK([
#if HAVE_WINDOWS_H
#include <windows.h>
#endif /* HAVE_WINDOWS_H */
],
                 [(void)LoadLibrary(0L); (void)GetProcAddress(0L, 0L); (void)FreeLibrary(0L); ],
                 [sim_cv_lib_loadlibrary_avail=yes],
                 [sim_cv_lib_loadlibrary_avail=no])])

  if test x"$sim_cv_lib_loadlibrary_avail" = xyes; then
    ifelse([$1], , :, [$1])
  else
    ifelse([$2], , :, [$2])
  fi
fi
])

# Usage:
#  SIM_AC_CHECK_X11([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
#  Try to find the X11 development system. If it is found, these
#  shell variables are set:
#
#    $sim_ac_x11_cppflags (extra flags the compiler needs for X11)
#    $sim_ac_x11_ldflags  (extra flags the linker needs for X11)
#    $sim_ac_x11_libs     (link libraries the linker needs for X11)
#
#  The CPPFLAGS, LDFLAGS and LIBS flags will also be modified accordingly.
#  In addition, the variable $sim_ac_x11_avail is set to "yes" if
#  the X11 development system is found.
#
#
# Author: Morten Eriksen, <mortene@sim.no>.

AC_DEFUN([SIM_AC_CHECK_X11], [
AC_REQUIRE([AC_PATH_XTRA])

sim_ac_x11_avail=no

if test x"$no_x" != xyes; then
  #  *** DEBUG ***
  #  Keep this around, as it can be handy when testing on new systems.
  # echo "X_CFLAGS: $X_CFLAGS"
  # echo "X_PRE_LIBS: $X_PRE_LIBS"
  # echo "X_LIBS: $X_LIBS"
  # echo "X_EXTRA_LIBS: $X_EXTRA_LIBS"
  # echo
  # exit 0

  sim_ac_x11_cppflags="$X_CFLAGS"
  sim_ac_x11_ldflags="$X_LIBS"
  sim_ac_x11_libs="$X_PRE_LIBS -lX11 $X_EXTRA_LIBS"

  sim_ac_save_cppflags=$CPPFLAGS
  sim_ac_save_ldflags=$LDFLAGS
  sim_ac_save_libs=$LIBS

  CPPFLAGS="$CPPFLAGS $sim_ac_x11_cppflags"
  LDFLAGS="$LDFLAGS $sim_ac_x11_ldflags"
  LIBS="$sim_ac_x11_libs $LIBS"

  AC_CACHE_CHECK(
    [whether we can link against X11],
    sim_cv_lib_x11_avail,
    [AC_TRY_LINK([#include <X11/Xlib.h>],
                 [(void)XOpenDisplay(0L);],
                 [sim_cv_lib_x11_avail=yes],
                 [sim_cv_lib_x11_avail=no])])

  if test x"$sim_cv_lib_x11_avail" = x"yes"; then
    sim_ac_x11_avail=yes
    $1
  else
    CPPFLAGS=$sim_ac_save_cppflags
    LDFLAGS=$sim_ac_save_ldflags
    LIBS=$sim_ac_save_libs
    $2
  fi
fi
])

# Usage:
#  SIM_AC_CHECK_X11SHMEM([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
#  Try to find the X11 shared memory extension. If it is found, this
#  shell variable is set:
#
#    $sim_ac_x11shmem_libs   (link libraries the linker needs for X11 Shm)
#
#  The LIBS flag will also be modified accordingly. In addition, the
#  variable $sim_ac_x11shmem_avail is set to "yes" if the X11 shared
#  memory extension is found.
#
#
# Author: Morten Eriksen, <mortene@sim.no>.
#
# TODO:
#    * [mortene:20000122] make sure this work on MSWin (with
#      Cygwin installation)
#

AC_DEFUN([SIM_AC_CHECK_X11SHMEM], [

sim_ac_x11shmem_avail=no
sim_ac_x11shmem_libs="-lXext"
sim_ac_save_libs=$LIBS
LIBS="$sim_ac_x11shmem_libs $LIBS"

AC_CACHE_CHECK(
  [whether the X11 shared memory extension is available],
  sim_cv_lib_x11shmem_avail,
  [AC_TRY_LINK([#include <X11/Xlib.h>
               #include <X11/extensions/XShm.h>],
               [(void)XShmQueryVersion(0L, 0L, 0L, 0L);],
               [sim_cv_lib_x11shmem_avail=yes],
               [sim_cv_lib_x11shmem_avail=no])])

if test x"$sim_cv_lib_x11shmem_avail" = xyes; then
  sim_ac_x11shmem_avail=yes
  $1
else
  LIBS=$sim_ac_save_libs
  $2
fi
])

# Usage:
#  SIM_AC_CHECK_X11MU([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
#  Try to find the X11 miscellaneous utilities extension. If it is
#  found, this shell variable is set:
#
#    $sim_ac_x11mu_libs   (link libraries the linker needs for X11 MU)
#
#  The LIBS flag will also be modified accordingly. In addition, the
#  variable $sim_ac_x11mu_avail is set to "yes" if the X11 miscellaneous
#  utilities extension is found.
#
# Author: Morten Eriksen, <mortene@sim.no>.
#
# TODO:
#    * [mortene:20000122] make sure this work on MSWin (with
#      Cygwin installation)
#

AC_DEFUN([SIM_AC_CHECK_X11MU], [

sim_ac_x11mu_avail=no
sim_ac_x11mu_libs="-lXmu"
sim_ac_save_libs=$LIBS
LIBS="$sim_ac_x11mu_libs $LIBS"

AC_CACHE_CHECK(
  [whether the X11 miscellaneous utilities is available],
  sim_cv_lib_x11mu_avail,
  [AC_TRY_LINK([#include <X11/Xlib.h>
                #include <X11/Xmu/Xmu.h>
                #include <X11/Xmu/StdCmap.h>],
               [(void)XmuAllStandardColormaps(0L);],
               [sim_cv_lib_x11mu_avail=yes],
               [sim_cv_lib_x11mu_avail=no])])

if test x"$sim_cv_lib_x11mu_avail" = xyes; then
  sim_ac_x11mu_avail=yes
  $1
else
  LIBS=$sim_ac_save_libs
  $2
fi
])

# Usage:
#  SIM_AC_CHECK_X11XID([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
#  Try to find the X11 extension device library. Sets this
#  shell variable:
#
#    $sim_ac_x11xid_libs   (link libraries the linker needs for X11 XID)
#
#  The LIBS flag will also be modified accordingly. In addition, the
#  variable $sim_ac_x11xid_avail is set to "yes" if the X11 extension
#  device library is found.
#
# Author: Morten Eriksen, <mortene@sim.no>.
#
# TODO:
#    * [mortene:20000122] make sure this work on MSWin (with
#      Cygwin installation)
#

AC_DEFUN([SIM_AC_CHECK_X11XID], [

sim_ac_x11xid_avail=no
sim_ac_x11xid_libs="-lXi"
sim_ac_save_libs=$LIBS
LIBS="$sim_ac_x11xid_libs $LIBS"

AC_CACHE_CHECK(
  [whether the X11 extension device library is available],
  sim_cv_lib_x11xid_avail,
  [AC_TRY_LINK([#include <X11/extensions/XInput.h>],
               [(void)XOpenDevice(0L, 0);],
               [sim_cv_lib_x11xid_avail=yes],
               [sim_cv_lib_x11xid_avail=no])])

if test x"$sim_cv_lib_x11xid_avail" = xyes; then
  sim_ac_x11xid_avail=yes
  $1
else
  LIBS=$sim_ac_save_libs
  $2
fi
])

# Usage:
#  SIM_AC_CHECK_X_INTRINSIC([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
#  Try to find the Xt intrinsic library. Sets this shell variable:
#
#    $sim_ac_xt_libs   (link library the linker needs for X Intrinsic)
#
#  The LIBS flag will also be modified accordingly. In addition, the
#  variable $sim_ac_xt_avail is set to "yes" if the X11 Intrinsic
#  library is found.
#
# Author: Morten Eriksen, <mortene@sim.no>.
#

AC_DEFUN([SIM_AC_CHECK_X_INTRINSIC], [

sim_ac_xt_avail=no
sim_ac_xt_libs="-lXt"
sim_ac_save_libs=$LIBS
LIBS="$sim_ac_xt_libs $LIBS"

AC_CACHE_CHECK(
  [whether the X11 Intrinsic library is available],
  sim_cv_lib_xt_avail,
  [AC_TRY_LINK([#include <X11/Intrinsic.h>],
               [(void)XtVaCreateWidget("", 0L, 0L);],
               [sim_cv_lib_xt_avail=yes],
               [sim_cv_lib_xt_avail=no])])

if test x"$sim_cv_lib_xt_avail" = xyes; then
  sim_ac_xt_avail=yes
  $1
else
  LIBS=$sim_ac_save_libs
  $2
fi
])

# Usage:
#   SIM_AC_CHECK_LIBXPM( [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND] )
#
# Description:
#   This macro checks for libXpm.
#
# Variables:
#   $sim_ac_xpm_avail      yes | no
#   $sim_ac_xpm_libs       [link-line libraries]
#
# Authors:
#   Lars J. Aas <larsa@sim.no>
#

AC_DEFUN([SIM_AC_CHECK_LIBXPM], [

sim_ac_xpm_avail=no
sim_ac_xpm_libs="-lXpm"

AC_CACHE_CHECK(
  [whether libXpm is available],
  sim_cv_lib_xpm_avail,
  [sim_ac_save_libs=$LIBS
  LIBS="$sim_ac_xpm_libs $LIBS"
  AC_TRY_LINK([#include <X11/xpm.h>],
              [(void)XpmLibraryVersion();],
              [sim_cv_lib_xpm_avail=yes],
              [sim_cv_lib_xpm_avail=no])
  LIBS="$sim_ac_save_libs"])

if test x"$sim_cv_lib_xpm_avail" = x"yes"; then
  sim_ac_xpm_avail=yes
  LIBS="$sim_ac_xpm_libs $LIBS"
  $1
else
  ifelse([$2], , :, [$2])
fi
])


# Usage:
#  SIM_AC_CHECK_X11_XP([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
#  Try to find the Xp library for printing functionality. Sets this
#  shell variable:
#
#    $sim_ac_xp_libs   (link library the linker needs for the Xp library)
#
#  The LIBS flag will also be modified accordingly. In addition, the
#  variable $sim_ac_xp_avail is set to "yes" if the Xp library is found.
#
# Author: Morten Eriksen, <mortene@sim.no>.
#

AC_DEFUN([SIM_AC_CHECK_X11_XP], [
sim_ac_xp_avail=no
sim_ac_xp_libs="-lXp"
sim_ac_save_libs=$LIBS
LIBS="$sim_ac_xp_libs $LIBS"

AC_CACHE_CHECK(
  [whether the X11 printing library is available],
  sim_cv_lib_xp_avail,
  [AC_TRY_LINK([#include <X11/extensions/Print.h>],
               [XpEndJob(0L);],
               [sim_cv_lib_xp_avail=yes],
               [sim_cv_lib_xp_avail=no])])

if test x"$sim_cv_lib_xp_avail" = xyes; then
  sim_ac_xp_avail=yes
  $1
else
  LIBS=$sim_ac_save_libs
  $2
fi
])

# SIM_AC_CHECK_X11_ATHENA( [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND] )

AC_DEFUN([SIM_AC_CHECK_X11_ATHENA], [
sim_ac_athena_avail=no
sim_ac_athena_libs="-lXaw"
sim_ac_save_libs=$LIBS
LIBS="$sim_ac_athena_libs $LIBS"

AC_CACHE_CHECK(
  [whether the X11 Athena widgets library is available],
  sim_cv_lib_athena_avail,
  [AC_TRY_LINK([#include <X11/Xfuncproto.h>
                #include <X11/Xaw/XawInit.h>],
               [XawInitializeWidgetSet();],
               [sim_cv_lib_athena_avail=yes],
               [sim_cv_lib_athena_avail=no])])

if test x"$sim_cv_lib_athena_avail" = xyes; then
  sim_ac_athena_avail=yes
  $1
else
  LIBS=$sim_ac_save_libs
  $2
fi
])

# SIM_AC_X11_READY( [ACTION-IF-TRUE], [ACTION-IF-FALSE] )

AC_DEFUN([SIM_AC_CHECK_X11_READY],
[AC_CACHE_CHECK(
  [if X11 linkage is ready],
  [sim_cv_x11_ready],
  [AC_TRY_LINK(
    [#include <X11/Xlib.h>],
    [(void)XOpenDisplay(0L);],
    [sim_cv_x11_ready=true],
    [sim_cv_x11_ready=false])])
if ${sim_cv_x11_ready}; then
  ifelse([$1], , :, [$1])
else
  ifelse([$2], , :, [$2])
fi
]) # SIM_AC_X11_READY()


# **************************************************************************

AC_DEFUN([SIM_AC_HAVE_LIBX11_IFELSE], [
: ${sim_ac_have_libx11=false}
AC_REQUIRE([AC_PATH_X])

# prevent multiple runs
$sim_ac_have_libx11 || {
  if test x"$no_x" != xyes; then
    sim_ac_libx11_cppflags=
    sim_ac_libx11_ldflags=
    test x"$x_includes" != x && sim_ac_libx11_cppflags="-I$x_includes"
    test x"$x_libraries" != x && sim_ac_libx11_ldflags="-L$x_libraries"
    sim_ac_libx11_libs="-lX11"

    sim_ac_libx11_save_cppflags=$CPPFLAGS
    sim_ac_libx11_save_ldflags=$LDFLAGS
    sim_ac_libx11_save_libs=$LIBS

    CPPFLAGS="$CPPFLAGS $sim_ac_libx11_cppflags"
    LDFLAGS="$LDFLAGS $sim_ac_libx11_ldflags"
    LIBS="$sim_ac_libx11_libs $LIBS"

    AC_TRY_LINK(
      [#include <X11/Xlib.h>],
      [(void)XOpenDisplay(0L);],
      [sim_ac_have_libx11=true])

    CPPFLAGS=$sim_ac_libx11_save_cppflags
    LDFLAGS=$sim_ac_libx11_save_ldflags
    LIBS=$sim_ac_libx11_save_libs
  fi
}

if $sim_ac_have_libx11; then
  ifelse([$1], , :, [$1])
else
  ifelse([$2], , :, [$2])
fi
]) # SIM_AC_HAVE_LIBX11_IFELSE


# **************************************************************************
# SIM_AC_CHECK_HEADER_SILENT([header], [if-found], [if-not-found], [includes])
# 
# This macro will not output any header checking information, nor will it
# cache the result, so it can be used multiple times on the same header,
# trying out different compiler options.

AC_DEFUN([SIM_AC_CHECK_HEADER_SILENT],
[AS_VAR_PUSHDEF([ac_Header], [ac_cv_header_$1])
m4_ifval([$4],
         [AC_COMPILE_IFELSE([AC_LANG_SOURCE([$4
@%:@include <$1>])],
                            [AS_VAR_SET(ac_Header, yes)],
                            [AS_VAR_SET(ac_Header, no)])],
         [AC_PREPROC_IFELSE([AC_LANG_SOURCE([@%:@include <$1>])],
                            [AS_VAR_SET(ac_Header, yes)],
                            [AS_VAR_SET(ac_Header, no)])])
AS_IF([test AS_VAR_GET(ac_Header) = yes], [$2], [$3])
AS_VAR_POPDEF([ac_Header])
])# SIM_AC_CHECK_HEADER_SILENT

# **************************************************************************
# SIM_AC_CHECK_HEADER_GL([IF-FOUND], [IF-NOT-FOUND])
#
# This macro detects how to include the GL header file, and gives you the
# necessary CPPFLAGS in $sim_ac_gl_cppflags, and also sets the config.h
# defines HAVE_GL_GL_H or HAVE_OPENGL_GL_H if one of them is found.

AC_DEFUN([SIM_AC_CHECK_HEADER_GL],
[sim_ac_gl_header_avail=false
AC_MSG_CHECKING([how to include gl.h])
if test x"$with_opengl" != x"no"; then
  sim_ac_gl_save_CPPFLAGS=$CPPFLAGS
  sim_ac_gl_cppflags=

  if test x"$with_opengl" != xyes && test x"$with_opengl" != x""; then
    sim_ac_gl_cppflags="-I${with_opengl}/include"
  else
    # On HP-UX platforms, OpenGL headers and libraries are usually installed
    # at this location.
    sim_ac_gl_hpux=/opt/graphics/OpenGL
    if test -d $sim_ac_gl_hpux; then
      sim_ac_gl_cppflags=-I$sim_ac_gl_hpux/include
    fi
  fi

  CPPFLAGS="$CPPFLAGS $sim_ac_gl_cppflags"

  SIM_AC_CHECK_HEADER_SILENT([GL/gl.h], [
    sim_ac_gl_header_avail=true
    sim_ac_gl_header=GL/gl.h
    AC_DEFINE([HAVE_GL_GL_H], 1, [define if the GL header should be included as GL/gl.h])
  ], [
    SIM_AC_CHECK_HEADER_SILENT([OpenGL/gl.h], [
      sim_ac_gl_header_avail=true
      sim_ac_gl_header=OpenGL/gl.h
      AC_DEFINE([HAVE_OPENGL_GL_H], 1, [define if the GL header should be included as OpenGL/gl.h])
    ])
  ])

  CPPFLAGS="$sim_ac_gl_save_CPPFLAGS"
  if $sim_ac_gl_header_avail; then
    if test x"$sim_ac_gl_cppflags" = x""; then
      AC_MSG_RESULT([@%:@include <$sim_ac_gl_header>])
    else
      AC_MSG_RESULT([$sim_ac_gl_cppflags, @%:@include <$sim_ac_gl_header>])
    fi
    $1
  else
    AC_MSG_RESULT([not found])
    $2
  fi
else
  AC_MSG_RESULT([disabled])
  $2
fi
])# SIM_AC_CHECK_HEADER_GL

# **************************************************************************
# SIM_AC_CHECK_HEADER_GLU([IF-FOUND], [IF-NOT-FOUND])
#
# This macro detects how to include the GLU header file, and gives you the
# necessary CPPFLAGS in $sim_ac_glu_cppflags, and also sets the config.h
# defines HAVE_GL_GLU_H or HAVE_OPENGL_GLU_H if one of them is found.

AC_DEFUN([SIM_AC_CHECK_HEADER_GLU],
[sim_ac_glu_header_avail=false
AC_MSG_CHECKING([how to include glu.h])
if test x"$with_opengl" != x"no"; then
  sim_ac_glu_save_CPPFLAGS=$CPPFLAGS
  sim_ac_glu_cppflags=

  if test x"$with_opengl" != xyes && test x"$with_opengl" != x""; then
    sim_ac_glu_cppflags="-I${with_opengl}/include"
  else
    # On HP-UX platforms, OpenGL headers and libraries are usually installed
    # at this location.
    sim_ac_gl_hpux=/opt/graphics/OpenGL
    if test -d $sim_ac_gl_hpux; then
      sim_ac_glu_cppflags=-I$sim_ac_gl_hpux/include
    fi
  fi

  CPPFLAGS="$CPPFLAGS $sim_ac_glu_cppflags"

  SIM_AC_CHECK_HEADER_SILENT([GL/glu.h], [
    sim_ac_glu_header_avail=true
    sim_ac_glu_header=GL/glu.h
    AC_DEFINE([HAVE_GL_GLU_H], 1, [define if the GLU header should be included as GL/glu.h])
  ], [
    SIM_AC_CHECK_HEADER_SILENT([OpenGL/glu.h], [
      sim_ac_glu_header_avail=true
      sim_ac_glu_header=OpenGL/glu.h
      AC_DEFINE([HAVE_OPENGL_GLU_H], 1, [define if the GLU header should be included as OpenGL/glu.h])
    ])
  ])

  CPPFLAGS="$sim_ac_glu_save_CPPFLAGS"
  if $sim_ac_glu_header_avail; then
    if test x"$sim_ac_glu_cppflags" = x""; then
      AC_MSG_RESULT([@%:@include <$sim_ac_glu_header>])
    else
      AC_MSG_RESULT([$sim_ac_glu_cppflags, @%:@include <$sim_ac_glu_header>])
    fi
    $1
  else
    AC_MSG_RESULT([not found])
    $2
  fi
else
  AC_MSG_RESULT([disabled])
  $2
fi
])# SIM_AC_CHECK_HEADER_GLU

# **************************************************************************
# SIM_AC_CHECK_HEADER_GLEXT([IF-FOUND], [IF-NOT-FOUND])
#
# This macro detects how to include the GLEXT header file, and gives you the
# necessary CPPFLAGS in $sim_ac_glext_cppflags, and also sets the config.h
# defines HAVE_GL_GLEXT_H or HAVE_OPENGL_GLEXT_H if one of them is found.

AC_DEFUN([SIM_AC_CHECK_HEADER_GLEXT],
[sim_ac_glext_header_avail=false
AC_MSG_CHECKING([how to include glext.h])
if test x"$with_opengl" != x"no"; then
  sim_ac_glext_save_CPPFLAGS=$CPPFLAGS
  sim_ac_glext_cppflags=

  if test x"$with_opengl" != xyes && test x"$with_opengl" != x""; then
    sim_ac_glext_cppflags="-I${with_opengl}/include"
  else
    # On HP-UX platforms, OpenGL headers and libraries are usually installed
    # at this location.
    sim_ac_gl_hpux=/opt/graphics/OpenGL
    if test -d $sim_ac_gl_hpux; then
      sim_ac_glext_cppflags=-I$sim_ac_gl_hpux/include
    fi
  fi

  CPPFLAGS="$CPPFLAGS $sim_ac_glext_cppflags"

  SIM_AC_CHECK_HEADER_SILENT([GL/glext.h], [
    sim_ac_glext_header_avail=true
    sim_ac_glext_header=GL/glext.h
    AC_DEFINE([HAVE_GL_GLEXT_H], 1, [define if the GLEXT header should be included as GL/glext.h])
  ], [
    SIM_AC_CHECK_HEADER_SILENT([OpenGL/gl.h], [
      sim_ac_glext_header_avail=true
      sim_ac_glext_header=OpenGL/glext.h
      AC_DEFINE([HAVE_OPENGL_GLEXT_H], 1, [define if the GLEXT header should be included as OpenGL/glext.h])
    ])
  ])

  CPPFLAGS="$sim_ac_glext_save_CPPFLAGS"
  if $sim_ac_glext_header_avail; then
    if test x"$sim_ac_glext_cppflags" = x""; then
      AC_MSG_RESULT([@%:@include <$sim_ac_glext_header>])
    else
      AC_MSG_RESULT([$sim_ac_glext_cppflags, @%:@include <$sim_ac_glext_header>])
    fi
    $1
  else
    AC_MSG_RESULT([not found])
    $2
  fi
else
  AC_MSG_RESULT([disabled])
  $2
fi
])# SIM_AC_CHECK_HEADER_GLEXT


# **************************************************************************
# SIM_AC_CHECK_OPENGL([IF-FOUND], [IF-NOT-FOUND])
#
# This macro detects whether or not it's possible to link against OpenGL
# (or Mesa), and gives you the necessary modifications to the
# pre-processor, compiler and linker environment in the envvars
#
#                $sim_ac_ogl_cppflags
#                $sim_ac_ogl_ldflags
#                $sim_ac_ogl_libs
#
# The necessary extra options are also automatically added to CPPFLAGS,
# LDFLAGS and LIBS.
#
# Authors: <larsa@sim.no>, <mortene@sim.no>.

AC_DEFUN(SIM_AC_CHECK_OPENGL, [

sim_ac_ogl_cppflags=
sim_ac_ogl_ldflags=
sim_ac_ogl_libs=

AC_ARG_WITH(
  [mesa],
  AC_HELP_STRING([--with-mesa],
                 [prefer MesaGL (if found) over OpenGL [[default=yes]]]),
  [],
  [with_mesa=yes])


sim_ac_ogl_glnames="-lGL -lopengl32"
sim_ac_ogl_mesaglnames=-lMesaGL

if test "x$with_mesa" = "xyes"; then
  sim_ac_ogl_first=$sim_ac_ogl_mesaglnames
  sim_ac_ogl_second=$sim_ac_ogl_glnames
else
  sim_ac_ogl_first=$sim_ac_ogl_glnames
  sim_ac_ogl_second=$sim_ac_ogl_mesaglnames
fi

AC_ARG_WITH(
  [opengl],
  AC_HELP_STRING([--with-opengl=DIR],
                 [OpenGL/Mesa installation directory]),
  [],
  [with_opengl=yes])

if test x"$with_opengl" != xno; then

  if test x"$with_opengl" != xyes && test x"$with_opengl" != x""; then
    sim_ac_ogl_ldflags=-L$with_opengl/lib
    # $sim_ac_ogl_cppflags is set up in the SIM_AC_CHECK_HEADER_GL
    # invocation further below.
  else
    # On HP-UX platforms, OpenGL headers and libraries are usually installed
    # at this location.
    sim_ac_gl_hpux=/opt/graphics/OpenGL
    if test -d $sim_ac_gl_hpux; then
      sim_ac_ogl_ldflags=-L$sim_ac_gl_hpux/lib
    fi
  fi

  sim_ac_use_framework_option=false;
  case $host_os in
  darwin*)
    if test x"$GCC" = x"yes"; then
      SIM_AC_CC_COMPILER_OPTION([-framework OpenGL], [sim_ac_use_framework_option=true])
    fi
    ;;
  esac

  if $sim_ac_use_framework_option; then
    # hopefully, this is the default behavior and not needed. 20011005 larsa
    # sim_ac_ogl_cppflags="-F/System/Library/Frameworks/OpenGL.framework/"
    sim_ac_ogl_ldflags="-Wl,-framework,OpenGL"
  fi

  sim_ac_save_cppflags=$CPPFLAGS
  sim_ac_save_ldflags=$LDFLAGS
  sim_ac_save_libs=$LIBS

  CPPFLAGS="$CPPFLAGS $sim_ac_ogl_cppflags"
  LDFLAGS="$LDFLAGS $sim_ac_ogl_ldflags"

  SIM_AC_CHECK_HEADER_GL([CPPFLAGS="$CPPFLAGS $sim_ac_gl_cppflags"],
                         [AC_MSG_WARN([could not find gl.h])])

  sim_ac_glchk_hit=false
  for sim_ac_tmp_outerloop in barebones withpthreads; do
    if ! $sim_ac_glchk_hit; then

      sim_ac_oglchk_pthreadslib=""
      if test "$sim_ac_tmp_outerloop" = "withpthreads"; then
        AC_MSG_WARN([couldn't compile or link with OpenGL library -- trying with pthread library in place...])
        LIBS="$sim_ac_save_libs"
        SIM_AC_CHECK_PTHREAD([
          sim_ac_ogl_cppflags="$sim_ac_ogl_cppflags $sim_ac_pthread_cppflags"
          sim_ac_ogl_ldflags="$sim_ac_ogl_ldflags $sim_ac_pthread_ldflags"
          sim_ac_oglchk_pthreadslib="$sim_ac_pthread_libs"
          ],
          [AC_MSG_WARN([couldn't compile or link with pthread library])
          ])
      fi

      AC_MSG_CHECKING([for OpenGL library dev-kit])
      # Mac OS X uses nada (only LDFLAGS), which is why "" was set first
      for sim_ac_ogl_libcheck in "" $sim_ac_ogl_first $sim_ac_ogl_second; do
        if ! $sim_ac_glchk_hit; then
          LIBS="$sim_ac_ogl_libcheck $sim_ac_oglchk_pthreadslib $sim_ac_save_libs"
          AC_TRY_LINK(
            [#ifdef HAVE_WINDOWS_H
             #include <windows.h>
             #endif
             #ifdef HAVE_GL_GL_H
             #include <GL/gl.h>
             #endif
             #ifdef HAVE_OPENGL_GL_H
             /* Mac OS X */
             #include <OpenGL/gl.h>
             #endif
            ],
            [glPointSize(1.0f);],
            [
             sim_ac_glchk_hit=true
             sim_ac_ogl_libs="$sim_ac_ogl_libcheck $sim_ac_oglchk_pthreadslib"
            ]
          )
        fi
      done
      if $sim_ac_glchk_hit; then
        AC_MSG_RESULT($sim_ac_ogl_cppflags $sim_ac_ogl_ldflags $sim_ac_ogl_libs)
      else
        AC_MSG_RESULT([unresolved])
      fi
    fi
  done

  if $sim_ac_glchk_hit; then
    LIBS="$sim_ac_ogl_libs $sim_ac_save_libs"
    $1
  else
    CPPFLAGS="$sim_ac_save_cppflags"
    LDFLAGS="$sim_ac_save_ldflags"
    LIBS="$sim_ac_save_libs"
    $2
  fi
fi
])


# **************************************************************************
# SIM_AC_GLU_READY_IFELSE( [ACTION-IF-TRUE], [ACTION-IF-FALSE] )

AC_DEFUN([SIM_AC_GLU_READY_IFELSE], [
sim_ac_glu_save_CPPFLAGS=$CPPFLAGS
SIM_AC_CHECK_HEADER_GLU(, [AC_MSG_WARN([could not find glu.h])])
if test x"$sim_ac_gl_cppflags" != x"$sim_ac_glu_cppflags"; then
  CPPFLAGS="$CPPFLAGS $sim_ac_gl_cppflags $sim_ac_glu_cppflags"
fi
AC_CACHE_CHECK(
  [if GLU is available as part of GL library],
  [sim_cv_glu_ready],
  [AC_TRY_LINK(
    [
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif /* HAVE_WINDOWS_H */
#ifdef HAVE_GL_GL_H
#include <GL/gl.h>
#else
#ifdef HAVE_OPENGL_GL_H
#include <OpenGL/gl.h>
#endif
#endif
#ifdef HAVE_GL_GLU_H
#include <GL/glu.h>
#else
#ifdef HAVE_OPENGL_GLU_H
#include <OpenGL/glu.h>
#endif
#endif
],
    [
gluSphere(0L, 1.0, 1, 1);
/* Defect JAGad01283 of HP's aCC compiler causes a link failure unless
   there is at least one "pure" OpenGL call along with GLU calls. */
glEnd();
],
    [sim_cv_glu_ready=true],
    [sim_cv_glu_ready=false])])

if $sim_cv_glu_ready; then
  ifelse([$1], , :, [$1])
else
  ifelse([$2], , :, [$2])
fi
]) # SIM_AC_GLU_READY_IFELSE()

# Usage:
#  SIM_AC_CHECK_GLU([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
#  Try to use the OpenGL utility library; GLU. If it is found,
#  these shell variables are set:
#
#    $sim_ac_glu_cppflags (extra flags the compiler needs for GLU)
#    $sim_ac_glu_ldflags  (extra flags the linker needs for GLU)
#    $sim_ac_glu_libs     (link libraries the linker needs for GLU)
#
#  The CPPFLAGS, LDFLAGS and LIBS flags will also be modified accordingly.
#  In addition, the variable $sim_ac_glu_avail is set to "yes" if GLU
#  is found.
#
#
# Author: Morten Eriksen, <mortene@sim.no>.

AC_DEFUN([SIM_AC_CHECK_GLU], [
sim_ac_glu_save_CPPFLAGS=$CPPFLAGS
SIM_AC_CHECK_HEADER_GLU(, [AC_MSG_WARN([could not find glu.h])])
if test x"$sim_ac_gl_cppflags" != x"$sim_ac_glu_cppflags"; then
  CPPFLAGS="$CPPFLAGS $sim_ac_gl_cppflags $sim_ac_glu_cppflags"
fi
sim_ac_glu_avail=no

# It's usually libGLU.so on UNIX systems and glu32.lib on MSWindows.
sim_ac_glu_names="-lGLU -lglu32"
sim_ac_glu_mesanames=-lMesaGLU

# with_mesa is set from the SIM_AC_CHECK_OPENGL macro.
if test "x$with_mesa" = "xyes"; then
  sim_ac_glu_first=$sim_ac_glu_mesanames
  sim_ac_glu_second=$sim_ac_glu_names
else
  sim_ac_glu_first=$sim_ac_glu_names
  sim_ac_glu_second=$sim_ac_glu_mesanames
fi

AC_ARG_WITH(
  [glu],
  AC_HELP_STRING([--with-glu=DIR],
                 [use the OpenGL utility library [[default=yes]]]),
  [],
  [with_glu=yes])

if test x"$with_glu" != xno; then
  if test x"$with_glu" != xyes; then
    # sim_ac_glu_cppflags="-I${with_glu}/include"
    sim_ac_glu_ldflags="-L${with_glu}/lib"
  fi

  sim_ac_save_cppflags=$CPPFLAGS
  sim_ac_save_ldflags=$LDFLAGS
  sim_ac_save_libs=$LIBS

  CPPFLAGS="$CPPFLAGS $sim_ac_glu_cppflags"
  LDFLAGS="$LDFLAGS $sim_ac_glu_ldflags"

  AC_CACHE_CHECK(
    [whether GLU is available],
    sim_cv_lib_glu,
    [sim_cv_lib_glu=UNRESOLVED

    # Some platforms (like BeOS) have the GLU functionality in the GL
    # library (and no GLU library present).
    for sim_ac_glu_libcheck in "" $sim_ac_glu_first $sim_ac_glu_second; do
      if test "x$sim_cv_lib_glu" = "xUNRESOLVED"; then
        LIBS="$sim_ac_glu_libcheck $sim_ac_save_libs"
        AC_TRY_LINK([
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif /* HAVE_WINDOWS_H */
#ifdef HAVE_GL_GL_H
#include <GL/gl.h>
#else
#ifdef HAVE_OPENGL_GL_H
#include <OpenGL/gl.h>
#endif
#endif
#ifdef HAVE_GL_GLU_H
#include <GL/glu.h>
#else
#ifdef HAVE_OPENGL_GLU_H
#include <OpenGL/glu.h>
#endif
#endif
],
                    [
gluSphere(0L, 1.0, 1, 1);
/* Defect JAGad01283 of HP's aCC compiler causes a link failure unless
   there is at least one "pure" OpenGL call along with GLU calls. */
glEnd();
],
                    [sim_cv_lib_glu="$sim_ac_glu_libcheck"])
      fi
    done
    if test x"$sim_cv_lib_glu" = x"" &&
       test x`echo $LDFLAGS | grep -c -- "-Wl,-framework,OpenGL"` = x1; then
      # just for the visual representation on Mac OS X
      sim_cv_lib_glu="-Wl,-framework,OpenGL"
    fi
  ])

  LIBS="$sim_ac_save_libs"

  CPPFLAGS=$sim_ac_glu_save_CPPFLAGS
  if test "x$sim_cv_lib_glu" != "xUNRESOLVED"; then
    if test x"$sim_cv_lib_glu" = x"-Wl,-framework,OpenGL"; then
      sim_ac_glu_libs=""
    else
      sim_ac_glu_libs="$sim_cv_lib_glu"
    fi
    LIBS="$sim_ac_glu_libs $sim_ac_save_libs"
    sim_ac_glu_avail=yes
    $1
  else
    CPPFLAGS=$sim_ac_save_cppflags
    LDFLAGS=$sim_ac_save_ldflags
    LIBS=$sim_ac_save_libs
    $2
  fi
fi
])


# Usage:
#  SIM_AC_GLU_NURBSOBJECT([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
#  Try to find out whether the interface struct against the GLU
#  library NURBS functions is called "GLUnurbs" or "GLUnurbsObj".
#  (This seems to have changed somewhere between release 1.1 and
#  release 1.3 of GLU).
#
#  The variable $sim_ac_glu_nurbsobject is set to the correct name
#  if the nurbs structure is found.
#
# Author: Morten Eriksen, <mortene@sim.no>.

AC_DEFUN(SIM_AC_GLU_NURBSOBJECT, [
AC_CACHE_CHECK(
  [what structure to use in the GLU NURBS interface],
  sim_cv_func_glu_nurbsobject,
  [sim_cv_func_glu_nurbsobject=NONE
   for sim_ac_glu_structname in GLUnurbs GLUnurbsObj; do
    if test "$sim_cv_func_glu_nurbsobject" = NONE; then
      AC_TRY_LINK([
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif /* HAVE_WINDOWS_H */
#ifdef HAVE_GL_GL_H
#include <GL/gl.h>
#else
#ifdef HAVE_OPENGL_GL_H
#include <OpenGL/gl.h>
#endif
#endif
#ifdef HAVE_GL_GLU_H
#include <GL/glu.h>
#else
#ifdef HAVE_OPENGL_GLU_H
#include <OpenGL/glu.h>
#endif
#endif
],
                  [
$sim_ac_glu_structname * hepp = gluNewNurbsRenderer();
gluDeleteNurbsRenderer(hepp);
/* Defect JAGad01283 of HP's aCC compiler causes a link failure unless
   there is at least one "pure" OpenGL call along with GLU calls. */
glEnd();
],
                  [sim_cv_func_glu_nurbsobject=$sim_ac_glu_structname])
    fi
  done
])

if test $sim_cv_func_glu_nurbsobject = NONE; then
  sim_ac_glu_nurbsobject=
  $2
else
  sim_ac_glu_nurbsobject=$sim_cv_func_glu_nurbsobject
  $1
fi
])

# **************************************************************************
# SIM_AC_HAVE_GLXGETCURRENTDISPLAY_IFELSE( IF-FOUND, IF-NOT-FOUND )
#
# Check whether the OpenGL implementation includes the method
# glXGetCurrentDisplay().

AC_DEFUN([SIM_AC_HAVE_GLXGETCURRENTDISPLAY_IFELSE], [
AC_CACHE_CHECK(
  [whether glXGetCurrentDisplay() is available],
  sim_cv_have_glxgetcurrentdisplay,
  AC_TRY_LINK([
#include <GL/gl.h>
#include <GL/glx.h>
],
[(void)glXGetCurrentDisplay();],
[sim_cv_have_glxgetcurrentdisplay=true],
[sim_cv_have_glxgetcurrentdisplay=false]))

if ${sim_cv_have_glxgetcurrentdisplay}; then
  ifelse([$1], , :, [$1])
else
  ifelse([$2], , :, [$2])
fi
]) # SIM_AC_HAVE_GLXGETCURRENTDISPLAY_IFELSE()

# **************************************************************************
# SIM_AC_HAVE_GLX_IFELSE( IF-FOUND, IF-NOT-FOUND )
#
# Check whether GLX is on the system.

AC_DEFUN([SIM_AC_HAVE_GLX_IFELSE], [
AC_CACHE_CHECK(
  [whether GLX is on the system],
  sim_cv_have_glx,
  AC_TRY_LINK(
    [
#include <GL/glx.h>
#include <GL/gl.h>
],
    [
(void)glXChooseVisual(0L, 0, 0L);
/* Defect JAGad01283 of HP's aCC compiler causes a link failure unless
   there is at least one "pure" OpenGL call along with GLU calls. */
glEnd();
],
    [sim_cv_have_glx=true],
    [sim_cv_have_glx=false]))

if ${sim_cv_have_glx=false}; then
  ifelse([$1], , :, [$1])
else
  ifelse([$2], , :, [$2])
fi
]) # SIM_AC_HAVE_GLX_IFELSE()

# **************************************************************************
# SIM_AC_HAVE_GLXGETPROCADDRESSARB_IFELSE( IF-FOUND, IF-NOT-FOUND )
#
# Check for glXGetProcAddressARB() function.

AC_DEFUN([SIM_AC_HAVE_GLXGETPROCADDRESSARB_IFELSE], [
AC_CACHE_CHECK(
  [for glXGetProcAddressARB() function],
  sim_cv_have_glxgetprocaddressarb,
  AC_TRY_LINK(
    [
#include <GL/glx.h>
#include <GL/gl.h>
],
    [
      glXGetProcAddressARB((const GLubyte *)"glClearColor");
/* Defect JAGad01283 of HP's aCC compiler causes a link failure unless
   there is at least one "pure" OpenGL call along with GLU calls. */
      glEnd();
],
    [sim_cv_have_glxgetprocaddressarb=true],
    [sim_cv_have_glxgetprocaddressarb=false]))

if ${sim_cv_have_glxgetprocaddressarb=false}; then
  ifelse([$1], , :, [$1])
else
  ifelse([$2], , :, [$2])
fi
]) # SIM_AC_HAVE_GLXGETPROCADDRESSARB_IFELSE()


# **************************************************************************
# SIM_AC_HAVE_WGL_IFELSE( IF-FOUND, IF-NOT-FOUND )
#
# Check whether WGL is on the system.
#
# This macro has one important side-effect: the variable
# sim_ac_wgl_libs will be set to the list of libraries
# needed to link with wgl*() functions.

AC_DEFUN([SIM_AC_HAVE_WGL_IFELSE], [
sim_ac_save_libs=$LIBS
sim_ac_wgl_libs="-lgdi32"
LIBS="$LIBS $sim_ac_wgl_libs"

AC_CACHE_CHECK(
  [whether WGL is on the system],
  sim_cv_have_wgl,
  AC_TRY_LINK(
    [
#include <windows.h>
#include <GL/gl.h>
],
    [(void)wglCreateContext(0L);],
    [sim_cv_have_wgl=true],
    [sim_cv_have_wgl=false]))

LIBS=$sim_ac_save_libs
if ${sim_cv_have_wgl=false}; then
  ifelse([$1], , :, [$1])
else
  ifelse([$2], , :, [$2])
fi
]) # SIM_AC_HAVE_WGL_IFELSE()

# **************************************************************************
# SIM_AC_HAVE_AGL_IFELSE( IF-FOUND, IF-NOT-FOUND )
#
# Check whether WGL is on the system.

AC_DEFUN([SIM_AC_HAVE_AGL_IFELSE], [
sim_ac_save_ldflags=$LDFLAGS
sim_ac_agl_ldflags="-Wl,-framework,ApplicationServices -Wl,-framework,AGL"

LDFLAGS="$LDFLAGS $sim_ac_agl_ldflags"

AC_CACHE_CHECK(
  [whether AGL is on the system],
  sim_cv_have_agl,
  AC_TRY_LINK(
    [#include <AGL/agl.h>
#include <Carbon/Carbon.h>],
    [aglGetCurrentContext();],
    [sim_cv_have_agl=true],
    [sim_cv_have_agl=false]))

LDFLAGS=$sim_ac_save_ldflags
if ${sim_cv_have_agl=false}; then
  ifelse([$1], , :, [$1])
else
  ifelse([$2], , :, [$2])
fi
]) # SIM_AC_HAVE_AGL_IFELSE()


# Usage:
#  SIM_AC_CHECK_PTHREAD([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
#  Try to find the PTHREAD development system. If it is found, these
#  shell variables are set:
#
#    $sim_ac_pthread_cppflags (extra flags the compiler needs for pthread)
#    $sim_ac_pthread_ldflags  (extra flags the linker needs for pthread)
#    $sim_ac_pthread_libs     (link libraries the linker needs for pthread)
#
#  The CPPFLAGS, LDFLAGS and LIBS flags will also be modified accordingly.
#  In addition, the variable $sim_ac_pthread_avail is set to "true" if the
#  pthread development system is found.
#
#
# Author: Morten Eriksen, <mortene@sim.no>.

AC_DEFUN([SIM_AC_CHECK_PTHREAD], [

AC_ARG_WITH(
  [pthread],
  AC_HELP_STRING([--with-pthread=DIR],
                 [pthread installation directory]),
  [],
  [with_pthread=yes])

sim_ac_pthread_avail=no

if test x"$with_pthread" != xno; then
  if test x"$with_pthread" != xyes; then
    sim_ac_pthread_cppflags="-I${with_pthread}/include"
    sim_ac_pthread_ldflags="-L${with_pthread}/lib"
  fi

  # FIXME: should investigate and document the exact meaning of
  # the _REENTRANT flag. larsa's commit message mentions
  # "glibc-doc/FAQ.threads.html".
  #
  # Preferably, it should only be set up when really needed
  # (as detected by some other configure check).
  #
  # 20030306 mortene.
  sim_ac_pthread_cppflags="-D_REENTRANT ${sim_ac_pthread_cppflags}"

  sim_ac_save_cppflags=$CPPFLAGS
  sim_ac_save_ldflags=$LDFLAGS
  sim_ac_save_libs=$LIBS

  CPPFLAGS="$CPPFLAGS $sim_ac_pthread_cppflags"
  LDFLAGS="$LDFLAGS $sim_ac_pthread_ldflags"

  sim_ac_pthread_avail=false

  AC_MSG_CHECKING([for POSIX threads])
  # At least under FreeBSD, we link to pthreads library with -pthread.
  for sim_ac_pthreads_libcheck in "-lpthread" "-pthread"; do
    if ! $sim_ac_pthread_avail; then
      LIBS="$sim_ac_pthreads_libcheck $sim_ac_save_libs"
      AC_TRY_LINK([#include <pthread.h>],
                  [(void)pthread_create(0L, 0L, 0L, 0L);],
                  [sim_ac_pthread_avail=true
                   sim_ac_pthread_libs="$sim_ac_pthreads_libcheck"
                  ])
    fi
  done

  if $sim_ac_pthread_avail; then
    AC_MSG_RESULT($sim_ac_pthread_cppflags $sim_ac_pthread_ldflags $sim_ac_pthread_libs)
  else
    AC_MSG_RESULT(not available)
  fi

  if $sim_ac_pthread_avail; then
    AC_CACHE_CHECK(
      [the struct timespec resolution],
      sim_cv_lib_pthread_timespec_resolution,
      [AC_TRY_COMPILE([#include <pthread.h>],
                      [struct timespec timeout;
                       timeout.tv_nsec = 0;],
                      [sim_cv_lib_pthread_timespec_resolution=nsecs],
                      [sim_cv_lib_pthread_timespec_resolution=usecs])])
    if test x"$sim_cv_lib_pthread_timespec_resolution" = x"nsecs"; then
      AC_DEFINE([HAVE_PTHREAD_TIMESPEC_NSEC], 1, [define if pthread's struct timespec uses nsecs and not usecs])
    fi
  fi

  if $sim_ac_pthread_avail; then
    ifelse([$1], , :, [$1])
  else
    CPPFLAGS=$sim_ac_save_cppflags
    LDFLAGS=$sim_ac_save_ldflags
    LIBS=$sim_ac_save_libs
    ifelse([$2], , :, [$2])
  fi
fi
]) # SIM_AC_CHECK_PTHREAD

# Usage:
#  SIM_CHECK_OIV_XT([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
#  Try to compile and link against the Xt GUI glue library for
#  the Open Inventor development system. Sets this shell
#  variable:
#
#    $sim_ac_oivxt_libs     (link libraries the linker needs for InventorXt)
#
#  The LIBS variable will also be modified accordingly. In addition,
#  the variable $sim_ac_oivxt_avail is set to "yes" if the Xt glue
#  library for the Open Inventor development system is found.
#
# Authors:
#   Morten Eriksen, <mortene@sim.no>.
#   Lars J. Aas, <larsa@sim.no>.
#

AC_DEFUN([SIM_CHECK_OIV_XT], [
sim_ac_oivxt_avail=no

sim_ac_oivxt_libs="-lInventorXt"
sim_ac_save_libs=$LIBS
LIBS="$sim_ac_oivxt_libs $LIBS"

AC_CACHE_CHECK([for Xt glue library in the Open Inventor developer kit],
  sim_cv_lib_oivxt_avail,
  [AC_TRY_LINK([#include <Inventor/Xt/SoXt.h>],
               [(void)SoXt::init(0L, 0L);],
               [sim_cv_lib_oivxt_avail=yes],
               [sim_cv_lib_oivxt_avail=no])])

if test x"$sim_cv_lib_oivxt_avail" = xyes; then
  sim_ac_oivxt_avail=yes
  $1
else
  LIBS=$sim_ac_save_libs
  $2
fi
])

# **************************************************************************
# SIM_AC_WITH_INVENTOR
# This macro just ensures the --with-inventor option is used.

AC_DEFUN([SIM_AC_WITH_INVENTOR], [
: ${sim_ac_want_inventor=false}
AC_ARG_WITH([inventor],
  AC_HELP_STRING([--with-inventor], [use another Open Inventor than Coin [[default=no]], with InventorXt])
AC_HELP_STRING([--with-inventor=PATH], [specify where Open Inventor and InventorXt resides]),
  [case "$withval" in
  no)  sim_ac_want_inventor=false ;;
  yes) sim_ac_want_inventor=true
       test -n "$OIVHOME" &&
         SIM_AC_DEBACKSLASH(sim_ac_inventor_path, "$OIVHOME") ;;
  *)   sim_ac_want_inventor=true; sim_ac_inventor_path="$withval" ;;
  esac])
]) # SIM_AC_WITH_INVENTOR

# **************************************************************************
# SIM_AC_HAVE_INVENTOR_IMAGE_IFELSE

AC_DEFUN([SIM_AC_HAVE_INVENTOR_IMAGE_IFELSE], [
AC_REQUIRE([SIM_AC_WITH_INVENTOR])

if $sim_ac_want_inventor; then
  sim_ac_inventor_image_save_CPPFLAGS="$CPPFLAGS"
  sim_ac_inventor_image_save_LDFLAGS="$LDFLAGS"
  sim_ac_inventor_image_save_LIBS="$LIBS"

  if test s${sim_ac_inventor_path+et} = set; then
    sim_ac_inventor_image_cppflags="-I${sim_ac_inventor_path}/include"
    sim_ac_inventor_image_ldflags="-L${sim_ac_inventor_path}/lib"
  fi
  sim_ac_inventor_image_libs="-limage"

  AC_CACHE_CHECK(
    [if linking with libimage is possible],
    sim_cv_have_inventor_image,
    [
    CPPFLAGS="$sim_ac_inventor_image_cppflags $CPPFLAGS"
    LDFLAGS="$sim_ac_inventor_image_ldflags $LDFLAGS"
    LIBS="$sim_ac_inventor_image_libs $LIBS"
    AC_TRY_LINK(
      [],
      [],
      [sim_cv_have_inventor_image=true],
      [sim_cv_have_inventor_image=false])
    CPPFLAGS="$sim_ac_inventor_image_save_CPPFLAGS"
    LDFLAGS="$sim_ac_inventor_image_save_LDFLAGS"
    LIBS="$sim_ac_inventor_image_save_LIBS"
    ])

  if $sim_cv_have_inventor_image; then
    ifelse([$1], , :, [$1])
  else
    ifelse([$2], , :, [$2])
  fi
else
  ifelse([$2], , :, [$2])
fi
]) # SIM_AC_HAVE_INVENTOR_IMAGE_IFELSE

# **************************************************************************
# SIM_AC_HAVE_INVENTOR_IFELSE( [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND] ] )
#
# Defines $sim_ac_inventor_cppflags, $sim_ac_inventor_ldflags and
# $sim_ac_inventor_libs.

AC_DEFUN([SIM_AC_HAVE_INVENTOR_IFELSE], [
AC_REQUIRE([SIM_AC_WITH_INVENTOR])

if $sim_ac_want_inventor; then
  sim_ac_save_CPPFLAGS="$CPPFLAGS"
  sim_ac_save_LDFLAGS="$LDFLAGS"
  sim_ac_save_LIBS="$LIBS"

  SIM_AC_HAVE_INVENTOR_IMAGE_IFELSE([
    sim_ac_inventor_cppflags="$sim_ac_inventor_image_cppflags"
    sim_ac_inventor_ldflags="$sim_ac_inventor_image_ldflags"
  ], [
    if test s${sim_ac_inventor_path+et} = set; then
      sim_ac_inventor_cppflags="-I${sim_ac_inventor_path}/include"
      sim_ac_inventor_ldflags="-L${sim_ac_inventor_path}/lib"
    fi
    sim_ac_inventor_image_libs=
  ])

  # Let's at least test for "libInventor".
  sim_ac_inventor_chk_libs="-lInventor"
  sim_ac_inventor_libs=UNRESOLVED

  CPPFLAGS="$sim_ac_inventor_cppflags $CPPFLAGS"

  AC_CHECK_HEADER([Inventor/SbBasic.h],
                  [sim_ac_sbbasic=true],
                  [AC_MSG_WARN([header file Inventor/SbBasic.h not found])
                   sim_ac_sbbasic=false])

  if $sim_ac_sbbasic; then
  AC_MSG_CHECKING([the Open Inventor version])
  # See if we can get the TGS_VERSION number for including a
  # check for inv{ver}.lib.
  # TGS did not include TGS_VERSION before 2.6, so this may have to be
  # back-ported to SO_VERSION+SO_REVISION usage.  larsa 2001-07-25
    cat <<EOF > conftest.c
#include <Inventor/SbBasic.h>
#ifdef __COIN__
#error Testing for original Open Inventor, but found Coin...
#endif
PeekInventorVersion: TGS_VERSION
EOF
  if test x"$CPP" = x; then
    AC_MSG_ERROR([cpp not detected - aborting.  notify maintainer at coin-support@coin3d.org.])
  fi
  echo "$CPP $CPPFLAGS conftest.c" >&AS_MESSAGE_LOG_FD
  tgs_version_line=`$CPP $CPPFLAGS conftest.c 2>&AS_MESSAGE_LOG_FD | grep "^PeekInventorVersion"`
  if test x"$tgs_version_line" = x; then
    echo "second try..." >&AS_MESSAGE_LOG_FD
    echo "$CPP -DWIN32 $CPPFLAGS conftest.c" >&AS_MESSAGE_LOG_FD
    tgs_version_line=`$CPP -DWIN32 $CPPFLAGS conftest.c 2>&AS_MESSAGE_LOG_FD | grep "^PeekInventorVersion"`
  fi
  rm -f conftest.c
  tgs_version=`echo $tgs_version_line | cut -c22-24`
  tgs_suffix=
  if test x"${enable_inventor_debug+set}" = xset &&
     test x"${enable_inventor_debug}" = xyes; then
    tgs_suffix=d
  fi
  if test x"$tgs_version" != xTGS; then
    sim_ac_inventor_chk_libs="$sim_ac_inventor_chk_libs -linv$tgs_version$tgs_suffix"
    tgs_version_string=`echo $tgs_version | sed 's/\(.\)\(.\)\(.\)/\1.\2.\3/g'`
    AC_MSG_RESULT([TGS Open Inventor v$tgs_version_string])
  else
    AC_MSG_RESULT([probably SGI or older TGS Open Inventor])
  fi

  AC_MSG_CHECKING([for Open Inventor library])

  for sim_ac_iv_cppflags_loop in "" "-DWIN32"; do
    # Trying with no libraries first, as TGS Inventor uses pragmas in
    # a header file to notify MSVC of what to link with.
    for sim_ac_iv_libcheck in "" $sim_ac_inventor_chk_libs; do
      if test "x$sim_ac_inventor_libs" = "xUNRESOLVED"; then
        CPPFLAGS="$sim_ac_iv_cppflags_loop $sim_ac_inventor_cppflags $sim_ac_save_CPPFLAGS"
        LDFLAGS="$sim_ac_inventor_ldflags $sim_ac_save_LDFLAGS"
        LIBS="$sim_ac_iv_libcheck $sim_ac_inventor_image_libs $sim_ac_save_LIBS"
        AC_TRY_LINK([#include <Inventor/SoDB.h>],
                    [SoDB::init();],
                    [sim_ac_inventor_libs="$sim_ac_iv_libcheck $sim_ac_inventor_image_libs"
                     sim_ac_inventor_cppflags="$sim_ac_iv_cppflags_loop $sim_ac_inventor_cppflags"])
      fi
    done
  done

  if test "x$sim_ac_inventor_libs" != "xUNRESOLVED"; then
    AC_MSG_RESULT($sim_ac_inventor_cppflags $sim_ac_inventor_ldflags $sim_ac_inventor_libs)
  else
    AC_MSG_RESULT([unavailable])
  fi

  fi # sim_ac_sbbasic = TRUE

  CPPFLAGS="$sim_ac_save_CPPFLAGS"
  LDFLAGS="$sim_ac_save_LDFLAGS"
  LIBS="$sim_ac_save_LIBS"

  if test "x$sim_ac_inventor_libs" != "xUNRESOLVED"; then
    :
    $1
  else
    :
    $2
  fi
else
  ifelse([$2], , :, [$2])
fi
]) # SIM_AC_HAVE_INVENTOR_IFELSE

# **************************************************************************

# utility macros:
AC_DEFUN([AC_TOUPPER], [translit([$1], [[a-z]], [[A-Z]])])
AC_DEFUN([AC_TOLOWER], [translit([$1], [[A-Z]], [[a-z]])])

# **************************************************************************
# SIM_AC_HAVE_INVENTOR_NODE( NODE, [ACTION-IF-FOUND] [, ACTION-IF-NOT-FOUND])
#
# Check whether or not the given NODE is available in the Open Inventor
# development system.  If so, the HAVE_<NODE> define is set.
#
# Authors:
#   Lars J. Aas  <larsa@sim.no>
#   Morten Eriksen  <mortene@sim.no>

AC_DEFUN([SIM_AC_HAVE_INVENTOR_NODE], 
[m4_do([pushdef([cache_variable], sim_cv_have_oiv_[]AC_TOLOWER([$1])_node)],
       [pushdef([DEFINE_VARIABLE], HAVE_[]AC_TOUPPER([$1]))])
AC_CACHE_CHECK(
  [if the Open Inventor $1 node is available],
  cache_variable,
  [AC_TRY_LINK(
    [#include <Inventor/nodes/$1.h>],
    [$1 * p = new $1;],
    cache_variable=true,
    cache_variable=false)])

if $cache_variable; then
  AC_DEFINE(DEFINE_VARIABLE, 1, [Define to enable use of the Open Inventor $1 node])
  $2
else
  ifelse([$3], , :, [$3])
fi
m4_do([popdef([cache_variable])],
      [popdef([DEFINE_VARIABLE])])
]) # SIM_AC_HAVE_INVENTOR_NODE

# **************************************************************************
# SIM_AC_HAVE_INVENTOR_VRMLNODE( VRLMNODE, [ACTION-IF-FOUND] [, ACTION-IF-NOT-FOUND])
#
# Check whether or not the given VRMLNODE is available in the Open Inventor
# development system.  If so, the HAVE_<VRMLNODE> define is set.
#
# Authors:
#   Lars J. Aas  <larsa@sim.no>
#   Morten Eriksen  <mortene@sim.no>

AC_DEFUN([SIM_AC_HAVE_INVENTOR_VRMLNODE], 
[m4_do([pushdef([cache_variable], sim_cv_have_oiv_[]AC_TOLOWER([$1])_vrmlnode)],
       [pushdef([DEFINE_VARIABLE], HAVE_[]AC_TOUPPER([$1]))])
AC_CACHE_CHECK(
  [if the Open Inventor $1 VRML node is available],
  cache_variable,
  [AC_TRY_LINK(
    [#include <Inventor/VRMLnodes/$1.h>],
    [$1 * p = new $1;],
    cache_variable=true,
    cache_variable=false)])

if $cache_variable; then
  AC_DEFINE(DEFINE_VARIABLE, 1, [Define to enable use of the Open Inventor $1 VRML node])
  $2
else
  ifelse([$3], , :, [$3])
fi
m4_do([popdef([cache_variable])],
      [popdef([DEFINE_VARIABLE])])
]) # SIM_AC_HAVE_INVENTOR_VRMLNODE

# **************************************************************************
# SIM_AC_HAVE_INVENTOR_FEATURE(MESSAGE, HEADERS, BODY, DEFINE
#                              [, ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])
#
# Authors:
#   Morten Eriksen <mortene@sim.no>

AC_DEFUN([SIM_AC_HAVE_INVENTOR_FEATURE],
[m4_do([pushdef([cache_variable], sim_cv_have_oiv_[]AC_TOLOWER([$4]))],
       [pushdef([DEFINE_VARIABLE], AC_TOUPPER([$4]))])
AC_CACHE_CHECK(
  [$1],
  cache_variable,
  [AC_TRY_LINK(
    [$2],
    [$3],
    cache_variable=true,
    cache_variable=false)])

if $cache_variable; then
  AC_DEFINE(DEFINE_VARIABLE, 1, [Define to enable use of Inventor feature])
  $5
else
  ifelse([$6], , :, [$6])
fi
m4_do([popdef([cache_variable])],
      [popdef([DEFINE_VARIABLE])])
]) # SIM_AC_HAVE_INVENTOR_FEATURE

# **************************************************************************
# SIM_AC_INVENTOR_EXTENSIONS( ACTION )
#
# This macro adds an "--with-iv-extensions=..." option to configure, that
# enabes the configurer to enable extensions in third-party libraries to
# be initialized by the library by default.  The configure-option argument
# must be a comma-separated list of link library path options, link library
# options and class-names.
#
# Sample usage is
#   ./configure --with-iv-extension=-L/tmp/mynodes,-lmynodes,MyNode1,MyNode2
#
# TODO:
#   * check if __declspec(dllimport) is needed on Cygwin

AC_DEFUN([SIM_AC_INVENTOR_EXTENSIONS],
[
AC_ARG_WITH(
  [iv-extensions],
  [AC_HELP_STRING([--with-iv-extensions=extensions], [enable extra open inventor extensions])],
  [sim_ac_iv_try_extensions=$withval])

sim_ac_iv_extension_save_LIBS=$LIBS

sim_ac_iv_extension_LIBS=
sim_ac_iv_extension_LDFLAGS=
sim_ac_iv_extension_decarations=
sim_ac_iv_extension_initializations=

sim_ac_iv_extensions=
while test x"${sim_ac_iv_try_extensions}" != x""; do
  sim_ac_iv_extension=`echo ,$sim_ac_iv_try_extensions | cut -d, -f2`
  sim_ac_iv_try_extensions=`echo ,$sim_ac_iv_try_extensions | cut -d, -f3-`
  case $sim_ac_iv_extension in
  sim_ac_dummy ) # ignore
    ;;
  -L* ) # extension library path hint
    sim_ac_iv_extension_LDFLAGS="$sim_ac_iv_extension_LDFLAGS $sim_ac_iv_extension"
    ;;
  -l* ) # extension library hint
    LIBS="$sim_ac_iv_extension_save_LIBS $sim_ac_iv_extension_LIBS $sim_ac_iv_extension"
    AC_MSG_CHECKING([for Open Inventor extension library $sim_ac_iv_extension])
    AC_TRY_LINK([#include <Inventor/SoDB.h>], [SoDB::init();],
      [sim_ac_iv_extension_LIBS="$sim_ac_iv_extension_LIBS $sim_ac_iv_extension"
       AC_MSG_RESULT([linkable])],
      [AC_MSG_RESULT([unlinkable - discarded])])
    ;;
  * )
    AC_MSG_CHECKING([for Open Inventor extension $sim_ac_iv_extension])
    AC_TRY_LINK(
[#include <Inventor/SoDB.h>
// hack up a declaration and see if the mangled name is found by the linker
class $sim_ac_iv_extension {
public:
static void initClass(void);
};], [
  SoDB::init();
  $sim_ac_iv_extension::initClass();
], [
  AC_MSG_RESULT([found])
  sim_ac_iv_extensions="$sim_ac_iv_extensions COIN_IV_EXTENSION($sim_ac_iv_extension)"
], [
  AC_MSG_RESULT([not found])
])
    ;;
  esac
done

AC_DEFINE_UNQUOTED([COIN_IV_EXTENSIONS], [$sim_ac_iv_extensions], [Open Inventor extensions])

LIBS=$sim_ac_iv_extension_save_LIBS

ifelse([$1], , :, [$1])

]) # SIM_AC_INVENTOR_EXTENSIONS


# Convenience macros SIM_AC_DEBACKSLASH and SIM_AC_DOBACKSLASH for
# converting to and from MSWin/MS-DOS style paths.
#
# Example use:
#
#     SIM_AC_DEBACKSLASH(my_ac_reversed, "C:\\mydir\\bin")
#
# will give a shell variable $my_ac_reversed with the value "C:/mydir/bin").
# Vice versa for SIM_AC_DOBACKSLASH.
#
# Author: Marius Bugge Monsen <mariusbu@sim.no>
#         Lars Jørgen Aas <larsa@sim.no>
#         Morten Eriksen <mortene@sim.no>

AC_DEFUN([SIM_AC_DEBACKSLASH], [
eval "$1=\"`echo $2 | sed -e 's%\\\\%\\/%g'`\""
])

AC_DEFUN([SIM_AC_DOBACKSLASH], [
eval "$1=\"`echo $2 | sed -e 's%\\/%\\\\%g'`\""
])

AC_DEFUN([SIM_AC_DODOUBLEBACKSLASH], [
eval "$1=\"`echo $2 | sed -e 's%\\/%\\\\\\\\\\\\\\\\%g'`\""
])


# Usage:
#   SIM_AC_HAVE_COIN_IFELSE( IF-FOUND, IF-NOT-FOUND )
#
# Description:
#   This macro locates the Coin development system.  If it is found,
#   the set of variables listed below are set up as described and made
#   available to the configure script.
#
#   The $sim_ac_coin_desired variable can be set to false externally to
#   make Coin default to be excluded.
#
# Autoconf Variables:
# > $sim_ac_coin_desired     true | false (defaults to true)
# < $sim_ac_coin_avail       true | false
# < $sim_ac_coin_cppflags    (extra flags the preprocessor needs)
# < $sim_ac_coin_cflags      (extra flags the C compiler needs)
# < $sim_ac_coin_cxxflags    (extra flags the C++ compiler needs)
# < $sim_ac_coin_ldflags     (extra flags the linker needs)
# < $sim_ac_coin_libs        (link library flags the linker needs)
# < $sim_ac_coin_datadir     (location of Coin data files)
# < $sim_ac_coin_includedir  (location of Coin headers)
# < $sim_ac_coin_version     (the libCoin version)
# < $sim_ac_coin_msvcrt      (the MSVC++ C library Coin was built with)
# < $sim_ac_coin_configcmd   (the path to coin-config or "false")
#
# Authors:
#   Lars J. Aas, <larsa@sim.no>
#   Morten Eriksen, <mortene@sim.no>
#
# TODO:
#

AC_DEFUN([SIM_AC_HAVE_COIN_IFELSE], [
AC_PREREQ([2.14a])

# official variables
sim_ac_coin_avail=false
sim_ac_coin_cppflags=
sim_ac_coin_cflags=
sim_ac_coin_cxxflags=
sim_ac_coin_ldflags=
sim_ac_coin_libs=
sim_ac_coin_datadir=
sim_ac_coin_includedir=
sim_ac_coin_version=

# internal variables
: ${sim_ac_coin_desired=true}
sim_ac_coin_extrapath=

AC_ARG_WITH([coin],
AC_HELP_STRING([--with-coin], [enable use of Coin [[default=yes]]])
AC_HELP_STRING([--with-coin=DIR], [give prefix location of Coin]),
  [ case $withval in
    no)  sim_ac_coin_desired=false ;;
    yes) sim_ac_coin_desired=true ;;
    *)   sim_ac_coin_desired=true
         sim_ac_coin_extrapath=$withval ;;
    esac],
  [])

if $sim_ac_coin_desired; then
  sim_ac_path=$PATH
  test -z "$sim_ac_coin_extrapath" || ## search in --with-coin path
    sim_ac_path=$sim_ac_coin_extrapath/bin:$sim_ac_path
  test x"$prefix" = xNONE ||          ## search in --prefix path
    sim_ac_path=$sim_ac_path:$prefix/bin

  AC_PATH_PROG(sim_ac_coin_configcmd, coin-config, false, $sim_ac_path)
  if $sim_ac_coin_configcmd; then
    test -n "$CONFIG" &&
      $sim_ac_coin_configcmd --alternate=$CONFIG >/dev/null 2>/dev/null &&
      sim_ac_coin_configcmd="$sim_ac_coin_configcmd --alternate=$CONFIG"
    sim_ac_coin_version=`$sim_ac_coin_configcmd --version`
    sim_ac_coin_cppflags=`$sim_ac_coin_configcmd --cppflags`
    sim_ac_coin_cflags=`$sim_ac_coin_configcmd --cflags 2>/dev/null`
    sim_ac_coin_cxxflags=`$sim_ac_coin_configcmd --cxxflags`
    sim_ac_coin_ldflags=`$sim_ac_coin_configcmd --ldflags`
    sim_ac_coin_libs=`$sim_ac_coin_configcmd --libs`
    sim_ac_coin_datadir=`$sim_ac_coin_configcmd --datadir`
    # Hide stderr on the following, as ``--includedir'', ``--msvcrt''
    # and ``--cflags'' options were added late to coin-config.
    sim_ac_coin_includedir=`$sim_ac_coin_configcmd --includedir 2>/dev/null`
    sim_ac_coin_msvcrt=`$sim_ac_coin_configcmd --msvcrt 2>/dev/null`
    sim_ac_coin_cflags=`$sim_ac_coin_configcmd --cflags 2>/dev/null`
    AC_CACHE_CHECK(
      [whether libCoin is available],
      sim_cv_coin_avail,
      [sim_ac_save_cppflags=$CPPFLAGS
      sim_ac_save_ldflags=$LDFLAGS
      sim_ac_save_libs=$LIBS
      CPPFLAGS="$CPPFLAGS $sim_ac_coin_cppflags"
      LDFLAGS="$LDFLAGS $sim_ac_coin_ldflags"
      LIBS="$sim_ac_coin_libs $LIBS"
      AC_LANG_PUSH(C++)
      AC_TRY_LINK(
        [#include <Inventor/SoDB.h>],
        [SoDB::init();],
        [sim_cv_coin_avail=true],
        [sim_cv_coin_avail=false])
      AC_LANG_POP
      CPPFLAGS=$sim_ac_save_cppflags
      LDFLAGS=$sim_ac_save_ldflags
      LIBS=$sim_ac_save_libs
    ])
    sim_ac_coin_avail=$sim_cv_coin_avail
  else
    locations=`IFS=:; for p in $sim_ac_path; do echo " -> $p/coin-config"; done`
    AC_MSG_WARN([cannot find 'coin-config' at any of these locations:
$locations])
  fi
fi

if $sim_ac_coin_avail; then
  ifelse([$1], , :, [$1])
else
  ifelse([$2], , :, [$2])
fi
]) # SIM_AC_HAVE_COIN_IFELSE()


# Usage:
#  SIM_AC_CHECK_QT([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
#  Try to find the Qt development system. If it is found, these
#  shell variables are set:
#
#    $sim_ac_qt_cppflags (extra flags the compiler needs for Qt lib)
#    $sim_ac_qt_ldflags  (extra flags the linker needs for Qt lib)
#    $sim_ac_qt_libs     (link libraries the linker needs for Qt lib)
#
#  The CPPFLAGS, LDFLAGS and LIBS flags will also be modified accordingly.
#  In addition, the variable $sim_ac_qt_avail is set to "yes" if
#  the Qt development system is found.
#
# Authors:
#   Morten Eriksen <mortene@sim.no>.
#   Lars J. Aas <larsa@sim.no>.

AC_DEFUN([SIM_AC_CHECK_QT], [

AC_ARG_WITH(
  [qt],
  AC_HELP_STRING([--with-qt=DIR],
                 [specify location of Qt library [default=yes]]),
  [],
  [with_qt=yes])

AC_ARG_ENABLE(
  [qt-debug],
  AC_HELP_STRING([--enable-qt-debug], [win32: link with debug versions of Qt libraries]),
  [case $enableval in
  yes | true ) sim_ac_qt_debug=true ;;
  *) sim_ac_qt_debug=false ;;
  esac],
  [sim_ac_qt_debug=false])

sim_ac_qt_avail=no

if test x"$with_qt" != xno; then
  sim_ac_path=$PATH

  # Remember over config.status re-runs of configure.
  # FIXME: this doesn't work unless QTDIR is part of the configure
  # argument line. We should be able to grab it from the
  # environment settings. 20011018 mortene. 
  AC_SUBST([QTDIR], [$QTDIR])

  # The Cygwin environment needs to invoke moc with a POSIX-style path.
  AC_PATH_PROG(sim_ac_qt_cygpath, cygpath, false)
  if test $sim_ac_qt_cygpath = "false"; then
    sim_ac_qt_dir=$QTDIR
  else
    # Quote $QTDIR in case it's empty.
    sim_ac_qt_dir=`$sim_ac_qt_cygpath -u "$QTDIR"`
  fi

  if test x"$with_qt" != xyes; then
    sim_ac_qt_incflags="-I${with_qt}/include"
    sim_ac_qt_ldflags="-L${with_qt}/lib"
    sim_ac_path=${with_qt}/bin:$PATH
  else
    AC_MSG_CHECKING([value of the QTDIR environment variable])
    if test x"$sim_ac_qt_dir" = x""; then
      AC_MSG_RESULT([empty])
      AC_MSG_WARN([QTDIR environment variable not set -- this might be an indication of a problem])
    else
      AC_MSG_RESULT([$sim_ac_qt_dir])

      # list contents of what's in the qt dev environment into config.log...
      for i in "" bin lib; do
        echo "Listing contents of $sim_ac_qt_dir/$i:" >&5
        ls -l $sim_ac_qt_dir/$i >&5 2>&1
      done

      sim_ac_qt_incflags="-I$sim_ac_qt_dir/include"
      sim_ac_qt_ldflags="-L$sim_ac_qt_dir/lib"
      sim_ac_path=$sim_ac_qt_dir/bin:$PATH
    fi
  fi

  sim_ac_save_cppflags=$CPPFLAGS
  sim_ac_save_ldflags=$LDFLAGS
  sim_ac_save_libs=$LIBS

  CPPFLAGS="$sim_ac_qt_incflags $CPPFLAGS"
  LDFLAGS="$LDFLAGS $sim_ac_qt_ldflags"

  sim_ac_qt_libs=UNRESOLVED

  AC_PATH_PROG(MOC, moc, false, $sim_ac_path)

  if test x"$MOC" = x"false"; then
    AC_MSG_WARN([the ``moc'' Qt pre-processor tool not found])
  else

  sim_ac_qglobal=false
  SIM_AC_CHECK_HEADER_SILENT([qglobal.h],
    [sim_ac_qglobal=true],
    # Debian Linux has the Qt-dev installation headers in a separate subdir.
    [sim_ac_debian_qtheaders=/usr/include/qt
     if test -d $sim_ac_debian_qtheaders; then
       sim_ac_qt_incflags="-I$sim_ac_debian_qtheaders $sim_ac_qt_incflags"
       CPPFLAGS="-I$sim_ac_debian_qtheaders $CPPFLAGS"
       SIM_AC_CHECK_HEADER_SILENT([qglobal.h], [sim_ac_qglobal=true])
     fi])

  if $sim_ac_qglobal; then

  # Find version of the Qt library (MSWindows .dll is named with the
  # version number.)
  AC_MSG_CHECKING([version of Qt library])
  cat > conftest.c << EOF
#include <qglobal.h>
int VerQt = QT_VERSION;
EOF
  # The " *"-parts of the last sed-expression on the next line are necessary
  # because at least the Solaris/CC preprocessor adds extra spaces before and
  # after the trailing semicolon.
  sim_ac_qt_version=`$CXXCPP $CPPFLAGS conftest.c 2>/dev/null | grep '^int VerQt' | sed 's%^int VerQt = %%' | sed 's% *; *$%%'`

  case $sim_ac_qt_version in
  0x* )
    sim_ac_qt_version=`echo $sim_ac_qt_version | sed -e 's/^0x.\(.\).\(.\).\(.\)/\1\2\3/;'`
    ;;
  * )
    # nada
    ;;
  esac

  rm -f conftest.c
  AC_MSG_RESULT($sim_ac_qt_version)

  if test $sim_ac_qt_version -lt 200; then
    SIM_AC_ERROR([too-old-qt])
  fi

  # Too hard to feature-check for the Qt-on-Mac problems, as they involve
  # obscure behavior of the QGLWidget -- so we just resort to do platform
  # and version checking instead.
  case $host_os in
  darwin*)
    if test $sim_ac_qt_version -lt 302; then
      SIM_AC_CONFIGURATION_WARNING([The version of Qt you are using is
known to contain some serious bugs on MacOS X. We strongly recommend you to
upgrade. (See $srcdir/README.MAC for details.)])
    fi

  # Qt/X11 is currently not supported on Mac OS X.
    AC_TRY_LINK([#include <qapplication.h>],
                [#if defined(__APPLE__) && defined(Q_WS_X11)
                 #error blah!
                 #endif],[],
                [SIM_AC_ERROR([x11-qt-on-mac])])
    ;;
  esac

  # Known problems:
  #
  #   * Qt v3.0.1 has a bug where SHIFT-PRESS + CTRL-PRESS + CTRL-RELEASE
  #     results in the last key-event coming out completely wrong under X11.
  #     Known to be fixed in 3.0.3, unknown status in 3.0.2.  <mortene@sim.no>.
  #
  if test $sim_ac_qt_version -lt 303; then
    SIM_AC_CONFIGURATION_WARNING([The version of Qt you are compiling against
is known to contain bugs which influences functionality in SoQt. We strongly
recommend you to upgrade.])
  fi

  sim_ac_qt_cppflags=
  if test x"$MOC" != xfalse; then
    # Do not cache the result, as we might need to play tricks with
    # CPPFLAGS under MSWin.

    # It should be helpful to be able to override the libs-checking with
    # environment variables. Then people won't get completely stuck
    # when the check fails -- we can just take a look at the
    # config.log and give them advice on how to proceed with no updates
    # necessary.
    #
    # (Note also that this makes it possible to select whether to use the
    # mt-safe or the "standard" Qt library if both are installed on the
    # user's system.)
    #
    # mortene.
  
    if test x"$CONFIG_QTLIBS" != x""; then
      AC_MSG_CHECKING([for Qt linking with $CONFIG_QTLIBS])

      for sim_ac_qt_cppflags_loop in "" "-DQT_DLL"; do
        CPPFLAGS="$sim_ac_qt_incflags $sim_ac_qt_cppflags_loop $sim_ac_save_cppflags"
        LIBS="$CONFIG_QTLIBS $sim_ac_save_libs"
        AC_TRY_LINK([#include <qapplication.h>],
                    [qApp = NULL; /* QT_DLL must be defined for assignment to global variables to work */
                     qApp->exit(0);],
                    [sim_ac_qt_libs="$CONFIG_QTLIBS"
                     sim_ac_qt_cppflags="$sim_ac_qt_incflags $sim_ac_qt_cppflags_loop"])
      done

      if test "x$sim_ac_qt_libs" = "xUNRESOLVED"; then
        AC_MSG_RESULT([failed!])
      else
        AC_MSG_RESULT([ok])
      fi

    else
      AC_MSG_CHECKING([for Qt library devkit])

      ## Test all known possible combinations of linking against the
      ## Troll Tech Qt library:
      ##
      ## * "-lqt-gl": links against the standard Debian version of the
      ##   Qt library with embedded QGL
      ##
      ## * "-lqt": should work for most UNIX(-derived) platforms on
      ##   dynamic and static linking with the non-mtsafe library
      ##
      ## * "-lqt-mt": should work for most UNIX(-derived) platforms on
      ##   dynamic and static linking with the mtsafe library
      ##
      ## * "-lqt{version} -lqtmain -lgdi32": w/QT_DLL defined should
      ##   cover dynamic Enterprise Edition linking on Win32 platforms
      ##
      ## * "-lqt -lqtmain -lgdi32": ...unless the {version} suffix is missing,
      ##   which we've had reports about
      ##
      ## * "-lqt-mt{version} -lqtmain -lgdi32": w/QT_DLL defined should
      ##   cover dynamic multi-thread Enterprise Edition linking on Win32
      ##   platforms
      ##
      ## * "-lqt-mt{version}nc -lqtmain -lgdi32": w/QT_DLL defined should
      ##   cover dynamic Non-Commercial Edition linking on Win32 platforms
      ##
      ## * "-lqt -luser32 -lole32 -limm32 -lcomdlg32 -lgdi32": should cover
      ##   static linking on Win32 platforms
      ##
      ## * "-lqt-mt -luser32 -lole32 -limm32 -lcomdlg32 -lgdi32 -lwinspool -lwinmm -ladvapi32":
      ##   added for the benefit of the Qt 3.0.0 Evaluation Version
      ##   (update: "advapi32.lib" seems to be a new dependency for Qt 3.1.0)

      sim_ac_qt_suffix=
      if $sim_ac_qt_debug; then
        sim_ac_qt_suffix=d
      fi

      # Note that we need to always check for -lqt-mt before -lqt, because
      # at least the most recent Debian platforms (as of 2003-02-20) comes
      # with a -lqt which is missing QGL support, while it also has a
      # -lqt-mt *with* QGL support. The reason for this is because the
      # default GL (Mesa) library on Debian is built in mt-safe mode,
      # so a non-mt-safe Qt can't use it.

      for sim_ac_qt_cppflags_loop in "" "-DQT_DLL"; do
        for sim_ac_qt_libcheck in \
            "-lqt-gl" \
            "-lqt-mt" \
            "-lqt" \
            "-lqt-mt -luser32 -lole32 -limm32 -lcomdlg32 -lgdi32 -lwinspool -lwinmm -ladvapi32" \
            "-lqt-mt${sim_ac_qt_version}${sim_ac_qt_suffix} -lqtmain -lgdi32" \
            "-lqt-mt${sim_ac_qt_version}nc${sim_ac_qt_suffix} -lqtmain -lgdi32" \
            "-lqt -lqtmain -lgdi32" \
            "-lqt${sim_ac_qt_version}${sim_ac_qt_suffix} -lqtmain -lgdi32" \
            "-lqt -luser32 -lole32 -limm32 -lcomdlg32 -lgdi32"
        do
          if test "x$sim_ac_qt_libs" = "xUNRESOLVED"; then
            CPPFLAGS="$sim_ac_qt_incflags $sim_ac_qt_cppflags_loop $sim_ac_save_cppflags"
            LIBS="$sim_ac_qt_libcheck $sim_ac_save_libs"
            AC_TRY_LINK([#include <qapplication.h>],
                        [qApp = NULL; /* QT_DLL must be defined for assignment to global variables to work */
                         qApp->exit(0);],
                        [sim_ac_qt_libs="$sim_ac_qt_libcheck"
                         sim_ac_qt_cppflags="$sim_ac_qt_incflags $sim_ac_qt_cppflags_loop"])
          fi
        done
      done

      AC_MSG_RESULT($sim_ac_qt_cppflags $sim_ac_qt_libs)
    fi
  fi

  else # sim_ac_qglobal = false
   AC_MSG_WARN([header file qglobal.h not found, can not compile Qt code])
  fi
  fi # MOC = false

  if test ! x"$sim_ac_qt_libs" = xUNRESOLVED; then
    sim_ac_qt_avail=yes
    CPPFLAGS="$sim_ac_qt_cppflags $sim_ac_save_cppflags"
    LIBS="$sim_ac_qt_libs $sim_ac_save_libs"
    $1
  else
    CPPFLAGS=$sim_ac_save_cppflags
    LDFLAGS=$sim_ac_save_ldflags
    LIBS=$sim_ac_save_libs
    $2
  fi
fi
])

# Usage:
#  SIM_AC_CHECK_QGL([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
#
#  Try to find the QGL widget for interfacing Qt with OpenGL. If it
#  is found, these shell variables are set:
#
#    $sim_ac_qgl_cppflags (extra flags the compiler needs for QGL lib)
#    $sim_ac_qgl_ldflags  (extra flags the linker needs for QGL lib)
#    $sim_ac_qgl_libs     (link libraries the linker needs for QGL lib)
#
#  The LIBS flag will also be modified accordingly. In addition, the
#  variable $sim_ac_qgl_avail is set to "yes" if the QGL extension
#  library is found.
#
# Note that all "modern" variants of Qt should come with QGL embedded.
# There's one important deviation: Debian comes with a -lqt which is
# missing QGL support, while it also has a -lqt-mt *with* QGL support.
# The reason for this is because the default GL (Mesa) library on Debian
# is built in mt-safe mode.
#
# Author: Morten Eriksen, <mortene@sim.no>.

AC_DEFUN([SIM_AC_CHECK_QGL], [
sim_ac_qgl_avail=no
sim_ac_qgl_cppflags=
sim_ac_qgl_ldflags=
sim_ac_qgl_libs=

if test x"$with_qt" != xno; then
  # first check if we can link with the QGL widget already
  AC_CACHE_CHECK(
    [whether the QGL widget is part of main Qt library],
    sim_cv_lib_qgl_integrated,
    [AC_TRY_LINK([#include <qgl.h>],
                 [QGLFormat * f = new QGLFormat; f->setDepth(true); ],
                 [sim_cv_lib_qgl_integrated=yes],
                 [sim_cv_lib_qgl_integrated=no])])

  if test x"$sim_cv_lib_qgl_integrated" = xyes; then
    sim_ac_qgl_avail=yes
    $1
  else
    sim_ac_save_LIBS=$LIBS
    LIBS="$sim_ac_qgl_libs $LIBS"

    AC_MSG_CHECKING([for the QGL extension library])

    sim_ac_qgl_libs=UNRESOLVED
    for sim_ac_qgl_libcheck in "-lqgl" "-lqgl -luser32"; do
      if test "x$sim_ac_qgl_libs" = "xUNRESOLVED"; then
        LIBS="$sim_ac_qgl_libcheck $sim_ac_save_LIBS"
        AC_TRY_LINK([#include <qgl.h>],
                    [QGLFormat * f = new QGLFormat; f->setDepth(true); ],
                    [sim_ac_qgl_libs="$sim_ac_qgl_libcheck"])
      fi
    done

    if test x"$sim_ac_qgl_libs" != xUNRESOLVED; then
      AC_MSG_RESULT($sim_ac_qgl_libs)
      sim_ac_qgl_avail=yes
      $1
    else
      AC_MSG_RESULT([unavailable])
      LIBS=$sim_ac_save_LIBS
      $2
    fi
  fi
fi
])

# SIM_AC_QGLWIDGET_SETAUTOBUFFERSWAP
# ----------------------------------
#
# Use the macro for its side-effect: it defines
#
#       HAVE_QGLWIDGET_SETAUTOBUFFERSWAP
#
# to 1 in config.h if QGLWidget::setAutoBufferSwap() is available.
#
# Author: Morten Eriksen, <mortene@sim.no>.

AC_DEFUN([SIM_AC_QGLWIDGET_SETAUTOBUFFERSWAP], [
AC_CACHE_CHECK(
  [whether the QGLWidget::setAutoBufferSwap() is available],
  sim_cv_func_qglwidget_setautobufferswap,
  [AC_TRY_LINK([#include <qgl.h>
class MyGLWidget : public QGLWidget {
public: MyGLWidget() {setAutoBufferSwap(FALSE);} };],
               [MyGLWidget * w = new MyGLWidget;],
               [sim_cv_func_qglwidget_setautobufferswap=yes],
               [sim_cv_func_qglwidget_setautobufferswap=no])])

if test x"$sim_cv_func_qglwidget_setautobufferswap" = xyes; then
  AC_DEFINE([HAVE_QGLWIDGET_SETAUTOBUFFERSWAP], 1,
    [Define this to 1 if QGLWidget::setAutoBufferSwap() is available])
fi
])


# SIM_AC_QGLFORMAT_SETOVERLAY
# ---------------------------
#
# Use the macro for its side-effect: it defines
#
#       HAVE_QGLFORMAT_SETOVERLAY
#
# to 1 in config.h if QGLFormat::setOverlay() is available.
#
# Author: Morten Eriksen, <mortene@sim.no>.

AC_DEFUN([SIM_AC_QGLFORMAT_SETOVERLAY], [
AC_CACHE_CHECK(
  [whether QGLFormat::setOverlay() is available],
  sim_cv_func_qglformat_setoverlay,
  [AC_TRY_LINK([#include <qgl.h>],
               [QGLFormat f; f.setOverlay(TRUE);],
               [sim_cv_func_qglformat_setoverlay=yes],
               [sim_cv_func_qglformat_setoverlay=no])])

if test x"$sim_cv_func_qglformat_setoverlay" = xyes; then
  AC_DEFINE([HAVE_QGLFORMAT_SETOVERLAY], 1,
    [Define this to 1 if QGLFormat::setOverlay() is available])
fi
])


# SIM_AC_QGLFORMAT_EQ_OP
# ----------------------
#
# Use the macro for its side-effect: it defines
#
#       HAVE_QGLFORMAT_EQ_OP
#
# to 1 in config.h if operator==(QGLFormat&, QGLFormat&) is available.
# (For Qt v2.2.2 at least, Troll Tech forgot to include this method
# in the publicly exported API for MSWindows DLLs.)
#
# Author: Morten Eriksen, <mortene@sim.no>.

AC_DEFUN([SIM_AC_QGLFORMAT_EQ_OP], [
AC_CACHE_CHECK(
  [whether operator==(QGLFormat&,QGLFormat&) is available],
  sim_cv_func_qglformat_eq_op,
  [AC_TRY_LINK([#include <qgl.h>],
               [QGLFormat f; if (f == f) f.setDepth(true);],
               [sim_cv_func_qglformat_eq_op=true],
               [sim_cv_func_qglformat_eq_op=false])])

if $sim_cv_func_qglformat_eq_op; then
  AC_DEFINE([HAVE_QGLFORMAT_EQ_OP], 1,
    [Define this to 1 if operator==(QGLFormat&, QGLFormat&) is available])
fi
])


# SIM_AC_QWIDGET_SHOWFULLSCREEN
# -----------------------------
#
# Use the macro for its side-effect: it defines HAVE_QWIDGET_SHOWFULLSCREEN
# to 1 in config.h if QWidget::showFullScreen() is available (that
# function wasn't introduced in Qt until version 2.1.0).
#
# Author: Morten Eriksen, <mortene@sim.no>.

AC_DEFUN([SIM_AC_QWIDGET_SHOWFULLSCREEN], [
AC_CACHE_CHECK(
  [whether QWidget::showFullScreen() is available],
  sim_cv_def_qwidget_showfullscreen,
  [AC_TRY_LINK([#include <qwidget.h>],
               [QWidget * w = new QWidget(); w->showFullScreen();],
               [sim_cv_def_qwidget_showfullscreen=true],
               [sim_cv_def_qwidget_showfullscreen=false])])

if $sim_cv_def_qwidget_showfullscreen; then
  AC_DEFINE([HAVE_QWIDGET_SHOWFULLSCREEN], 1,
            [Define this if QWidget::showFullScreen() is available])
fi
]) # SIM_AC_QWIDGET_SHOWFULLSCREEN


# SIM_AC_QT_KEYPAD_DEFINE
# -----------------------
#
# Use the macro for its side-effect: it defines HAVE_QT_KEYPAD_DEFINE
# to 1 in config.h if Qt::Keypad is available.
#
# Author: Morten Eriksen, <mortene@sim.no>.

AC_DEFUN([SIM_AC_QT_KEYPAD_DEFINE], [
AC_CACHE_CHECK(
  [whether Qt::Keypad is defined],
  sim_cv_def_qt_keypad,
  [AC_TRY_LINK([#include <qkeycode.h>],
               [Qt::ButtonState s = Qt::Keypad;],
               [sim_cv_def_qt_keypad=true],
               [sim_cv_def_qt_keypad=false])])

if $sim_cv_def_qt_keypad; then
  AC_DEFINE([HAVE_QT_KEYPAD_DEFINE], 1,
            [Define this if Qt::Keypad is available])
fi
]) # SIM_AC_QT_KEYPAD_DEFINE

# Usage:
#   SIM_AC_COMPILE_DEBUG([ACTION-IF-DEBUG[, ACTION-IF-NOT-DEBUG]])
#
# Description:
#   Let the user decide if compilation should be done in "debug mode".
#   If compilation is not done in debug mode, all assert()'s in the code
#   will be disabled.
#
#   Also sets enable_debug variable to either "yes" or "no", so the
#   configure.in writer can add package-specific actions. Default is "yes".
#   This was also extended to enable the developer to set up the two first
#   macro arguments following the well-known ACTION-IF / ACTION-IF-NOT
#   concept.
#
# Authors:
#   Morten Eriksen, <mortene@sim.no>
#   Lars J. Aas, <larsa@sim.no>
#

AC_DEFUN([SIM_AC_COMPILE_DEBUG], [
AC_ARG_ENABLE(
  [debug],
  AC_HELP_STRING([--enable-debug], [compile in debug mode [[default=yes]]]),
  [case "${enableval}" in
    yes) enable_debug=true ;;
    no)  enable_debug=false ;;
    true | false) enable_debug=${enableval} ;;
    *) AC_MSG_ERROR(bad value "${enableval}" for --enable-debug) ;;
  esac],
  [enable_debug=true])

if $enable_debug; then
  DSUFFIX=d
  ifelse([$1], , :, [$1])
else
  DSUFFIX=
  CPPFLAGS="$CPPFLAGS -DNDEBUG"
  ifelse([$2], , :, [$2])
fi
AC_SUBST(DSUFFIX)
])

# Usage:
#   SIM_AC_COMPILER_OPTIMIZATION
#
# Description:
#   Let the user decide if optimization should be attempted turned off
#   by stripping off an "-O[0-9]" option.
# 
#   Note: this macro must be placed after either AC_PROG_CC or AC_PROG_CXX
#   in the configure.in script.
#
# FIXME: this is pretty much just a dirty hack. Unfortunately, this
# seems to be the best we can do without fixing Autoconf to behave
# properly wrt setting optimization options. 20011021 mortene.
# 
# Author: Morten Eriksen, <mortene@sim.no>.
# 

AC_DEFUN([SIM_AC_COMPILER_OPTIMIZATION], [
AC_ARG_ENABLE(
  [optimization],
  AC_HELP_STRING([--enable-optimization],
                 [allow compilers to make optimized code [[default=yes]]]),
  [case "${enableval}" in
    yes) sim_ac_enable_optimization=true ;;
    no)  sim_ac_enable_optimization=false ;;
    *) AC_MSG_ERROR(bad value "${enableval}" for --enable-optimization) ;;
  esac],
  [sim_ac_enable_optimization=true])

if $sim_ac_enable_optimization; then
  :
else
  CFLAGS="`echo $CFLAGS | sed 's/-O[[0-9]]*[[ ]]*//'`"
  CXXFLAGS="`echo $CXXFLAGS | sed 's/-O[[0-9]]*[[ ]]*//'`"
fi
])

# Usage:
#   SIM_AC_CHECK_VAR_FUNCTIONNAME
#
# Side-Effects:
#   config.h:
#     HAVE_VAR___func__              (1 if exists)
#     HAVE_VAR___PRETTY_FUNCTION__   (1 if exists)
#     HAVE_VAR___FUNCTION__          (1 if exists)
#
# (Note that only one of these will be defined.)
#
# Authors:
#   Lars J. Aas <larsa@sim.no>
#   Morten Eriksen <mortene@sim.no>

AC_DEFUN([SIM_AC_CHECK_VAR_FUNCTIONNAME], [
AC_CACHE_CHECK([for function name variable],
  sim_cv_var_functionname, [
  # __func__ is the identifier used by compilers which are
  # compliant with the C99 ISO/IEC 9899:1999 standard.
  AC_TRY_COMPILE(
    [#include <stdio.h>],
    [(void)printf("%s\n",__func__)],
    [sim_cv_var_functionname=__func__],
    [sim_cv_var_functionname=none])
  if test x"$sim_cv_var_functionname" = x"none"; then
    # GCC uses __PRETTY_FUNCTION__
    AC_TRY_COMPILE(
      [#include <stdio.h>],
      [(void)printf("%s\n",__PRETTY_FUNCTION__)],
      [sim_cv_var_functionname=__PRETTY_FUNCTION__],
      [sim_cv_var_functionname=none])
  fi
  if test x"$sim_cv_var_functionname" = x"none"; then
    AC_TRY_COMPILE(
      [#include <stdio.h>],
      [(void)printf("%s\n",__FUNCTION__)],
      [sim_cv_var_functionname=__FUNCTION__],
      [sim_cv_var_functionname=none])
  fi])

# FIXME: these can probably be contracted to a single test inside a loop.
# 20010330 mortene.

if test x"$sim_cv_var_functionname" = x"__func__"; then
  AC_DEFINE([HAVE_VAR___func__], 1,
    [Define this to true if the __func__ variable contains the current function name])
fi

if test x"$sim_cv_var_functionname" = x"__PRETTY_FUNCTION__"; then
  AC_DEFINE([HAVE_VAR___PRETTY_FUNCTION__], 1,
    [Define this to true if the __PRETTY_FUNCTION__ variable contains the current function name])
fi

if test x"$sim_cv_var_functionname" = x"__FUNCTION__"; then
  AC_DEFINE([HAVE_VAR___FUNCTION__], 1,
    [Define this to true if the __FUNCTION__ variable contains the current function name])
fi
])


# Usage:
#   SIM_AC_DEBUGSYMBOLS
#
# Description:
#   Let the user decide if debug symbol information should be compiled
#   in. The compiled libraries/executables will use a lot less space
#   if stripped for their symbol information.
# 
#   Note: this macro must be placed after either AC_PROG_CC or AC_PROG_CXX
#   in the configure.in script.
# 
# Author: Morten Eriksen, <mortene@sim.no>.
# 

AC_DEFUN([SIM_AC_DEBUGSYMBOLS], [
AC_ARG_ENABLE(
  [symbols],
  AC_HELP_STRING([--enable-symbols],
                 [include symbol debug information [[default=yes]]]),
  [case "${enableval}" in
    yes) enable_symbols=yes ;;
    no)  enable_symbols=no ;;
    *) AC_MSG_ERROR(bad value "${enableval}" for --enable-symbols) ;;
  esac],
  [enable_symbols=yes])

# FIXME: don't mangle options like -fno-gnu-linker and -fvolatile-global
# 20020104 larsa
if test x"$enable_symbols" = x"no"; then
  # CPPFLAGS="`echo $CPPFLAGS | sed 's/-g\>//'`"
  CFLAGS="`echo $CFLAGS | sed 's/-g\>//'`"
  CXXFLAGS="`echo $CXXFLAGS | sed 's/-g\>//'`"
fi
]) # SIM_AC_DEBUGSYMBOLS


# Usage:
#   SIM_AC_RTTI_SUPPORT
#
# Description:
#   Let the user decide if RTTI should be compiled in. The compiled
#   libraries/executables will use a lot less space if they don't
#   contain RTTI.
# 
#   Note: this macro must be placed after AC_PROG_CXX in the
#   configure.in script.
# 
# Author: Morten Eriksen, <mortene@sim.no>.

AC_DEFUN([SIM_AC_RTTI_SUPPORT], [
AC_PREREQ([2.13])
AC_ARG_ENABLE(
  [rtti],
  AC_HELP_STRING([--enable-rtti], [(g++ only) compile with RTTI [[default=yes]]]),
  [case "${enableval}" in
    yes) enable_rtti=yes ;;
    no)  enable_rtti=no ;;
    *) AC_MSG_ERROR(bad value "${enableval}" for --enable-rtti) ;;
  esac],
  [enable_rtti=yes])

if test x"$enable_rtti" = x"no"; then
  if test x"$GXX" = x"yes"; then
    CXXFLAGS="$CXXFLAGS -fno-rtti"
  else
    AC_MSG_WARN([--enable-rtti only has effect when using GNU g++])
  fi
fi
])

# Usage:
#   SIM_AC_EXCEPTION_HANDLING
#
# Description:
#   Let the user decide if C++ exception handling should be compiled
#   in. The compiled libraries/executables will use a lot less space
#   if they have exception handling support.
#
#   Note: this macro must be placed after AC_PROG_CXX in the
#   configure.in script.
#
#   Author: Morten Eriksen, <mortene@sim.no>.
#
# TODO:
#   * [mortene:19991114] make this work with compilers other than gcc/g++
#

AC_DEFUN([SIM_AC_EXCEPTION_HANDLING], [
AC_PREREQ([2.13])
AC_ARG_ENABLE(
  [exceptions],
  AC_HELP_STRING([--enable-exceptions],
                 [(g++ only) compile with exceptions [[default=no]]]),
  [case "${enableval}" in
    yes) enable_exceptions=yes ;;
    no)  enable_exceptions=no ;;
    *) AC_MSG_ERROR(bad value "${enableval}" for --enable-exceptions) ;;
  esac],
  [enable_exceptions=no])

if test x"$enable_exceptions" = x"no"; then
  if test "x$GXX" = "xyes"; then
    unset _exception_flag
    dnl This is for GCC >= 2.8
    SIM_AC_CXX_COMPILER_OPTION([-fno-exceptions], [_exception_flag=-fno-exceptions])
    if test x"${_exception_flag+set}" != x"set"; then
      dnl For GCC versions < 2.8
      SIM_AC_CXX_COMPILER_OPTION([-fno-handle-exceptions],
                                 [_exception_flag=-fno-handle-exceptions])
    fi
    if test x"${_exception_flag+set}" != x"set"; then
      AC_MSG_WARN([couldn't find a valid option for avoiding exception handling])
    else
      CXXFLAGS="$CXXFLAGS $_exception_flag"
    fi
  fi
else
  if test x"$GXX" != x"yes"; then
    AC_MSG_WARN([--enable-exceptions only has effect when using GNU g++])
  fi
fi
])


# Usage:
#   SIM_AC_PROFILING_SUPPORT
#
# Description:
#   Let the user decide if profiling code should be compiled
#   in. The compiled libraries/executables will use a lot less space
#   if they don't contain profiling code information, and they will also
#   execute faster.
#
#   Note: this macro must be placed after either AC_PROG_CC or AC_PROG_CXX
#   in the configure.in script.
#
# Author: Morten Eriksen, <mortene@sim.no>.
#
# TODO:
#   * [mortene:19991114] make this work with compilers other than gcc/g++
#

AC_DEFUN([SIM_AC_PROFILING_SUPPORT], [
AC_PREREQ([2.13])
AC_ARG_ENABLE(
  [profile],
  AC_HELP_STRING([--enable-profile],
                 [(GCC only) turn on inclusion of profiling code [[default=no]]]),
  [case "${enableval}" in
    yes) enable_profile=yes ;;
    no)  enable_profile=no ;;
    *) AC_MSG_ERROR(bad value "${enableval}" for --enable-profile) ;;
  esac],
  [enable_profile=no])

if test x"$enable_profile" = x"yes"; then
  if test x"$GXX" = x"yes" || test x"$GCC" = x"yes"; then
    CFLAGS="$CFLAGS -pg"
    CXXFLAGS="$CXXFLAGS -pg"
    LDFLAGS="$LDFLAGS -pg"
  else
    AC_MSG_WARN([--enable-profile only has effect when using GNU gcc or g++])
  fi
fi
])


# Usage:
#   SIM_AC_COMPILER_WARNINGS
#
# Description:
#   Take care of making a sensible selection of warning messages
#   to turn on or off.
# 
#   Note: this macro must be placed after either AC_PROG_CC or AC_PROG_CXX
#   in the configure.in script.
# 
# Author: Morten Eriksen, <mortene@sim.no>.
# 
# TODO:
#   * [mortene:19991114] find out how to get GCC's
#     -Werror-implicit-function-declaration option to work as expected
#
#   * [larsa:20010504] rename to SIM_AC_COMPILER_WARNINGS and clean up
#     the macro

AC_DEFUN([SIM_AC_COMPILER_WARNINGS], [
AC_ARG_ENABLE(
  [warnings],
  AC_HELP_STRING([--enable-warnings],
                 [turn on warnings when compiling [[default=yes]]]),
  [case "${enableval}" in
    yes) enable_warnings=yes ;;
    no)  enable_warnings=no ;;
    *) AC_MSG_ERROR([bad value "$enableval" for --enable-warnings]) ;;
  esac],
  [enable_warnings=yes])

if test x"$enable_warnings" = x"yes"; then
  if test x"$GCC" = x"yes"; then
    SIM_AC_CC_COMPILER_OPTION([-W -Wall -Wno-unused],
                              [CFLAGS="$CFLAGS -W -Wall -Wno-unused"])
    SIM_AC_CC_COMPILER_OPTION([-Wno-multichar],
                              [CFLAGS="$CFLAGS -Wno-multichar"])
  fi

  if test x"$GXX" = x"yes"; then
    SIM_AC_CXX_COMPILER_OPTION([-W -Wall -Wno-unused],
                               [CXXFLAGS="$CXXFLAGS -W -Wall -Wno-unused"])
    SIM_AC_CXX_COMPILER_OPTION([-Wno-multichar],
                               [CXXFLAGS="$CXXFLAGS -Wno-multichar"])
  fi

  case $host in
  *-*-irix*) 
    ### Turn on all warnings ######################################
    if test x"$CC" = xcc || test x"$CC" = xCC; then
      SIM_AC_CC_COMPILER_OPTION([-fullwarn], [CFLAGS="$CFLAGS -fullwarn"])
    fi
    if test x"$CXX" = xCC; then
      SIM_AC_CXX_COMPILER_OPTION([-fullwarn], [CXXFLAGS="$CXXFLAGS -fullwarn"])
    fi

    ### Turn off specific (bogus) warnings ########################

    ### SGI MipsPro v?.?? (our compiler on IRIX 6.2) ##############
    ##
    ## 3115: ``type qualifiers are meaningless in this declaration''.
    ## 3262: unused variables.
    ##
    ### SGI MipsPro v7.30 #########################################
    ##
    ## 1174: "The function was declared but never referenced."
    ## 1209: "The controlling expression is constant." (kill warning on
    ##       if (0), assert(FALSE), etc).
    ## 1355: Kill warnings on extra semicolons (which happens with some
    ##       of the Coin macros).
    ## 1375: Non-virtual destructors in base classes.
    ## 3201: Unused argument to a function.
    ## 1110: "Statement is not reachable" (the Lex/Flex generated code in
    ##       Coin/src/engines has lots of shitty code which needs this).
    ## 1506: Implicit conversion from "unsigned long" to "long".
    ##       SbTime.h in SGI/TGS Inventor does this, so we need to kill
    ##       this warning to avoid all the output clutter when compiling
    ##       the SoQt, SoGtk or SoXt libraries on IRIX with SGI MIPSPro CC.
    ## 1169: External/internal linkage conflicts with a previous declaration.
    ##       We get this for the "friend operators" in SbString.h

    sim_ac_bogus_warnings="-woff 3115,3262,1174,1209,1355,1375,3201,1110,1506,1169"

    if test x"$CC" = xcc || test x"$CC" = xCC; then
      SIM_AC_CC_COMPILER_OPTION([$sim_ac_bogus_warnings],
                                [CFLAGS="$CFLAGS $sim_ac_bogus_warnings"])
    fi
    if test x"$CXX" = xCC; then
      SIM_AC_CXX_COMPILER_OPTION([$sim_ac_bogus_warnings],
                                 [CXXFLAGS="$CXXFLAGS $sim_ac_bogus_warnings"])
    fi
  ;;
  esac
fi
])

# conf-macros/sogui.m4
#
# Common macros for the various GUI toolkit libraries for Coin.
#
# Authors:
#   Lars J. Aas <larsa@sim.no>

# SIM_AC_SOGUI_STATIC_DEFAULTS
# ============================
# If --disable-static-defaults is used, do not define WITH_STATIC_DEFAULTS.

AC_DEFUN([SIM_AC_SOGUI_STATIC_DEFAULTS],
[
sim_ac_static_defaults=true;
AC_ARG_ENABLE(
  [static-defaults],
  AC_HELP_STRING([--disable-static-defaults], [Disable defaults from being statically linked in]),
  [case ${enable_static_defaults} in
   no)  sim_ac_static_defaults=false ;;
   yes) ;;
   *)   echo "Option '--enable-static-defaults=${enable_static_defaults}' ignored" ;;
  esac],
  [])

if $sim_ac_static_defaults; then
  AC_DEFINE(WITH_STATIC_DEFAULTS, ,
    [Define this if you want defaults to be linked into SoXt])
fi
])


# **************************************************************************
# SIM_AC_UNIQIFY_LIST( VARIABLE, LIST )
#
# This macro filters out redundant items from a list.  This macro was made
# to avoid having multiple equivalent -I and -L options for the compiler on
# the command-line, which made compilation quite messy to watch.
#
# BUGS:
#   Items with spaces are probably not supported.
#
# Authors:
#   Lars J. Aas <larsa@sim.no>
#

AC_DEFUN([SIM_AC_UNIQIFY_LIST], [
sim_ac_save_prefix=$prefix
sim_ac_save_exec_prefix=$exec_prefix
test x"$prefix" = xNONE && prefix=/usr/local
test x"$exec_prefix" = xNONE && exec_prefix='${prefix}'
sim_ac_uniqued_list=
for sim_ac_item in $2; do
  eval sim_ac_eval_item="$sim_ac_item"
  eval sim_ac_eval_item="$sim_ac_eval_item"
  if test x"$sim_ac_uniqued_list" = x; then
    sim_ac_uniqued_list="$sim_ac_item"
  else
    sim_ac_unique=true
    for sim_ac_uniq in $sim_ac_uniqued_list; do
      eval sim_ac_eval_uniq="$sim_ac_uniq"
      eval sim_ac_eval_uniq="$sim_ac_eval_uniq"
      test x"$sim_ac_eval_item" = x"$sim_ac_eval_uniq" && sim_ac_unique=false
    done
    $sim_ac_unique && sim_ac_uniqued_list="$sim_ac_uniqued_list $sim_ac_item"
  fi
done
$1=$sim_ac_uniqued_list
prefix=$sim_ac_save_prefix
exec_prefix=$sim_ac_save_exec_prefix
# unset sim_ac_save_prefix
# unset sim_ac_save_exec_prefix
# unset sim_ac_eval_item
# unset sim_ac_eval_uniq
]) # SIM_AC_UNIQIFY_LIST


# CPP_AC_SEARCH_ORDER_FILTER( VARIABLE, LIST )
#
# This macro filters out system directories from a list. This macro was made
# to avoid passing -I options to the gcc3 compiler that are possibly already
# one of gcc3's system directories. This would cause the preprocessor to
# issue a "warning: changing search order for system directory ..." message
# that could break some configure scripts and produce annoying warning
# messages for every source file compiled.
#
# Authors:
#   Tamer Fahmy <tamer@tammura.at>
#

AC_DEFUN([CPP_AC_SEARCH_ORDER_FILTER], [
if test x"$GCC" = x"yes"; then
 sim_ac_save_cpp=$CPP
 CPP="cpp"
 case $host_os in
  darwin*) CPP="cpp3"
    ;;
  esac
  cpp_sys_dirs=`$CPP -v <<EOF 2>&1 | sed -n -e \
  '/#include <...> search starts here:/,/End of search list./{
    /#include <...> search starts here:/b
    /End of search list./b
    s/ /-I/
    p
  }'
  EOF`
  result=
  for inc_path in $2; do
    additem=true
    for sys_dir in $cpp_sys_dirs; do
      if test x$inc_path = x$sys_dir; then
        additem=false
        break
      fi
    done
    $additem && result="$result $inc_path"
  done
  $1=$result
  CPP=$sim_ac_save_cpp
fi
]) # CPP_AC_SEARCH_ORDER_FILTER



# AM_CONDITIONAL                                              -*- Autoconf -*-

# Copyright 1997, 2000, 2001 Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

# serial 5

AC_PREREQ(2.52)

# AM_CONDITIONAL(NAME, SHELL-CONDITION)
# -------------------------------------
# Define a conditional.
AC_DEFUN([AM_CONDITIONAL],
[ifelse([$1], [TRUE],  [AC_FATAL([$0: invalid condition: $1])],
        [$1], [FALSE], [AC_FATAL([$0: invalid condition: $1])])dnl
AC_SUBST([$1_TRUE])
AC_SUBST([$1_FALSE])
if $2; then
  $1_TRUE=
  $1_FALSE='#'
else
  $1_TRUE='#'
  $1_FALSE=
fi
AC_CONFIG_COMMANDS_PRE(
[if test -z "${$1_TRUE}" && test -z "${$1_FALSE}"; then
  AC_MSG_ERROR([conditional "$1" was never defined.
Usually this means the macro was only invoked conditionally.])
fi])])

# Usage:
#   SIM_AC_HAVE_COIN_IFELSE( IF-FOUND, IF-NOT-FOUND )
#
# Description:
#   This macro locates the Coin development system.  If it is found,
#   the set of variables listed below are set up as described and made
#   available to the configure script.
#
#   The $sim_ac_coin_desired variable can be set to false externally to
#   make Coin default to be excluded.
#
# Autoconf Variables:
# > $sim_ac_coin_desired     true | false (defaults to true)
# < $sim_ac_coin_avail       true | false
# < $sim_ac_coin_cppflags    (extra flags the preprocessor needs)
# < $sim_ac_coin_cflags      (extra flags the C compiler needs)
# < $sim_ac_coin_cxxflags    (extra flags the C++ compiler needs)
# < $sim_ac_coin_ldflags     (extra flags the linker needs)
# < $sim_ac_coin_libs        (link library flags the linker needs)
# < $sim_ac_coin_datadir     (location of Coin data files)
# < $sim_ac_coin_includedir  (location of Coin headers)
# < $sim_ac_coin_version     (the libCoin version)
# < $sim_ac_coin_msvcrt      (the MSVC++ C library Coin was built with)
# < $sim_ac_coin_configcmd   (the path to coin-config or "false")
#
# Authors:
#   Lars J. Aas, <larsa@sim.no>
#   Morten Eriksen, <mortene@sim.no>
#
# TODO:
#

AC_DEFUN([SIM_AC_HAVE_COIN_IFELSE], [
AC_PREREQ([2.14a])

# official variables
sim_ac_coin_avail=false
sim_ac_coin_cppflags=
sim_ac_coin_cflags=
sim_ac_coin_cxxflags=
sim_ac_coin_ldflags=
sim_ac_coin_libs=
sim_ac_coin_datadir=
sim_ac_coin_includedir=
sim_ac_coin_version=

# internal variables
: ${sim_ac_coin_desired=true}
sim_ac_coin_extrapath=

AC_ARG_WITH([coin],
AC_HELP_STRING([--with-coin], [enable use of Coin [[default=yes]]])
AC_HELP_STRING([--with-coin=DIR], [give prefix location of Coin]),
  [ case $withval in
    no)  sim_ac_coin_desired=false ;;
    yes) sim_ac_coin_desired=true ;;
    *)   sim_ac_coin_desired=true
         sim_ac_coin_extrapath=$withval ;;
    esac],
  [])

case $build in
*-mks ) sim_ac_pathsep=";" ;;
* )     sim_ac_pathsep="${PATH_SEPARATOR}" ;;
esac

if $sim_ac_coin_desired; then
  sim_ac_path=$PATH
  test -z "$sim_ac_coin_extrapath" || ## search in --with-coin path
    sim_ac_path="$sim_ac_coin_extrapath/bin${sim_ac_pathsep}$sim_ac_path"
  test x"$prefix" = xNONE ||          ## search in --prefix path
    sim_ac_path="$sim_ac_path${sim_ac_pathsep}$prefix/bin"

  AC_PATH_PROG(sim_ac_coin_configcmd, coin-config, false, $sim_ac_path)

  if ! test "X$sim_ac_coin_configcmd" = "Xfalse"; then
    test -n "$CONFIG" &&
      $sim_ac_coin_configcmd --alternate=$CONFIG >/dev/null 2>/dev/null &&
      sim_ac_coin_configcmd="$sim_ac_coin_configcmd --alternate=$CONFIG"
  fi

  if $sim_ac_coin_configcmd; then
    sim_ac_coin_version=`$sim_ac_coin_configcmd --version`
    sim_ac_coin_cppflags=`$sim_ac_coin_configcmd --cppflags`
    sim_ac_coin_cflags=`$sim_ac_coin_configcmd --cflags 2>/dev/null`
    sim_ac_coin_cxxflags=`$sim_ac_coin_configcmd --cxxflags`
    sim_ac_coin_ldflags=`$sim_ac_coin_configcmd --ldflags`
    sim_ac_coin_libs=`$sim_ac_coin_configcmd --libs`
    sim_ac_coin_datadir=`$sim_ac_coin_configcmd --datadir`
    # Hide stderr on the following, as ``--includedir'', ``--msvcrt''
    # and ``--cflags'' options were added late to coin-config.
    sim_ac_coin_includedir=`$sim_ac_coin_configcmd --includedir 2>/dev/null`
    sim_ac_coin_msvcrt=`$sim_ac_coin_configcmd --msvcrt 2>/dev/null`
    sim_ac_coin_cflags=`$sim_ac_coin_configcmd --cflags 2>/dev/null`
    AC_CACHE_CHECK(
      [if we can compile and link with the Coin library],
      sim_cv_coin_avail,
      [sim_ac_save_cppflags=$CPPFLAGS
      sim_ac_save_cxxflags=$CXXFLAGS
      sim_ac_save_ldflags=$LDFLAGS
      sim_ac_save_libs=$LIBS
      CPPFLAGS="$CPPFLAGS $sim_ac_coin_cppflags"
      CXXFLAGS="$CXXFLAGS $sim_ac_coin_cxxflags"
      LDFLAGS="$LDFLAGS $sim_ac_coin_ldflags"
      LIBS="$sim_ac_coin_libs $LIBS"
      AC_LANG_PUSH(C++)

      AC_TRY_LINK(
        [#include <Inventor/SoDB.h>],
        [SoDB::init();],
        [sim_cv_coin_avail=true],
        [sim_cv_coin_avail=false])

      AC_LANG_POP
      CPPFLAGS=$sim_ac_save_cppflags
      CXXFLAGS=$sim_ac_save_cxxflags
      LDFLAGS=$sim_ac_save_ldflags
      LIBS=$sim_ac_save_libs
    ])
    sim_ac_coin_avail=$sim_cv_coin_avail
    if ! $sim_ac_coin_avail; then
      AC_MSG_WARN([
Compilation and/or linking with the Coin main library SDK failed, for
unknown reason. If you are familiar with configure-based configuration
and building, investigate the 'config.log' file for clues.

If you can not figure out what went wrong, please forward the 'config.log'
file to the email address <coin-support@coin3d.org> and ask for help by
describing the situation where this failed.
])
    fi
  else # no 'coin-config' found
    locations=`IFS="${sim_ac_pathsep}"; for p in $sim_ac_path; do echo " -> $p/coin-config"; done`
    AC_MSG_WARN([cannot find 'coin-config' at any of these locations:
$locations])
    AC_MSG_WARN([
Need to be able to run 'coin-config' to figure out how to build and link
against the Coin library. To rectify this problem, you most likely need
to a) install Coin if it has not been installed, b) add the Coin install
bin/ directory to your PATH environment variable.
])
  fi
fi

if $sim_ac_coin_avail; then
  ifelse([$1], , :, [$1])
else
  ifelse([$2], , :, [$2])
fi
]) # SIM_AC_HAVE_COIN_IFELSE()

