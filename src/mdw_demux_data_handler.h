#ifndef MDW_DEMUX_DATA_HANDLER_H
#define MDW_DEMUX_DATA_HANDLER_H


#include <glib-object.h>

G_BEGIN_DECLS
/* premabule init */
#define MDW_TYPE_DEMUX_DATA_HANDLER (mdw_demux_data_handler_get_type())
#define MDW_DEMUX_DATA_HANDLER(obj)  (G_TYPE_CHECK_INSTANCE_CAST((obj), MDW_TYPE_DEMUX_DATA_HANDLER, MdwDemuxDataHandler))
#define MDW_IS_DEMUX_DATA_HANDLER(obj)  (G_TYPE_CHECK_INSTANCE_TYPE((obj), MDW_TYPE_DEMUX_DATA_HANDLER))
#define MDW_DEMUX_DATA_HANDLER_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), MDW_TYPE_DEMUX_DATA_HANDLER, MdwDemuxDataHandlerInterface))

/* preambule end */


/** Dummy class */
typedef struct _MdwDemuxDataHandler MdwDemuxDataHandler; 
typedef struct _MdwDemuxDataHandlerInterface MdwDemuxDataHandlerInterface;


/** 
 * Interface for handle data from MdwDemux
 */
struct _MdwDemuxDataHandlerInterface {
    GTypeInterface parent; /**< obligatory declaration */
    /* interface methods */
    gboolean (*new_data) (MdwDemuxDataHandler *self, 
        const guint16 pid, const gsize size, const guint8* data); /**< called when new data is ready see*/

};


/** 
 * (obligatory declaration) Get GType id associated with MdwDemuxDataHandlerInterface.
 */
GType mdw_demux_data_handler_get_type(void);

/* methods declarations */
/**
 * Called when new fresh data is ready.
 * @param self MdwDemuxDataHandler instance.
 * @param pid The pid.
 * @param size Size of _data_ buffer.
 * @param data The data buffer.
 * @return If it returns FALSE no more subsequence calls are performed. 
 */
gboolean mdw_demux_data_handler_new_data (MdwDemuxDataHandler *self, 
        const guint16 pid, const gsize size, const guint8* data);



G_END_DECLS






#endif /* MDW_DEMUX_DATA_HANDLER_H */
