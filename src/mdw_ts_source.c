#include  "mdw_ts_source.h"

static void
mdw_ts_source_base_init (gpointer g_class) {
    static gboolean initialized = FALSE;

    if (G_UNLIKELY(initialized == FALSE)) {
        /* signals */
        initialized = TRUE;
    }
}


GType 
mdw_ts_source_get_type(void) {
    static GType type = 0;

    if (G_UNLIKELY(type == 0)) {
        static const GTypeInfo info = {
            .class_size = sizeof(MdwTsSourceInterface),
            .base_init = mdw_ts_source_base_init,
            .base_finalize = NULL,
            .class_init = NULL,
            .class_finalize = NULL, 
            .class_data = NULL,
            .instance_size = 0,
            .n_preallocs = 0,
            .instance_init = NULL,
            .value_table = NULL
        };
        type = g_type_register_static(G_TYPE_INTERFACE, "MdwTsSource", &info, 0);
    }
    return type;
}


gssize 
mdw_ts_source_read(MdwTsSource *self, guint8 *buffer, const gssize count) 
{
    g_return_val_if_fail(self != NULL, -1);
    g_return_val_if_fail(MDW_TS_SOURCE_GET_INTERFACE(self)->read != NULL, -1);
    return MDW_TS_SOURCE_GET_INTERFACE (self)->read(self, buffer, count);
}


