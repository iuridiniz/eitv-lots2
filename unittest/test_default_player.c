#include <glib.h>
#include <check.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/mdw_player.h"
#include "../src/mdw_default_player.h"

#include "../src/mdw_util.h"
#include "../src/mdw_demux.h"
#include "../src/mdw_ts_source.h"
#include "../src/mdw_ts_file_source.h"

#define NOT_NULL(x) OBLIGATORY((x) != NULL)

#define OBLIGATORY(x) \
    {   \
        if (!(x)) { \
            fail(#x); \
        } \
    } while (0)


/****************************************************************/
void new_setup() {
    g_type_init();
}

void new_teardown() {
}

START_TEST( test_new_g_object ) {
   
    MdwDefaultPlayer *p;
    NOT_NULL(p = g_object_new(MDW_TYPE_DEFAULT_PLAYER, NULL));
    
    g_object_unref(p);
    p = NULL;
    
}
END_TEST

START_TEST( test_new_func ) {
    MdwDefaultPlayer *p;
    NOT_NULL(p = mdw_default_player_new());

    g_object_unref(p);
    p = NULL;
}
END_TEST

START_TEST( test_get_set_demux ) {
    MdwDefaultPlayer *p;
    MdwDemux *d;
    NOT_NULL(p = mdw_default_player_new());
    NOT_NULL(d = mdw_get_most_suitable_demux());

    mdw_player_set_demux(MDW_PLAYER(p), d); 

    OBLIGATORY(mdw_player_get_demux(MDW_PLAYER(p)) == d);

    g_object_unref(d);
    g_object_unref(p);
    p = NULL;
   
}
END_TEST

/****************************************************************/

MdwTsSource *source = NULL;
MdwDemux* demux = NULL;
MdwPlayer* player = NULL;

#ifndef TS_FILE
#define TS_FILE "sample.ts"
#endif

#ifndef VIDEO_PID
#define VIDEO_PID 1280
#endif

#ifndef AUDIO_PID
#define AUDIO_PID 2560
#endif


#ifndef VIDEO_PID_2
#define VIDEO_PID_2 1281
#endif

#ifndef AUDIO_PID_2
#define AUDIO_PID_2 2561
#endif

#if 0
static void 
aux_get_default_size(int* OUT_width, int* OUT_height)
{
    static int width = -1;
    static int height = -1;

    if (width ==  -1 || height == -1) {
        width = 12

    }

    OBLIGATORY(width != NULL);
    OBLIGATORY(height != NULL);

    *OUT_width = width;
    *OUT_height = height;

}
#endif

void interface_setup() {
    g_type_init();
    demux = mdw_get_most_suitable_demux();
    source = mdw_get_most_suitable_ts_source("file://"TS_FILE);
    g_object_set(G_OBJECT(source), "continuous", TRUE, NULL);

    player = MDW_PLAYER(mdw_default_player_new());
    mdw_demux_set_ts_source(demux, source);
    mdw_player_set_demux(player, demux);
}

void interface_teardown() {
    g_object_unref(source);
    g_object_unref(demux);
    g_object_unref(player);
    source = NULL;
    demux = NULL;
    player = NULL;
}
START_TEST( test_pids ) {
    mdw_player_set_audio_pid(player, AUDIO_PID);
    OBLIGATORY(mdw_player_get_audio_pid(player) == AUDIO_PID);
    
    mdw_player_set_video_pid(player, VIDEO_PID);
    OBLIGATORY(mdw_player_get_video_pid(player) == VIDEO_PID);
}
END_TEST

START_TEST( test_play_stop ) {
    GError* error = NULL;
    /* no audio/video pids setted */
    g_debug("*** IGNORE NEXT CRITICAL ***");
    OBLIGATORY(mdw_player_play(player, NULL) == FALSE);
    if (error) {
        g_error_free(error);
        error = NULL;
    }

    mdw_player_set_audio_pid(player, AUDIO_PID);
    mdw_player_set_video_pid(player, VIDEO_PID);

    OBLIGATORY(mdw_player_play(player, NULL) == TRUE);

    /* play for 2 seconds */
    sleep(2);

    OBLIGATORY(mdw_player_stop(player, NULL) == TRUE);
   
    sleep(2);

    /* twice stop */
    g_debug("*** IGNORE NEXT CRITICAL ***");
    OBLIGATORY(mdw_player_stop(player, NULL) == FALSE);

    if (error) {
        g_error_free(error);
        error = NULL;
    }

}
END_TEST

START_TEST( test_change_pids ) {
    
    mdw_player_set_audio_pid(player, AUDIO_PID);
    mdw_player_set_video_pid(player, VIDEO_PID);

    OBLIGATORY(mdw_player_play(player, NULL) == TRUE);
    
    /* play for 2 seconds */
    sleep(2);

    OBLIGATORY(mdw_player_stop(player, NULL) == TRUE);

    mdw_player_set_audio_pid(player, AUDIO_PID_2);
    mdw_player_set_video_pid(player, VIDEO_PID_2);

    OBLIGATORY(mdw_player_play(player, NULL) == TRUE);
    
    /* play for 2 seconds */
    sleep(2);

    OBLIGATORY(mdw_player_stop(player, NULL) == TRUE);

}
END_TEST

START_TEST( test_change_size_and_pos_while_playing ) {
   
    /* TODO: get these values from directfb */
    const int screen_width = 640;
    const int screen_height = 480;

    guint16 width, height, posx, posy;

    /* preambule */
    mdw_player_set_audio_pid(player, AUDIO_PID);
    mdw_player_set_video_pid(player, VIDEO_PID);

    OBLIGATORY(mdw_player_play(player, NULL) == TRUE);
    /* preambule end */
    sleep(2);

    mdw_player_set_pos(player, 300, 100, NULL);
    mdw_player_get_pos(player, &posx, &posy, NULL);
    OBLIGATORY(posx == 300);
    OBLIGATORY(posy == 100);

    sleep(2);

    mdw_player_set_size(player, 100, 150, NULL);
    mdw_player_get_size(player, &width, &height, NULL);
    OBLIGATORY(width == 100);
    OBLIGATORY(height == 150);
   
    sleep(2);

    mdw_player_fullscreen(player, NULL);
    mdw_player_get_size(player, &width, &height, NULL);
    mdw_player_get_pos(player, &posx, &posy, NULL);
    OBLIGATORY(width == screen_width);
    OBLIGATORY(height == screen_height);
    OBLIGATORY(posx == 0);
    OBLIGATORY(posy == 0);

    sleep(2);

    OBLIGATORY(mdw_player_stop(player, NULL) == TRUE);

}
END_TEST


START_TEST( test_raise_and_lower ) {
     /* preambule */
    mdw_player_set_audio_pid(player, AUDIO_PID_2);
    mdw_player_set_video_pid(player, VIDEO_PID_2);

    OBLIGATORY(mdw_player_play(player, NULL) == TRUE);
    /* preambule end */

    sleep(1);
    mdw_player_raise_to_top(player, NULL);
    sleep(1);
    mdw_player_lower_to_bottom(player, NULL);
    sleep(1);

    OBLIGATORY(mdw_player_stop(player, NULL) == TRUE);

}
END_TEST

/****************************************************************/
static Suite *player_suite(void) 
{
    Suite *s = suite_create("Class MdwDefaultPlayer");

    /* tc_new */
    TCase *tc_new = tcase_create("Constructors");
    tcase_add_test(tc_new, test_new_g_object);
    tcase_add_test(tc_new, test_new_func);
    tcase_add_test(tc_new, test_get_set_demux);

    tcase_add_checked_fixture(tc_new, new_setup, new_teardown);

    suite_add_tcase(s, tc_new);
    /* tc_interface */
    TCase *tc_interface = tcase_create("Interface use of class");
    tcase_add_test(tc_interface, test_pids);
    tcase_add_test(tc_interface, test_play_stop);
    tcase_add_test(tc_interface, test_change_pids);
    tcase_add_test(tc_interface, test_change_size_and_pos_while_playing);
    tcase_add_test(tc_interface, test_raise_and_lower);

    tcase_add_checked_fixture(tc_interface, interface_setup, interface_teardown);
    tcase_set_timeout(tc_interface, 30);
    suite_add_tcase(s, tc_interface);

    return s;
}

int main(void) {
    int nf;
    Suite *s = player_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_VERBOSE);
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

