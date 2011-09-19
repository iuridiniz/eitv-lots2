SUBDIRS=

AM_CFLAGS=@GLIB_CFLAGS@
LIBS=@GLIB_LIBS@
BUILT_SOURCES=

if HAVE_FFMPEG
AM_CFLAGS += @FFMPEG_CFLAGS@
LIBS += @FFMPEG_LIBS@
endif

if HAVE_DIRECTFB
AM_CFLAGS += @DIRECTFB_CFLAGS@
LIBS += @DIRECTFB_LIBS@
endif

if HAVE_DBUS_GLIB
AM_CFLAGS += @DBUS_GLIB_CFLAGS@
LIBS += @DBUS_GLIB_LIBS@
BUILT_SOURCES += \
	mdw_lots_dbus.h \
	mdw_manager_dbus.h \
	mdw_marshallers.h

SUBDIRS += sec_filter
endif

lib_LTLIBRARIES = \
	libmdwlots.la 

libmdwlots_la_SOURCES =

#######################################
# Utils 
#######################################
libmdwlots_la_SOURCES += \
	mdw_util.c \
	mdw_util.h

#######################################
# Non-Instanciable classes (interfaces)
#######################################
# interface MdwDemux
libmdwlots_la_SOURCES += \
	mdw_demux.c \
	mdw_demux.h

# interface MdwDemuxDataHandler
libmdwlots_la_SOURCES += \
	mdw_demux_data_handler.c \
	mdw_demux_data_handler.h

# interface MdwTsSource
libmdwlots_la_SOURCES += \
	mdw_ts_source.c \
    mdw_ts_source.h

# interface MdwPlayer 
libmdwlots_la_SOURCES += \
	mdw_player.c \
	mdw_player.h

#######################################
# Instanciable classes
#######################################
# Class MdwTsFileSource
libmdwlots_la_SOURCES += \
	mdw_ts_file_source.h \
	mdw_ts_file_source.c

if HAVE_LINUXDVBAPI 
# Class MdwDvbDemux (implements MdwDemux)
libmdwlots_la_SOURCES += \
	mdw_dvb_demux.c \
	mdw_dvb_demux.h 

# Class MdwDvbVideoDummy (implements MdwDemuxDataHandler)
libmdwlots_la_SOURCES += \
	mdw_dvb_video_dummy.c \
	mdw_dvb_video_dummy.h

# Class MdwDvbAudioDummy (implements MdwDemuxDataHandler)
libmdwlots_la_SOURCES += \
	mdw_dvb_audio_dummy.c \
	mdw_dvb_audio_dummy.h

# Class MdwDvbTunner (implements MdwTsSource)
libmdwlots_la_SOURCES += \
	mdw_dvb_tuner.c \
	mdw_dvb_tuner.h

if HAVE_DIRECTFB
# Class MdwStb810Player (implements MdwPlayer)
libmdwlots_la_SOURCES += \
	mdw_stb810_player.c \
	mdw_stb810_player.h
endif #HAVE_DIRECTFB

endif #HAVE_LINUXDVBAPI 

# Class MdwDefaultDemux (implements MdwDemux)
libmdwlots_la_SOURCES += \
	mdw_default_demux.h \
	mdw_default_demux.c 

# Class MdwSectionDumper (implements MdwDemuxDataHandler)
libmdwlots_la_SOURCES += \
	mdw_section_dumper.h \
	mdw_section_dumper.c

# Class MdwDemuxDataHandlerAdapter (implements MdwDemuxDataHandler)
libmdwlots_la_SOURCES += \
	mdw_demux_data_handler_adapter.h \
	mdw_demux_data_handler_adapter.c

# Class MdwDefaultPlayer (implements MdwDemuxDataHandler and MdwPlayer) 
libmdwlots_la_SOURCES += \
	mdw_default_player.c \
	mdw_default_player.h \
	mdw_default_player_decoder.h \
	mdw_default_player_display.h 

if HAVE_DIRECTFB
libmdwlots_la_SOURCES += \
	mdw_default_player_display_directfb.c
else
libmdwlots_la_SOURCES += \
	mdw_default_player_display_null.c
endif

if HAVE_FFMPEG
libmdwlots_la_SOURCES += \
	mdw_default_player_decoder_ffmpeg.c
else
libmdwlots_la_SOURCES += \
	mdw_default_player_decoder_null.c
endif

# Class MdwManager 
if HAVE_DBUS_GLIB
libmdwlots_la_SOURCES += \
	mdw_manager.c \
	mdw_manager.h \
	mdw_manager_dbus.h
endif


# extras
mdw_lots_dbus.h: mdw_manager.xml
	dbus-binding-tool --mode=glib-client $< > $@

mdw_manager_dbus.h: mdw_manager.xml
	dbus-binding-tool --mode=glib-server --prefix=mdw_manager $< > $@

mdw_marshallers.h: mdw_marshallers.list
	glib-genmarshal --header --body $< > $@

CLEANFILES = $(BUILT_SOURCES)
