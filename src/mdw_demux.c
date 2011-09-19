/*@COPYRIGHT@*/
#include  "mdw_demux.h"


static void
mdw_demux_base_init (gpointer g_class) {
    static gboolean initialized = FALSE;

    if (G_UNLIKELY(initialized == FALSE)) {
        /* signals */
        initialized = TRUE;
    }
}


GType 
mdw_demux_get_type(void) {
    static GType type = 0;

    if (G_UNLIKELY(type == 0)) {
        static const GTypeInfo info = {
            .class_size = sizeof(MdwDemuxInterface),
            .base_init = mdw_demux_base_init,
            .base_finalize = NULL,
            .class_init = NULL,
            .class_finalize = NULL, 
            .class_data = NULL,
            .instance_size = 0,
            .n_preallocs = 0,
            .instance_init = NULL,
            .value_table = NULL
        };
        type = g_type_register_static(G_TYPE_INTERFACE, "MdwDemux", &info, 0);
    }
    return type;
}



gboolean 
mdw_demux_add_section_handler(MdwDemux *self, MdwDemuxDataHandler *handler, guint16 pid) 
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(MDW_DEMUX_GET_INTERFACE(self)->add_section_handler != NULL, FALSE);

    return MDW_DEMUX_GET_INTERFACE (self)->add_section_handler(self, handler, pid);
}

gboolean 
mdw_demux_add_pes_handler(MdwDemux *self, MdwDemuxDataHandler *handler, guint16 pid) 
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(MDW_DEMUX_GET_INTERFACE(self)->add_pes_handler != NULL, FALSE);

    return MDW_DEMUX_GET_INTERFACE (self)->add_pes_handler(self, handler, pid);
}

gboolean 
mdw_demux_del_section_handler(MdwDemux *self, MdwDemuxDataHandler *handler, guint16 pid) 
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(MDW_DEMUX_GET_INTERFACE(self)->del_section_handler != NULL, FALSE);

    return MDW_DEMUX_GET_INTERFACE (self)->del_section_handler(self, handler, pid);
}

gboolean 
mdw_demux_del_pes_handler(MdwDemux *self, MdwDemuxDataHandler *handler, guint16 pid) 
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(MDW_DEMUX_GET_INTERFACE(self)->del_pes_handler != NULL, FALSE);

    return MDW_DEMUX_GET_INTERFACE (self)->del_pes_handler(self, handler, pid);
}


gboolean 
mdw_demux_set_ts_source(MdwDemux *self, MdwTsSource* source) 
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(MDW_DEMUX_GET_INTERFACE(self)->set_ts_source != NULL, FALSE);

    return MDW_DEMUX_GET_INTERFACE(self)->set_ts_source(self, source);
}

gboolean
mdw_demux_start(MdwDemux *self) 
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(MDW_DEMUX_GET_INTERFACE(self)->start != NULL, FALSE);

    return MDW_DEMUX_GET_INTERFACE(self)->start(self);
}

gboolean 
mdw_demux_stop(MdwDemux *self) 
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(MDW_DEMUX_GET_INTERFACE(self)->stop != NULL, FALSE);

    return MDW_DEMUX_GET_INTERFACE(self)->stop(self);
}

/*void
mdw_demux_step(MdwDemux *self) 
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(MDW_DEMUX_GET_INTERFACE(self)->step != NULL, FALSE);

    return MDW_DEMUX_GET_INTERFACE(self)->step(self);
}*/
