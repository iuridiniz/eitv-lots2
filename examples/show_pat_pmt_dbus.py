#!/usr/bin/python

import dbus
from dbus.mainloop.glib import DBusGMainLoop
import gobject

dbus.set_default_main_loop(DBusGMainLoop())

session = dbus.SessionBus()
proxy = session.get_object('com.eitv.lots', '/com/eitv/lots')

player = dbus.Interface(proxy, 'com.eitv.lots.player')
source = dbus.Interface(proxy, 'com.eitv.lots.source')
demuxer = dbus.Interface(proxy, 'com.eitv.lots.demuxer')


class PatPmtHandler:
    def __init__(self, demuxer, loop = None):
        self.demuxer = demuxer
        self.loop = loop
        self.programs = []
        self.n_programs = 0
    
        # connect to pat
        self.demuxer.connect_to_signal('new_payload', self.new_payload_handler)
        self.demuxer.add_watch(0)

    def new_pmt(self, pid, data):
        pmt = {}
        pmt["pid"] = int(pid)
        pmt["streams"] = []
        
        info_lenght = (data[10] << 8 | data[11]) & 0x0fff;
        i = 12 + info_lenght
        while i < (len(data) - 4):
            stream = {}
            stream["type"] = int(data[i])
            stream["pid"] = (data[i+1] << 8 | data[i+2]) & 0x1fff
            es_info_lenght = (data[i+3] << 8 | data[i+4]) & 0x0fff
            i = i + 5 + es_info_lenght
            pmt["streams"].append(stream)

        self.programs.append(pmt)

        if self.n_programs == len(self.programs):
            self.dump_programs()
            if self.loop:
                self.loop.quit()

    def dump_programs(self):
        from pprint import pprint
        pprint(self.programs)

    def new_pat(self, data):
        self.n_programs = int((len(data)-12)/4)
        for i in range(8, len(data) - 4, 4): 
            #p_number = data[i] << 8 | data[i+1]
            pmt_pid = (data[i+2] << 8 | data[i+3]) & 0x1fff 
            #connect to pmt
            self.demuxer.add_watch(pmt_pid)

    def new_payload_handler(self, pid, data):
        demuxer.del_watch(pid)
        if (pid == 0):
            self.new_pat(data)
        else:
            self.new_pmt(pid, data)

   
if __name__ == '__main__':
    if not player.is_playing():
    	source.set_uri(u'file://./sample.ts')
        player.play_with_pids(1281, 2561, 1281)
    
    loop = gobject.MainLoop()
    pat_pmt = PatPmtHandler(demuxer, loop)
    loop.run()


