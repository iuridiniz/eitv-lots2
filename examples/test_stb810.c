
#include <glib-object.h>
#include <glib/gprintf.h>
#include <stdlib.h>
#include <unistd.h>
#include "mdw_lots.h"
#include "mdw_stb810_player.h"
#include "mdw_dvb_demux.h"
#include "mdw_ts_file_source.h"


#define MY_ASSERT(x) \
    { \
        g_fprintf(stderr, "Executing: %s\n", #x); \
        g_assert((x)); \
    } while (0)

int main(int argc, char** argv) {

    g_type_init();

    GError* error = NULL;

    MdwPlayer* player = NULL;
    MdwTsSource* source = NULL;
    MdwDemux* demux = NULL;

    MY_ASSERT(argc == 6);
    const gchar* file = argv[1];

    const gint video1 = atoi(argv[2]);
    const gint audio1 = atoi(argv[3]);

    const gint video2 = atoi(argv[4]);
    const gint audio2 = atoi(argv[5]);

    g_type_init();
    
    /* creating the objects */
    MY_ASSERT( source = MDW_TS_SOURCE(mdw_ts_file_source_new_with_location(file)) );
    MY_ASSERT( demux = MDW_DEMUX(mdw_dvb_demux_new()) );
    MY_ASSERT( mdw_demux_set_ts_source(demux, source) == TRUE);

    MY_ASSERT( player = MDW_PLAYER(mdw_stb810_player_new()) );
    
    /* setting demux */
    {
        MdwDemux* d;
        mdw_player_set_demux(player, demux);
        
        /* was setted correctly? */
        MY_ASSERT( d = mdw_player_get_demux(player) );
        MY_ASSERT( d == demux );
    }

    /* configuring the audio pid */
    {
        int pid;
        mdw_player_set_audio_pid(player, audio1);

        /* was setted correctly? */
        pid = mdw_player_get_audio_pid(player);
        MY_ASSERT(pid == audio1);
    }

    /* configuring the video pid */
    {
        int pid;
        mdw_player_set_video_pid(player, video1);
        
        /* was setted correctly? */
        pid = mdw_player_get_video_pid(player);
        MY_ASSERT(pid == video1);
    }

    /* starting to play */
    MY_ASSERT( mdw_player_play(player, &error) == TRUE );

    sleep(1);

    /* lowering the video */
    {
        int old, new;
        old = mdw_stb810_player_get_level(MDW_STB810_PLAYER(player));

        mdw_player_lower_to_bottom(player, NULL);

        new = mdw_stb810_player_get_level(MDW_STB810_PLAYER(player));

        MY_ASSERT(old - 1 == new);

    }

    sleep(1);

    /* raising the video */
    { 
        int old, new;
        old = mdw_stb810_player_get_level(MDW_STB810_PLAYER(player));

        mdw_player_raise_to_top(player, NULL);

        new = mdw_stb810_player_get_level(MDW_STB810_PLAYER(player));

        MY_ASSERT(old + 1 == new);
    }

    sleep(1);
    
    /* resizing the video */
    {   
        guint16 w, h;

        mdw_player_set_size(player, 210, 180, NULL);
        mdw_player_get_size(player, &w, &h, NULL);
        
        MY_ASSERT(w == 210);
        MY_ASSERT(h == 180);
    }
    
    sleep(1);

    /* stopping the video */
    MY_ASSERT( mdw_player_stop(player, &error) == TRUE );

    sleep(1);
    
    /* changing the pids */
    {
        int apid, vpid;

        mdw_player_set_audio_pid(player, audio2);
        mdw_player_set_video_pid(player, video2);

        apid = mdw_player_get_audio_pid(player);
        vpid = mdw_player_get_video_pid(player);

        MY_ASSERT(apid == audio2);
        MY_ASSERT(vpid == video2);

    }
    /* starting to play again */
    MY_ASSERT( mdw_player_play(player, &error) == TRUE );

    sleep(1);

    g_object_unref(G_OBJECT(player));
    g_object_unref(G_OBJECT(source));
    g_object_unref(G_OBJECT(demux));

    return 0;

}

