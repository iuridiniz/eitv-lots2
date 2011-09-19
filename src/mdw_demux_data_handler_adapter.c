/*@COPYRIGHT@*/
#include "mdw_demux_data_handler_adapter.h"

/********************/
/* local prototypes */
/********************/
/* interface methods */
static void _iface_mdw_demux_data_handler_init (gpointer g_iface, gpointer data);
static gboolean _iface_mdw_demux_data_handler_new_data (
    MdwDemuxDataHandler *self, 
    const guint16 pid, 
    const gsize size, 
    const guint8* data);

/* define my Object */
G_DEFINE_TYPE_WITH_CODE(
    MdwDemuxDataHandlerAdapter,
    mdw_demux_data_handler_adapter,
    G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE(MDW_TYPE_DEMUX_DATA_HANDLER,
        _iface_mdw_demux_data_handler_init)
)



/** 
 * Overrides methods of MdwDemuxDataHandlerInterface 
 */
static void 
_iface_mdw_demux_data_handler_init (gpointer g_iface, gpointer data) 
{
    //new_data
    ((MdwDemuxDataHandlerInterface *) g_iface)->new_data = _iface_mdw_demux_data_handler_new_data;

}


static gboolean _stub_new_data(
    MdwDemuxDataHandler *self, 
    const guint16 pid, 
    const gsize size, 
    const guint8* data) 
{
    return FALSE;
}
/**
 * called when new section arrives.
 */
static gboolean
_iface_mdw_demux_data_handler_new_data (
    MdwDemuxDataHandler *self, 
    const guint16 pid, 
    const gsize size, 
    const guint8* data) 
{
    return MDW_DEMUX_DATA_HANDLER_ADAPTER(self)->new_data(self, pid, size, data);
}

/**
 * initilize/override parent class methods, create signals, properties.
 */

static void
mdw_demux_data_handler_adapter_class_init (MdwDemuxDataHandlerAdapterClass *g_class) 
{
    //void
}

static void
mdw_demux_data_handler_adapter_init(MdwDemuxDataHandlerAdapter *self) 
{
    //self->new_data = (gboolean (*)(MdwDemuxDataHandler*, const guint16, const gsize, const guint8*)) (_stub_new_data);
    self->new_data = _stub_new_data;
}

/* Constructors */
MdwDemuxDataHandlerAdapter* 
mdw_demux_data_handler_adapter_new() {
    return (MdwDemuxDataHandlerAdapter*) g_object_new(MDW_TYPE_DEMUX_DATA_HANDLER_ADAPTER, NULL);
}

