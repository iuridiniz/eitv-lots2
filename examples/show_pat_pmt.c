#include <glib/gprintf.h>
#include <unistd.h>

#include "config.h"

#include "mdw_lots.h"
#include "mdw_util.h"

#include "mdw_demux_data_handler_adapter.h"


#define CONVERT_8_TO_16(x,y) \
    (((x) << 8) | (y)) 


struct my_data_t {
    MdwDemux* demux;
    gboolean to_exit;
    gint n;
};

gboolean read_section (
    MdwDemuxDataHandler *iface, 
    const guint16 pid, 
    const gsize size, 
    const guint8* buffer) 
{
    MdwDemuxDataHandlerAdapter* self = MDW_DEMUX_DATA_HANDLER_ADAPTER(iface);
    struct my_data_t* my_data = (struct my_data_t *) self->data; 
    //printf("Pat readed\n");
    /* read programs pids */
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
            mdw_demux_add_section_handler(my_data->demux, MDW_DEMUX_DATA_HANDLER(self), pids[i]);
        }
        if (n == 0) {
            /* no PMT */
            my_data->to_exit = 1;
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
            my_data->to_exit = 1;
        }
    }
    
    /* flush */
    g_printf("%s", s->str);
    g_string_free(s, TRUE);

    return FALSE;
    
}

int 
main(int argc, char** argv)
{
    g_type_init();
    g_assert(argc == 2);

    /* type to hold my data throung threads */
    struct my_data_t my_data;

    /* new demux */
    my_data.demux = mdw_get_most_suitable_demux();
    my_data.to_exit = 0;

    /* new handler */
    MdwDemuxDataHandlerAdapter* section_handler = mdw_demux_data_handler_adapter_new();

    /* setup handler */
    section_handler->new_data = read_section; /* function to call on new data */
    section_handler->data = &my_data; /* pointer to some structure */

    /* ts provider */
    char uri[1024];
    strcpy(uri, "file://");
    strncat(uri, argv[1], 1017);

    MdwTsSource* source = MDW_TS_SOURCE(mdw_get_most_suitable_ts_source(uri));

    /* install provider on demux */
    mdw_demux_set_ts_source(my_data.demux, source);

    /* add my handler */
    mdw_demux_add_section_handler(my_data.demux, MDW_DEMUX_DATA_HANDLER(section_handler), 0);

    /* start demuxing process */
    mdw_demux_start(my_data.demux);
    
    /* FIXME: use g_main_loop, the old interface doesn't work more 'cause is OLD :-P */
    /* wait */
    while(my_data.to_exit == 0) {
        usleep(10);
    }
    mdw_demux_stop(my_data.demux);

    g_object_unref(my_data.demux);
    g_object_unref(source);
    g_object_unref(section_handler);

    return 0;
}
