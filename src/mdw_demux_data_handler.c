

#include "mdw_demux_data_handler.h"


static void 
mdw_demux_data_handler_base_init (gpointer g_class) {
    static gboolean initialized = FALSE;

    if (G_UNLIKELY(initialized == FALSE)) {
        /* singnals associeted */
        initialized = TRUE;
    }

}


GType
mdw_demux_data_handler_get_type(void) {
    static GType type = 0;
    if (G_UNLIKELY(type == 0)) {
        static const GTypeInfo info = {
            .class_size = sizeof(MdwDemuxDataHandlerInterface),
            .base_init = mdw_demux_data_handler_base_init,
            .base_finalize = NULL,
            .class_init = NULL,
            .class_finalize = NULL, 
            .class_data = NULL,
            .instance_size = 0,
            .n_preallocs = 0,
            .instance_init = NULL,
            .value_table = NULL
        };
        type = g_type_register_static(G_TYPE_INTERFACE, "MdwDemuxDataHandlerInterface", &info, 0);
    }
    return type;
}



gboolean 
mdw_demux_data_handler_new_data (MdwDemuxDataHandler *self, 
        const guint16 pid, const gsize size, const guint8* data) {
    
    return MDW_DEMUX_DATA_HANDLER_GET_INTERFACE 
        (self)->new_data(self, pid, size, data);

}

