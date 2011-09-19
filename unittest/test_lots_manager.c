#include <glib.h>
#include <check.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mdw_lots_dbus.h"

#define NOT_NULL(x) OBLIGATORY((x) != NULL)

#define OBLIGATORY(x) \
    {   \
        if (!(x)) { \
            fail(#x); \
        } \
    } while (0)

#define CHECK(error) \
    { \
        if ((error) != NULL) { \
            if (error->domain == DBUS_GERROR && error->code == DBUS_GERROR_REMOTE_EXCEPTION) { \
                g_warning("error != NULL, message:%s:%s", dbus_g_error_get_name(error), (error)->message); \
            } else { \
                g_warning("error != NULL, message:-:%s", (error)->message); \
            } \
            g_error_free((error)); \
            (error) = NULL; \
        } \
    } while(0)

DBusGConnection *connection = NULL;
DBusGProxy *source = NULL;
DBusGProxy *player = NULL;
DBusGProxy *demux = NULL;

#define DBUS_SERVICE "com.eitv.lots"
#define DBUS_REMOTE_OBJECT "/com/eitv/lots"
#define DBUS_REMOTE_SOURCE_INTERFACE "com.eitv.lots.source"
#define DBUS_REMOTE_PLAYER_INTERFACE "com.eitv.lots.player"
#define DBUS_REMOTE_DEMUX_INTERFACE "com.eitv.lots.demuxer"

void setup() {
    g_type_init();

    connection = dbus_g_bus_get(DBUS_BUS_SESSION, NULL);
    g_assert(connection != NULL);

    source = dbus_g_proxy_new_for_name (connection, 
        DBUS_SERVICE,
        DBUS_REMOTE_OBJECT, 
        DBUS_REMOTE_SOURCE_INTERFACE);
    player = dbus_g_proxy_new_for_name(connection, 
        DBUS_SERVICE,
        DBUS_REMOTE_OBJECT,
        DBUS_REMOTE_PLAYER_INTERFACE);
    demux = dbus_g_proxy_new_for_name(connection, 
        DBUS_SERVICE,
        DBUS_REMOTE_OBJECT,
        DBUS_REMOTE_DEMUX_INTERFACE);

}

void teardown() {

    g_object_unref(source);
    source = NULL;
    g_object_unref(player);
    player = NULL;
    g_object_unref(demux);
    demux = NULL;

    dbus_g_connection_unref(connection);
    connection = NULL;
}

#ifndef TS_FILE
#define TS_FILE "sample.ts"
#endif

#ifndef VIDEO_PID_1
#define VIDEO_PID_1 1280
#endif

#ifndef VIDEO_PID_2
#define VIDEO_PID_2 1281
#endif

#ifndef AUDIO_PID_1
#define AUDIO_PID_1 2560
#endif

#ifndef AUDIO_PID_2
#define AUDIO_PID_2 2561
#endif

START_TEST(test_source_get_set_uri) {

    GError* error = NULL;
    /* set a valid uri */
    gchar* uri = "file://./"TS_FILE;
    gchar* ret_uri;

    OBLIGATORY(com_eitv_lots_source_set_uri(source, uri, &error) == TRUE);
    CHECK(error);
    OBLIGATORY(com_eitv_lots_source_get_uri(source, &ret_uri, &error) == TRUE);
    CHECK(error);
    OBLIGATORY(strcmp(uri, ret_uri) == 0);
    
    g_free(ret_uri);
}
END_TEST

gchar* saved_uri = NULL;
guint saved_audio_pid, saved_pcr_pid, saved_video_pid;

static void aux_save_player_context() {
    gboolean is_playing;
    GError* error = NULL;
    OBLIGATORY(com_eitv_lots_player_is_playing(player, &is_playing, &error) == TRUE);
    CHECK(error);
    if (is_playing) {
        /* stop playing to test */
        OBLIGATORY(com_eitv_lots_source_get_uri(source, &saved_uri, &error) == TRUE);
        CHECK(error);
        NOT_NULL(saved_uri);

        OBLIGATORY(com_eitv_lots_player_get_audio_pid(player, &saved_audio_pid, &error) == TRUE);
        CHECK(error);

        OBLIGATORY(com_eitv_lots_player_get_video_pid(player, &saved_video_pid, &error) == TRUE);
        CHECK(error);

        OBLIGATORY(com_eitv_lots_player_get_pcr_pid(player, &saved_pcr_pid, &error) == TRUE);
        CHECK(error);

        OBLIGATORY(com_eitv_lots_player_stop(player, &error) == TRUE);
        CHECK(error);

        OBLIGATORY(com_eitv_lots_player_is_playing(player, &is_playing, &error) == TRUE);
        CHECK(error);

        OBLIGATORY(is_playing == FALSE);
    }

}

static void aux_restore_player_context() {
    GError* error = NULL;
    gboolean is_playing = FALSE;
    if (saved_uri) {
        /* restart playing */
        OBLIGATORY(com_eitv_lots_source_set_uri(source, saved_uri, &error) == TRUE);
        CHECK(error);

        OBLIGATORY(com_eitv_lots_player_set_audio_pid(player, saved_audio_pid, &error) == TRUE);
        CHECK(error);

        OBLIGATORY(com_eitv_lots_player_set_video_pid(player, saved_video_pid, &error) == TRUE);
        CHECK(error);

        OBLIGATORY(com_eitv_lots_player_set_pcr_pid(player, saved_pcr_pid, &error) == TRUE);
        CHECK(error);

        OBLIGATORY(com_eitv_lots_player_play(player, &error) == TRUE);
        CHECK(error);

        OBLIGATORY(com_eitv_lots_player_is_playing(player, &is_playing, &error) == TRUE);
        CHECK(error);

        OBLIGATORY(is_playing == TRUE);
        
        g_free(saved_uri);
        saved_uri = NULL;
    }


}

START_TEST(test_player_play_stop) {
    
    aux_save_player_context();

    GError* error = NULL;
    gchar* uri = "file://./"TS_FILE;
    gboolean is_playing;

    guint audio_pid, video_pid, pcr_pid;


    /* set uri (must stop the current playing) */
    OBLIGATORY(com_eitv_lots_source_set_uri(source, uri, &error) == TRUE);
    CHECK(error);

    OBLIGATORY(com_eitv_lots_player_get_audio_pid(player, &audio_pid, &error) == TRUE);
    CHECK(error);

    OBLIGATORY(com_eitv_lots_player_get_video_pid(player, &video_pid, &error) == TRUE);
    CHECK(error);

    OBLIGATORY(com_eitv_lots_player_get_pcr_pid(player, &pcr_pid, &error) == TRUE);
    CHECK(error);

    OBLIGATORY(audio_pid == 0);
    OBLIGATORY(video_pid == 0);
    OBLIGATORY(pcr_pid == 0);

    /* pids not setted (the uri has been changed) */
    //OBLIGATORY(com_eitv_lots_player_play(player, &error) == FALSE);
    //CHECK(error);

    /* play with pids */
    OBLIGATORY(com_eitv_lots_player_play_with_pids(player, VIDEO_PID_1, AUDIO_PID_1, VIDEO_PID_1, &error) == TRUE); 
    CHECK(error);

    OBLIGATORY(com_eitv_lots_player_get_audio_pid(player, &audio_pid, &error) == TRUE);
    CHECK(error);

    OBLIGATORY(com_eitv_lots_player_get_video_pid(player, &video_pid, &error) == TRUE);
    CHECK(error);

    OBLIGATORY(com_eitv_lots_player_get_pcr_pid(player, &pcr_pid, &error) == TRUE);
    CHECK(error);

    OBLIGATORY(com_eitv_lots_player_is_playing(player, &is_playing, &error) == TRUE);
    CHECK(error);
   
    OBLIGATORY(is_playing == TRUE);
    OBLIGATORY(audio_pid == AUDIO_PID_1);
    OBLIGATORY(video_pid == VIDEO_PID_1);
    OBLIGATORY(pcr_pid == VIDEO_PID_1);

    /* TODO: stop video */
    /* TODO: change to a invalid file but valid uri */
    /* TODO: change the pids */


    aux_restore_player_context();
}
END_TEST

START_TEST(test_player_set_pos_and_size) {
    const guint max_width = 640;
    const guint max_height = 480;
    GError* error = NULL;

    guint saved_pos_x, saved_pos_y;
    guint saved_width, saved_height;

    guint ret_width, ret_height;
    guint ret_pos_x, ret_pos_y;


    /* saving values */
    OBLIGATORY(com_eitv_lots_player_get_pos(player, &saved_pos_x, &saved_pos_y, &error));
    CHECK(error);
    OBLIGATORY(com_eitv_lots_player_get_size(player, &saved_width, &saved_height, &error));
    CHECK(error);

    OBLIGATORY(com_eitv_lots_player_get_max_size(player, &ret_width, &ret_height, &error));
    CHECK(error);

    OBLIGATORY(max_width == ret_width);
    OBLIGATORY(max_height == ret_height);

    OBLIGATORY(com_eitv_lots_player_set_pos(player, 200, 300, &error));
    CHECK(error);

    OBLIGATORY(com_eitv_lots_player_get_pos(player, &ret_pos_x, &ret_pos_y, &error));
    CHECK(error);

    OBLIGATORY(ret_pos_x == 200);
    OBLIGATORY(ret_pos_y == 300);

    OBLIGATORY(com_eitv_lots_player_set_size(player, 320, 200, &error));
    CHECK(error);

    OBLIGATORY(com_eitv_lots_player_get_size(player, &ret_width, &ret_height, &error));
    CHECK(error);

    OBLIGATORY(ret_width == 320);
    OBLIGATORY(ret_height == 200);

    OBLIGATORY(com_eitv_lots_player_set_size(player, max_width + 1, max_height + 1, &error) == FALSE);
    CHECK(error);

    OBLIGATORY(com_eitv_lots_player_get_size(player, &ret_width, &ret_height, &error));
    CHECK(error);

    OBLIGATORY(ret_width == 320);
    OBLIGATORY(ret_height == 200);

    /* restore old values */
    OBLIGATORY(com_eitv_lots_player_set_pos(player, saved_pos_x, saved_pos_y, &error));
    CHECK(error);
    OBLIGATORY(com_eitv_lots_player_set_size(player, saved_width, saved_height, &error));
    CHECK(error);

} 
END_TEST

#define SECTION_PID 39
gboolean loop_quit(gpointer data) {
    GMainLoop* loop = (GMainLoop*) data;
    g_main_loop_quit(loop);
    return FALSE;
}

static guint section_callback_count = 0;
static void 
section_callback(DBusGProxy* demux, guint pid, const GArray* array, gpointer user_data) 
{
    section_callback_count++;
    guint size = array->len;
    guchar* data = (guchar*) array->data;
    //g_debug("Section callback, pid %u", pid);
    //g_debug("  Data lenght %u", size);
    OBLIGATORY(size == ((data[1] & 0xf) << 8|data[2]) + 3);

}

static void watch_up_callback(DBusGProxy* demux, guint pid, gpointer user_data) {
    //g_debug("Watch up callback, pid %u", pid);
}

static void watch_down_callback(DBusGProxy* demux, guint pid, gpointer user_data) {
    //g_debug("Watch down callback, pid %u", pid);
}


START_TEST(test_section_handler)
{
    GError* error = NULL;
    gchar* uri = "file://./"TS_FILE;
    guint def_count = 0;
    guint count = 0;

    guint cb_count;

    GMainLoop* loop;

    /* signals */
    dbus_g_object_register_marshaller(g_cclosure_marshal_VOID__UINT_POINTER,
        G_TYPE_NONE,
        G_TYPE_UINT, DBUS_TYPE_G_UCHAR_ARRAY,
        G_TYPE_INVALID
    );
    dbus_g_object_register_marshaller(g_cclosure_marshal_VOID__UINT,
        G_TYPE_NONE,
        G_TYPE_UINT,
        G_TYPE_INVALID
    );
    dbus_g_proxy_add_signal (demux, "new_payload",
	    G_TYPE_UINT,
		DBUS_TYPE_G_UCHAR_ARRAY,
	    G_TYPE_INVALID
    );
    dbus_g_proxy_add_signal(demux, "watch_pid_up", 
        G_TYPE_UINT,
        G_TYPE_INVALID
    );
    dbus_g_proxy_add_signal(demux, "watch_pid_down", 
        G_TYPE_UINT,
        G_TYPE_INVALID
    );

    dbus_g_proxy_connect_signal(demux, "new_payload", 
        G_CALLBACK(section_callback),
        NULL,
        NULL
    );
    dbus_g_proxy_connect_signal(demux, "watch_pid_up",
        G_CALLBACK(watch_up_callback),
        NULL,
        NULL
    );
    dbus_g_proxy_connect_signal(demux, "watch_pid_down",
        G_CALLBACK(watch_down_callback),
        NULL,
        NULL
    );

    aux_save_player_context();
    
    OBLIGATORY(com_eitv_lots_source_set_uri(source, uri, &error) == TRUE);
    CHECK(error);

    OBLIGATORY(com_eitv_lots_player_play_with_pids(player, VIDEO_PID_2, AUDIO_PID_2, VIDEO_PID_2, &error) == TRUE);
    CHECK(error);

    OBLIGATORY(com_eitv_lots_demuxer_count_watch(demux, SECTION_PID, &count, &error) == TRUE);
    CHECK(error);
    def_count = count;
    
    OBLIGATORY(com_eitv_lots_demuxer_add_watch(demux, SECTION_PID, &error) == TRUE);
    CHECK(error);

    OBLIGATORY(com_eitv_lots_demuxer_count_watch(demux, SECTION_PID, &count, &error) == TRUE);
    CHECK(error);

    OBLIGATORY(def_count + 1 == count);

    cb_count = section_callback_count;
    /* g_main_loop 3 seconds */
    loop = g_main_loop_new(NULL, FALSE);
    g_timeout_add(3000, loop_quit, loop);
    g_main_loop_run(loop);
    g_main_loop_unref(loop);
    OBLIGATORY(section_callback_count > cb_count);

    OBLIGATORY(com_eitv_lots_demuxer_del_watch(demux, SECTION_PID, &error) == TRUE);
    CHECK(error);

    OBLIGATORY(com_eitv_lots_demuxer_count_watch(demux, SECTION_PID, &count, &error) == TRUE);
    CHECK(error);

    OBLIGATORY(def_count == count);



    
    aux_restore_player_context();

}
END_TEST

static Suite *manager_suite(void) 
{
    Suite *s = suite_create("DBus testing");

    /* tc_dbus */
    TCase *tc_dbus = tcase_create("DBus testsing");
    tcase_add_test(tc_dbus, test_source_get_set_uri);
    tcase_add_test(tc_dbus, test_player_play_stop);
    tcase_add_test(tc_dbus, test_player_set_pos_and_size);
    tcase_add_test(tc_dbus, test_section_handler);

    tcase_add_checked_fixture(tc_dbus, setup, teardown);
    tcase_set_timeout(tc_dbus, 10);

    suite_add_tcase(s, tc_dbus);

    return s;
}

int main(void) {
    int nf;
    Suite *s = manager_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_VERBOSE);
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


