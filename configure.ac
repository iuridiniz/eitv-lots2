
dnl ********
dnl PRELUDE
dnl ********
AC_INIT([MDW_LOTS], [2.2], [iuri.diniz@eitv.com.br])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

dnl ********
dnl PROGRAMS
dnl ********

dnl CXX=no
dnl F77=no
dnl AC_PROG_CC
AC_PROG_CC_C99
AC_PROG_LIBTOOL
dnl AC_PROG_RANLIB


dnl C inline
dnl AC_C_INLINE


dnl unittesting
dnl AM_PATH_CHECK([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for check, and define CHECK_CFLAGS and CHECK_LIBS
dnl
AM_PATH_CHECK(0.9.3, [have_check="true"], [have_check="false"])
AM_CONDITIONAL(HAVE_CHECK, [test "${have_check}" = "true"])

dnl ENABLE_STB810
AC_ARG_ENABLE([stb810], 
    [  --enable-stb810     configure typical flags and libs for NXP Stb810 plataform (default=no)])

if test "${enable_stb810}" = "yes"; then
    enable_linuxdvbapi="yes"
    enable_directfb="yes"
    enable_dbus="yes"
    enable_ffmpeg="no"
    with_dvbbase="/dev/dvb/adapter3"
    with_directfb_video_layer="3"
    AC_DEFINE(FOR_STB810, [], [Compilation for NXP Stb810 Plataform])
fi

AC_ARG_ENABLE([pc],
    [  --enable-pc      configure typical flags and libs for execute in typical PC system (default=no)])

if test "${enable_pc}" = "yes"; then
    enable_linuxdvbapi="no"
    enable_directfb="yes"
    enable_dbus="yes"
    enable_ffmpeg="yes"
    with_directfb_video_layer="0"
    AC_DEFINE(FOR_PC, [], [Compilation for a Home PC Plataform])
fi

if test "${enable_pc}" = "yes" -a "${enable_stb810}" = "yes"; then
    dnl AC_MSG_WARNING([Compiling for more than one plataform at same time])
    true
fi


dnl GLIB
dnl AM_PATH_GLIB_2_0([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND [, MODULES]]]])
dnl Test for GLIB, and define GLIB_CFLAGS and GLIB_LIBS, if gmodule, gobject or dnl gthread is specified in MODULES, pass to pkg-config
dnl
AM_PATH_GLIB_2_0(2.12.4, [true], [AC_MSG_ERROR([cannot find glib])], [gmodule gobject gthread])

LOTS_CFLAGS="${LOTS_CFLAGS} ${GLIB_CFLAGS}"
LOTS_LIBS="${LOTS_LIBS} ${GLIB_LIBS}"

dnl LINUX DVB API
AC_ARG_ENABLE([linuxdvbapi], 
    [  --enable-linuxdvbapi    use Linux DVB API (linux tv) (default=yes)])

AC_ARG_WITH([dvbbase],
    [  --with-dvbbase=PATH          specify which dir has dvb devices that are allowed to use (default=/dev/dvb/adapter0)])

have_linuxdvbapi="false"
if test "${enable_linuxdvbapi}" != "no"; then
    AC_CHECK_HEADER([linux/dvb/dmx.h], 
        [have_linuxdvbapi="true"], 
        [have_linuxdvbapi="false"])
fi

if test "${have_linuxdvbapi}" = "true"; then
    AC_DEFINE(HAVE_LINUXDVBAPI, [], [support for linux dvb api])
    if test "x${with_dvbbase}" != "x"; then
        AC_DEFINE_UNQUOTED(DVB_BASE, "${with_dvbbase}", [DVB base dir to use])
    else 
        AC_DEFINE(DVB_BASE, ["/dev/dvb/adapter0"], [DVB base dir to use])
    fi
fi

if test "${enable_linuxdvbapi}" = "yes"; then
    if test "${have_linuxdvbapi}" != "true"; then
        AC_MSG_ERROR([cannot find headers for linuxdvbapi])
    fi
fi
AM_CONDITIONAL(HAVE_LINUXDVBAPI, [test "${have_linuxdvbapi}" = "true"])

dnl ENABLE DEBUG
AC_ARG_ENABLE([debug], 
    [  --enable-debug          enable debug symbols(gdb) and the debug message level =(0|1|2|3|4|yes|no) (default=no)])

debug_level="0"
if test -n "${enable_debug}" -a "${enable_debug}" != "no"; then

    if test "${enable_debug}" = "1" -o "${enable_debug}" = "yes"; then
        debug_level="1"
    elif test "${enable_debug}" = "2"; then
        debug_level="2"
    elif test "${enable_debug}" = "3"; then
        debug_level="3"
    elif test "${enable_debug}" -gt "3"; then
        debug_level="4"
    fi
    CFLAGS="${CFLAGS} -g"
fi
AC_DEFINE_UNQUOTED(MDW_DEBUG_LEVEL, ${debug_level}, [Default debug level])

AC_ARG_ENABLE([release],
    [  --enable-release        configure special flags to release (default=no)])
 
if test "${enable_release}" = "yes"; then
    CPPFLAGS="${CPPFLAGS} -DG_DISABLE_ASSERT -DG_DISABLE_CHECKS"
    CFLAGS="${CFLAGS} -O3"
fi

AC_ARG_ENABLE([directfb], 
    [  --enable-directfb       enable DirectFB (default=yes)]) 

AC_ARG_WITH([directfb-video-layer],
    [  --with-directfb-video-layer=LAYER   specify which layer to use as video layer(default=0)])

have_directfb="false"
if test "${enable_directfb}" != "no"; then
    PKG_CHECK_MODULES([DIRECTFB], [directfb], [have_directfb="true"], [have_directfb="false"])
fi

if test "${have_directfb}" = "false" -a "${enable_directfb}" = "yes"; then
    AC_MSG_ERROR([cannot find directfb, try: --enable-directfb=no])
fi

if test "${have_directfb}" = "true"; then
    if test "x${with_directfb_video_layer}" != "x"; then
        AC_DEFINE_UNQUOTED(VIDEO_LAYER, ${with_directfb_video_layer}, [DirectFB layer to use as video Layer])
    else 
        AC_DEFINE_UNQUOTED(VIDEO_LAYER, 0, [DirectFB layer to use as video Layer])
    fi
fi

AM_CONDITIONAL(HAVE_DIRECTFB, [test "${have_directfb}" = "true"])
LOTS_CFLAGS="${LOTS_CFLAGS} ${DIRECTFB_CFLAGS}"
LOTS_LIBS="${LOTS_LIBS} ${DIRECTB_LIBS}"

AC_ARG_ENABLE([ffmpeg], 
    [  --enable-ffmpeg      enable ffmpeg (default=yes)])

have_ffmpeg="false"
enable_ffmpeg="no"
if test "${enable_ffmpeg}" != "no"; then
    PKG_CHECK_MODULES([FFMPEG], [libavcodec libavutil libavformat], [have_ffmpeg="true"], [have_ffmpeg="false"])
fi

if test "${have_ffmpeg}" = "false"; then
    if test "${enable_ffmpeg}" = "yes"; then
        AC_MSG_ERROR([cannot find ffmpeg, try: --disable-ffmpeg])
    fi
fi

AM_CONDITIONAL(HAVE_FFMPEG, [test "${have_ffmpeg}" = "true"])
LOTS_CFLAGS="${LOTS_CFLAGS} ${FFMPEG_CFLAGS}"
LOTS_LIBS="${LOTS_LIBS} ${FFMPEG_LIBS}"

dnl check for dbus-glib
AC_ARG_ENABLE([dbus], 
    [  --enable-dbus    enable dbus (via glib bindings) (default=yes)])

if test "${enable_dbus}" != "no"; then
    PKG_CHECK_MODULES(DBUS_GLIB, dbus-glib-1 >= 0.71, [have_dbus_glib="true"], [have_dbus_glib="false"])
fi
if test "${have_dbus_glib}" = "false"; then
    if test "${enable_dbus}" = "yes"; then 
        AC_MSG_ERROR([Cannot find dbus glib bindings, try: --enable-dbus=no]) 
    fi
fi

AM_CONDITIONAL(HAVE_DBUS_GLIB, [test "${have_dbus_glib}" = "true"])
LOTS_CFLAGS="${LOTS_CFLAGS} ${DBUS_GLIB_CFLAGS}"
LOTS_LIBS="${LOTS_LIBS} ${DBUS_GLIB_LIBS}"

dnl CFLAGS and CPPFLAGS
CFLAGS="${CFLAGS} -Wall -pedantic"

dnl PTHREADS
dnl LDFLAGS="${LDFLAGS} -lpthread"

dnl OUTPUTS

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
    Makefile
    unittest/Makefile
    src/Makefile
    src/sec_filter/Makefile
    examples/Makefile
])
AC_OUTPUT

#SUMMARY
echo "******************************************"
echo "LOTS_CFLAGS=${LOTS_CFLAGS}"
echo "CFLAGS=${CFLAGS}"
echo "CPPFLAGS=${CPPFLAGS}"
echo ""
echo "LOTS_LIBS=${LOTS_LIBS}"
echo "LD_FLAGS=${LD_FLAGS}"
echo "******************************************"

