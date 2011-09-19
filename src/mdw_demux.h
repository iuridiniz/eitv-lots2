/*@COPYRIGHT@*/
#ifndef MDW_DEMUX_H
#define MDW_DEMUX_H


#include <glib-object.h>
#include "mdw_demux_data_handler.h"
#include "mdw_ts_source.h"


G_BEGIN_DECLS

/* preambule init */
#define MDW_TYPE_DEMUX   (mdw_demux_get_type())
#define MDW_DEMUX(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj), MDW_TYPE_DEMUX, MdwDemux))
#define MDW_IS_DEMUX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), MDW_TYPE_DEMUX))
#define MDW_DEMUX_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), MDW_TYPE_DEMUX, MdwDemuxInterface))
/* preambule end */

typedef struct _MdwDemux  MdwDemux; /* dummy */
typedef struct _MdwDemuxInterface  MdwDemuxInterface;

/**
 * MdwDemux Interface
 *
 * @todo TODO: can I convert add_pes_handler to signals with compatibility with LINUXDVBAPI decoders and a little overread? I think not, but add_section_handler is possible. I think is better to add signals and leave this functions (async and sync).
 */
struct _MdwDemuxInterface {
    GTypeInterface parent; /**< obligatory declaration */
    
    /* interface methods */
    gboolean (*add_section_handler) (MdwDemux *self, MdwDemuxDataHandler *handler, guint16 pid); /**< Adds a new section handler */
    gboolean (*add_pes_handler) (MdwDemux *self, MdwDemuxDataHandler *handler, guint16 pid); /**< Adds a new pes handler */
    gboolean (*set_ts_source) (MdwDemux *self, MdwTsSource* source); /**< Setup the source */
    gboolean (*start) (MdwDemux *self); /**< Starts the demuxing process */
    gboolean (*stop) (MdwDemux *self); /**< Stops the demuxing process */
    //gboolean (*step) (MdwDemux *self); /**< Step by step demuxing process */

    gboolean (*del_section_handler) (MdwDemux *self, MdwDemuxDataHandler *handler, guint16 pid);
    gboolean (*del_pes_handler) (MdwDemux *self, MdwDemuxDataHandler *handler, guint16 pid); 
};


/** obligatory declaration */
GType mdw_demux_get_type (void);

/* methods declarations */

/**
 * Adds a new section handler.
 * @param self The MdwDemux instance.
 * @param handler The MdwDemuxDataHandler instance.
 * @param pid The pid of the section.
 */
gboolean mdw_demux_add_section_handler(MdwDemux *self, MdwDemuxDataHandler *handler, guint16 pid);



gboolean mdw_demux_del_section_handler(MdwDemux *self, MdwDemuxDataHandler *handler, guint16 pid);

/**
 * Adds a new Packetised Elementary Stream handler.
 * @param self The MdwDemux instance.
 * @param handler The MdwDemuxDataHandler instance.
 * @param pid The pid of the stream.
 */
gboolean mdw_demux_add_pes_handler(MdwDemux *self, MdwDemuxDataHandler *handler, guint16 pid);

gboolean mdw_demux_del_pes_handler(MdwDemux *self, MdwDemuxDataHandler *handler, guint16 pid);

/**
 * Setups the source of TS packets.
 * @param self the MdwDemux instance.
 * @param source the MdwTsSource instance.
 */
gboolean mdw_demux_set_ts_source(MdwDemux *self, MdwTsSource* source);

/** 
 * Starts the demuxing process.
 * @param self the MdwDemux instance.
 */
gboolean mdw_demux_start (MdwDemux *self);

/** 
 * Stops the demuxing process.
 * @param self the MdwDemux instance.
 */
gboolean mdw_demux_stop (MdwDemux *self);

//void mdw_demux_step(MdwDemux *self);

G_END_DECLS


#endif /* MDW_DEMUX_H */
