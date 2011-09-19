#AM_CFLAGS= -I$(top_srcdir)/src @GLIB_CFLAGS@ @FFMPEG_CFLAGS@ @DIRECTFB_CFLAGS@ @DBUS_GLIB_CFLAGS@
AM_CPPFLAGS = -I$(top_srcdir)/src -I../src @GLIB_CFLAGS@ @FFMPEG_CFLAGS@ @DIRECTFB_CFLAGS@ @DBUS_GLIB_CFLAGS@
LDADD= -L../src -lmdwlots @GLIB_LIBS@ @FFMPEG_LIBS@ @DIRECTFB_LIBS@ @DBUS_GLIB_LIBS@

noinst_PROGRAMS =

if HAVE_DIRECTFB
if HAVE_LINUXDVBAPI
noinst_PROGRAMS +=\
	test_stb810
test_stb810_SOURCES = \
	test_stb810.c 
test_stb810_CFLAGS = $(AM_CFLAGS) \
    -I$(top_srcdir)/src
endif
endif

#pat pmt 
noinst_PROGRAMS += \
	show_pat_pmt
show_pat_pmt_SOURCES = \
	show_pat_pmt.c 

#section dumper
noinst_PROGRAMS += \
	section_dumper
section_dumper_SOURCES = \
	section_dumper.c 

#dump stream
noinst_PROGRAMS += \
	stream_dumper
stream_dumper_SOURCES = \
	stream_dumper.c 

if HAVE_DBUS_GLIB
noinst_PROGRAMS += \
	daemon	
daemon_SOURCES = \
	daemon.c 
noinst_PROGRAMS += \
	show_pat_pmt_dbus
show_pat_pmt_dbus_SOURCES = \
	show_pat_pmt_dbus.c 
endif

