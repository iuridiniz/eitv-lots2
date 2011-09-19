/*@COPYRIGHT@*/

#ifndef MDW_DEMUX_DATA_HANDLER_ADAPTER_H
#define MDW_DEMUX_DATA_HANDLER_ADAPTER_H


#include <glib-object.h>
#include "mdw_demux_data_handler.h"


G_BEGIN_DECLS

/* preambule init */
#define MDW_TYPE_DEMUX_DATA_HANDLER_ADAPTER   (mdw_demux_data_handler_adapter_get_type())
#define MDW_DEMUX_DATA_HANDLER_ADAPTER(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj), MDW_TYPE_DEMUX_DATA_HANDLER_ADAPTER, MdwDemuxDataHandlerAdapter))
#define MDW_DEMUX_DATA_HANDLER_ADAPTER_CLASS(vtable)   (G_TYPE_CHECK_CLASS_CAST((vtable), MDW_TYPE_DEMUX_DATA_HANDLER_ADAPTER, MdwDemuxDataHandlerAdapterClass))
#define MDW_IS_DEMUX_DATA_HANDLER_ADAPTER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), MDW_TYPE_DEMUX_DATA_HANDLER_ADAPTER))
#define MDW_IS_DEMUX_DATA_HANDLER_ADAPTER_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE((vtable), MDW_TYPE_DEMUX_DATA_HANDLER_ADAPTER))

#define MDW_DEMUX_DATA_HANDLER_ADAPTER_GET_CLASS(inst) (G_TYPE_INSTANCE_GET_CLASS((inst), MDW_TYPE_DEMUX_DATA_HANDLER_ADAPTER, MdwDemuxDataHandlerAdapterClass))


/* preambule end */

typedef struct _MdwDemuxDataHandlerAdapter MdwDemuxDataHandlerAdapter;
typedef struct _MdwDemuxDataHandlerAdapterClass MdwDemuxDataHandlerAdapterClass;


/**
 *  Dump section instance.
 */
struct _MdwDemuxDataHandlerAdapter {
    GObject parent; /**< obligatory declaration */
    
    /* public members that need to be overrided*/
    gboolean (*new_data) (MdwDemuxDataHandler *self, const guint16 pid, const gsize size, const guint8* data);
    gpointer data;
};


/**
 * Dump section class.
 */
struct _MdwDemuxDataHandlerAdapterClass {
    GObjectClass parent; /**< obligatory declaration */
    
    /* public methods */
    /* void */
};

/** 
 * (obligatory declaration) Get GType id associated with MdwDemuxDataHandlerAdapter.
 */

GType mdw_demux_data_handler_adapter_get_type(void);

/* Constructors */

/**
 * Constructs a new MdwDemuxDataHandlerAdapter.
 * @return Returns a new MdwDemuxDataHandlerAdapter.
 */
MdwDemuxDataHandlerAdapter* mdw_demux_data_handler_adapter_new();

/* public methods */
/* void */

G_END_DECLS


#endif /* MDW_DEMUX_DATA_HANDLER_ADAPTER_H */
