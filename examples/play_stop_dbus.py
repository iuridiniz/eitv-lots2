#!/usr/bin/python

import dbus

session = dbus.SessionBus()
proxy = session.get_object('com.eitv.lots', '/com/eitv/lots')

player = dbus.Interface(proxy, 'com.eitv.lots.player')
source = dbus.Interface(proxy, 'com.eitv.lots.source')
demux = dbus.Interface(proxy, 'com.eitv.lots.demuxer')

if __name__ == '__main__':
    if player.is_playing():
        player.stop()
    else:
    	source.set_uri(u'file://./sample.ts')
        #player.play_with_pids(1281, 2561, 1281)
        player.play_with_pids(257, 258, 257)

