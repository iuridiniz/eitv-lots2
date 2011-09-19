#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <glib.h>

#include "mdw_lots.h"
#include "mdw_util.h"
#include "mdw_section_dumper.h"

gboolean _start_demux(MdwDemux *demux)
{
    
    mdw_demux_start(demux);
    return FALSE;
}

gboolean _stop_demux(MdwDemux *demux)
{
    
    mdw_demux_stop(demux);
    return FALSE;
}

gboolean _loop_quit(GMainLoop *loop)
{
    
    g_main_loop_quit(loop);
    return FALSE;
}



int main(int argc, char** argv) 
{

    g_assert(argc > 1);

    g_type_init();
    
    char uri[1024];
    strcpy(uri, "file://");
    strncat(uri, argv[1], 1017);

    /* source from argv[1] */
    MdwTsSource *source = mdw_get_most_suitable_ts_source(uri);

    /* dumper instance*/
    MdwSectionDumper *dumper = mdw_section_dumper_new();

    /* demux instance */
    MdwDemux *demux = mdw_get_most_suitable_demux();

    /* no args */
    if (argc == 2) {
        /* dump pat */
        mdw_demux_add_section_handler(demux,
            MDW_DEMUX_DATA_HANDLER(dumper), 0);
    }
    /* add dumper foreach pid in args */
    for(int i = 2; i < argc; i++) {
        mdw_demux_add_section_handler(demux,
            MDW_DEMUX_DATA_HANDLER(dumper), atoi(argv[i]));
    }

    /* set source */
    mdw_demux_set_ts_source(demux, source);

    /* FIXME: use g_main_loop, the old interface doesn't work more 'cause is OLD :-P */
    GMainLoop* loop = g_main_loop_new(NULL, FALSE);

    /* start the process */
    g_timeout_add(0, (GSourceFunc) _start_demux, demux);
    
    /* stop */
    g_timeout_add(3000, (GSourceFunc) _stop_demux, demux);
    /* quit */
    g_timeout_add(3001, (GSourceFunc) _loop_quit, loop);

    /* loop */
    g_main_loop_run(loop);

    g_object_unref(dumper);
    g_object_unref(demux);
    g_object_unref(source);
    
    return 0;
}

