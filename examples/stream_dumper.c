#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include "mdw_lots.h"
#include "mdw_util.h"
#include "mdw_demux_data_handler_adapter.h"


struct my_data_t {
    int fd;
    guint64 i;
    guint64 max;
};

gboolean pes_cb(
    MdwDemuxDataHandler *iface, 
    const guint16 pid, 
    const gsize size, 
    const guint8* buffer)
{
    struct my_data_t* my_data = MDW_DEMUX_DATA_HANDLER_ADAPTER(iface)->data;
    printf("Interation: %llu, size: %u\n", (long long) my_data->i++, (guint) size);
    int ret=0;
    if (my_data->fd != -1) {
        ret = write(my_data->fd, buffer, size);
    } else {
        printf("  PES length: %u\n", (buffer[4] << 8) | buffer[5]);
        printf("  Stream ID: %u (0x%x)\n", buffer[3], buffer[3]);
    }
    if (ret < 0) {
        perror("While writing");
    }
    if (my_data->max > 0 && my_data->i >= my_data->max) {
        return FALSE;
    }
    return TRUE;
}

int main(int argc, char** argv) {
    

    g_assert(argc == 4 || argc == 5);

    g_type_init();

    char uri[1024];
    strcpy(uri, "file://");
    strncat(uri, argv[1], 1017);

    char* out_filename = argv[2];
    guint16 pid = atoi(argv[3]);
    guint64 max = 0;

    if (argc >= 5) {
        max = strtoull(argv[4], NULL, 10);
    }

    struct my_data_t my_data;

    MdwDemux *demux = mdw_get_most_suitable_demux();
    MdwTsSource *source = mdw_get_most_suitable_ts_source(uri);

    MdwDemuxDataHandlerAdapter *pes_handler = mdw_demux_data_handler_adapter_new();

    g_assert(mdw_demux_set_ts_source(demux, source) == TRUE);
    if (strncmp(out_filename, "-", 1) == 0) {
        my_data.fd = -1;
    } else {
        g_assert((my_data.fd = open(out_filename, O_EXCL| O_CREAT| O_WRONLY, 0666)) >= 0);
    }
    my_data.i = 0;
    my_data.max = max;

    pes_handler->data = &my_data;
    pes_handler->new_data = pes_cb;

    mdw_demux_add_pes_handler(demux, MDW_DEMUX_DATA_HANDLER(pes_handler), pid);

    /* FIXME: use g_main_loop, the old interface doesn't work more 'cause is OLD :-P */
    mdw_demux_start(demux);

    getchar();

    mdw_demux_stop(demux);

    close(my_data.fd);

    g_object_unref(pes_handler);
    g_object_unref(demux);
    g_object_unref(source);

    
    return 0;
}

