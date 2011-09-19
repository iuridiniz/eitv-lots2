#include <glib.h>
#include <glib/gprintf.h>

#include <stdlib.h>
#include <string.h>

#include "mdw_lots_dbus.h"

#define DBUS_SERVICE "com.eitv.lots"
#define DBUS_REMOTE_OBJECT "/com/eitv/lots"
#define DBUS_REMOTE_SOURCE_INTERFACE "com.eitv.lots.source"
#define DBUS_REMOTE_PLAYER_INTERFACE "com.eitv.lots.player"
#define DBUS_REMOTE_DEMUXER_INTERFACE "com.eitv.lots.demuxer"


#define CONVERT_8_TO_16(x,y) \
    (((x) << 8) | (y)) 

#define CHECK_ERROR(error) \
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
    }

struct my_data_t {
    GMainLoop* main_loop;
    gint n;
    gboolean to_exit;
};
static void 
pat_pmt_handler(DBusGProxy* demuxer, guint pid, const GArray* array, gpointer user_data) 
{
    struct my_data_t* my_data = (struct my_data_t *) user_data; 
    gsize size = array->len;
    guint8* buffer = (guint8*) array->data;

    /* del watch */
    GError* error = NULL;
    gboolean ret_val = com_eitv_lots_demuxer_del_watch(demuxer, pid, &error);
    CHECK_ERROR(error);
    g_assert(ret_val != FALSE);

    GString* s = g_string_new("");

    if (pid == 0) {
        /* PAT */
        g_string_append_printf(s, "Number of programs: %u\n", (guint) ((size - 12)/4));
        guint pids[size - 12/4];
        guint n = 0;
        for(int i = 8; i < size - 4; i += 4) {
            guint8 program_number = CONVERT_8_TO_16(buffer[i], buffer[i+1]);
            //guint8 reserved = buffer[i+2] >> 5;
            guint16 pid = CONVERT_8_TO_16(buffer[i+2], buffer[i+3]) & 0x1fff;
            g_string_append_printf(s, "  program number: %u\n", program_number);
            //g_string_append_printf(s, "    reserved: %u\n", reserved);
            if (program_number == 0) {
                g_string_append_printf(s, "    NETWORK PID: %u\n", pid);
            } else {
                g_string_append_printf(s, "    PID: %u\n", pid);
                pids[n++] = pid;
            }
        }
        /* Add handlers for PMT pids */
        g_atomic_int_set(&my_data->n, n); 
        for(int i = n - 1; i >= 0; i--) { 
            ret_val = com_eitv_lots_demuxer_add_watch(demuxer, pids[i], &error);
            CHECK_ERROR(error);
            g_assert(ret_val != FALSE);
        }
        if (n == 0) {
            /* no PMT */
            my_data->to_exit = TRUE;
        }
    } else {
        /* PMT */
        g_string_append_printf(s, "PMT handler for pid %u\n", pid); 

        guint16 info_lenght = CONVERT_8_TO_16(buffer[10], buffer[11]) & 0x0fff;
        g_assert(12 + info_lenght < size);
        for(int i = 12 + info_lenght; i < size - 4;) {
            guint8 stream_type = buffer[i];
            //guint8 reserved1 = buffer[i+1] >> 5;
            guint16 pid = CONVERT_8_TO_16(buffer[i+1], buffer[i+2]) & 0x1fff;
            //guint8 reserved2 = buffer[i+3] >> 4;
            guint16 es_info_lenght = CONVERT_8_TO_16(buffer[i+3], buffer[i+4]) & 0x0fff;

            g_string_append_printf(s, "  stream type: %u 0x%02X\n", stream_type, stream_type);
            //g_string_append_printf(s, "    reserved: %u\n", reserved1);
            g_string_append_printf(s, "    PID: %u 0x%04X\n", pid, pid);
            //g_string_append_printf(s, "    reserved: %u\n", reserved2);
            //g_string_append_printf(s, "    ES info lenght: %u\n", es_info_lenght);
            
            /* sanity check */
            g_assert(i + 5 + es_info_lenght < size);

            i += 5 + es_info_lenght;
        }

        gint old_n = g_atomic_int_exchange_and_add(&my_data->n, -1);
        if (old_n <= 1) {
            /* I was the last pmt handler */
            my_data->to_exit = TRUE;
        }
    }
    
    /* flush */
    g_printf("%s", s->str);
    g_string_free(s, TRUE);

    if(my_data->to_exit == TRUE) {
        g_main_loop_quit(my_data->main_loop);
    }
}

int
main(int argc, char** argv)
{
    struct my_data_t my_data;

    DBusGConnection *connection = NULL;
    DBusGProxy *source = NULL;
    DBusGProxy *player = NULL;
    DBusGProxy *demuxer = NULL;

    GMainLoop *main_loop = NULL;

    GError *error = NULL;
    
    g_type_init();

    /* main loop */
    main_loop = g_main_loop_new(NULL, FALSE);

    /* get a connection to bus */
    connection = dbus_g_bus_get(DBUS_BUS_SESSION, NULL);
    g_assert(connection != NULL);

    /* get interfaces for the remote object */
    source = dbus_g_proxy_new_for_name (connection, 
        DBUS_SERVICE, 
        DBUS_REMOTE_OBJECT, 
        DBUS_REMOTE_SOURCE_INTERFACE
    );
    g_assert(source != NULL);

    player = dbus_g_proxy_new_for_name (connection, 
        DBUS_SERVICE,
        DBUS_REMOTE_OBJECT, 
        DBUS_REMOTE_PLAYER_INTERFACE
    );
    g_assert(player != NULL);

    demuxer = dbus_g_proxy_new_for_name (connection, 
        DBUS_SERVICE, 
        DBUS_REMOTE_OBJECT, 
        DBUS_REMOTE_DEMUXER_INTERFACE
    );
    g_assert(demuxer != NULL);

    /* register marshallers and signals */
    dbus_g_object_register_marshaller(g_cclosure_marshal_VOID__UINT_POINTER, 
        G_TYPE_NONE,
        G_TYPE_UINT, DBUS_TYPE_G_UCHAR_ARRAY,
        G_TYPE_INVALID
    );
    dbus_g_proxy_add_signal (demuxer, 
        "new_payload", 
        G_TYPE_UINT, DBUS_TYPE_G_UCHAR_ARRAY, 
        G_TYPE_INVALID
    );

    /* connect to signal */
    my_data.n = 0;
    my_data.main_loop = main_loop;
    my_data.to_exit = FALSE;
    dbus_g_proxy_connect_signal(demuxer, "new_payload", 
        G_CALLBACK(pat_pmt_handler),
        &my_data,
        NULL
    );

    /* play if not playing */
    gboolean is_playing = FALSE;
    gboolean ret_val;
    ret_val = com_eitv_lots_player_is_playing(player, &is_playing, &error);
    CHECK_ERROR(error);
    g_assert(ret_val != FALSE);
    if (!is_playing) {
        g_assert(argc >= 5);

        char uri[1024];
        guint pid_video, pid_audio, pid_pcr;

        strcpy(uri, "file://");
        strncat(uri, argv[1], 1017);
        pid_video = atoi(argv[2]);
        pid_audio = atoi(argv[3]);
        pid_pcr = atoi(argv[4]);

        ret_val = com_eitv_lots_source_set_uri(source, uri, &error);
        CHECK_ERROR(error);
        g_assert(ret_val == TRUE);

        ret_val = com_eitv_lots_player_play_with_pids(player, pid_video, pid_audio, pid_pcr, &error);
        CHECK_ERROR(error);
        g_assert(ret_val == TRUE);
    }
    
    /* add watch */
    ret_val = com_eitv_lots_demuxer_add_watch(demuxer, 0, &error);
    CHECK_ERROR(error);
    g_assert(ret_val == TRUE);

    /* run main loop */
    g_main_loop_run(main_loop);

    g_object_unref(source);
    g_object_unref(demuxer);
    g_object_unref(player);
    dbus_g_connection_unref(connection);


    return 0;
}
