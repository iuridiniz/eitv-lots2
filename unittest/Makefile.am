AM_CFLAGS=@CHECK_CFLAGS@ @GLIB_CFLAGS@ @DIRECTFB_CFLAGS@ @FFMPEG_CFLAGS@ @DBUS_GLIB_CFLAGS@
AM_CPPFLAGS=-I../src
LIBS=-L../src -lmdwlots @CHECK_LIBS@ @GLIB_LIBS@ @DIRECTFB_LIBS@ @FFMPEG_LIBS@ @DBUS_GLIB_LIBS@
TESTS = test_ts_file_source 

noinst_PROGRAMS =

check_PROGRAMS =\
	test_ts_file_source 

if HAVE_LINUXDVBAPI
check_PROGRAMS +=\
	test_dvb_demux
TESTS += \
	test_dvb_demux
endif


# Lots DEMUX
check_PROGRAMS += \
	test_default_demux
TESTS += \
	test_default_demux

#if HAVE_FFMPEG
#if HAVE_DIRECTFB
check_PROGRAMS +=\
	test_default_player
TESTS += \
	test_default_player
#endif
#endif


if HAVE_DBUS_GLIB
check_PROGRAMS +=\
	test_lots_manager
TESTS += \
	test_lots_manager.sh

check_PROGRAMS +=\
	test_sec_filter
TESTS += \
	test_sec_filter
endif

###############################################
test_ts_file_source_SOURCES =\
	test_ts_file_source.c 

test_ts_file_source_DEPENDENCIES =\
	../src/libmdwlots.la 

	
##############################################
test_dvb_demux_SOURCES =\
	test_dvb_demux.c

test_dvb_demux_DEPENDENCIES =\
	../src/libmdwlots.la


##############################################
test_default_demux_SOURCES =\
	test_default_demux.c

test_default_demux_DEPENDENCIES =\
	../src/libmdwlots.la

##############################################
test_default_player_SOURCES =\
	test_default_player.c

test_default_player_DEPENDENCIES =\
	../src/libmdwlots.la

###############################################

test_lots_manager_SOURCES = \
	test_lots_manager.c

test_lots_manager_DEPENDENCIES = \
	../src/mdw_lots_dbus.h 


##############################################
test_sec_filter_SOURCES = \
	test_sec_filter.c

test_sec_filter_CPPFLAGS = \
	-I../src/sec_filter

test_sec_filter_DEPENDENCIES = \
	../src/sec_filter/libsecfilter.la
test_sec_filter_LDADD = \
	-L../src/sec_filter -lsecfilter

#test_lots_manager_DEPENDENCIES = \
#	../src/mdw_lots_dbus.h 

