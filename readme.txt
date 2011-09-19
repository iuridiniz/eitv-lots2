Diretorios

* src/
    Classes
* unittest/
    testes
* examples/
    exemplos de uso da lib
* extras/
    extras

Interfaces 
 - MdwDemux
 - MdwPlayer
 - MdwTsSource
 - MdwDemuxDataHandler

Classes
 - MdwDefaultDemux (implementa MdwDemux)
 - MdwDvbDemux (implementa MdwDemux)

 - MdwDvbTuner (implementa MdwTsSource e possivelmente um MdwTunner)
 - MdwTsFileSource (implementa MdwTsSource)
 * MdwUdpSource (implementa MdwTsSource)
 
 - MdwDefaultPlayer (implementa MdwPlayer e MdwDemuxDataHandler)
 - MdwStb810Player (implementa MdwPlayer) - Player para plataforma NXP STB810

 - MdwDvbAudioDummy (implementa MdwDemuxDataHandler) - somente para uso de MdwDvbDemux
 - MdwDvbVideoDummy (implementa MdwDemuxDataHandler) - somente para uso de MdwDvbDemux

 - MdwDemuxDataHandlerAdapter (implementa MdwDemuxDataHandler) - forma facil de fazer um DemuxDataHandler
 - MdwDemuxSectionDumper (implementa MdwDemuxDataHandler) - decodifica tabelas PSI e imprime na saida padrao.

 - MdwManager (implementa MdwDemuxDataHandler) - para controlar o player via DBus.

Outros:
 - mdw_util.c: funcoes extra

Tecnologias usadas:
 - linuxdvbapi (aka LinuxTV) - http://www.linuxtv.org/downloads/linux-dvb-api-1.0.0.pdf
 - gobject - http://developer.gnome.org/doc/API/2.0/gobject/
 - glib - http://developer.gnome.org/doc/API/2.0/glib/
 - DirectFB - http://www.directfb.org/docs/DirectFB_Reference/index.html
 - libavcodec (FFMpeg) - http://ffmpeg.mplayerhq.hu/documentation.html | http://cekirdek.pardus.org.tr/~ismail/ffmpeg-docs/
 - dbus - http://dbus.freedesktop.org/doc/dbus-tutorial.html
