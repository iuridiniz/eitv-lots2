/***************************************************************************
 *  MdwDefaultDemux
 *  Thu Mar 29 17:44:47 2007 
 *
 *  Copyright (c) 2007 Iuri Diniz,,,
 *  Authors: Iuri Diniz,,, <iuri@digizap.com.br> 2007
 * 
 *  VERSION CONTROL INFORMATION: $Id$
 *
****************************************************************************/

#ifndef MDW_DEFAULT_DEMUX_H
#define MDW_DEFAULT_DEMUX_H

#include <glib-object.h>

G_BEGIN_DECLS

/* preambule init */
#define MDW_TYPE_DEFAULT_DEMUX   (mdw_default_demux_get_type())
#define MDW_DEFAULT_DEMUX(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj), MDW_TYPE_DEFAULT_DEMUX, MdwDefaultDemux))
#define MDW_DEFAULT_DEMUX_CLASS(vtable)   (G_TYPE_CHECK_CLASS_CAST((vtable), MDW_TYPE_DEFAULT_DEMUX, MdwDefaultDemuxClass))
#define MDW_IS_DEFAULT_DEMUX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), MDW_TYPE_DEFAULT_DEMUX))
#define MDW_IS_DEFAULT_DEMUX_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE((vtable), MDW_TYPE_DEFAULT_DEMUX))
#define MDW_DEFAULT_DEMUX_GET_CLASS(inst) (G_TYPE_INSTANCE_GET_CLASS((inst), MDW_TYPE_DEFAULT_DEMUX, MdwDefaultDemuxClass))
/* preambule end */

/* class types */
typedef struct _MdwDefaultDemux MdwDefaultDemux;
typedef struct _MdwDefaultDemuxClass MdwDefaultDemuxClass;

/**
 *  A DefaultDemux instance.
 */
struct _MdwDefaultDemux {
    GObject parent; /**< pointer to parent (obligatory declaration) */
    
    /* private members */
    /* void */
    
};


/**
 * A DefaultDemux class.
 */
struct _MdwDefaultDemuxClass {
    GObjectClass parent; /**< pointer to parent (obligatory declaration) */
    
    /* public methods */
    /* void */
};

/** 
 * Get GType id associated with MdwDefaultDemux (obligatory declaration).
 * @return Returns the GType id associated with MdwDefaultDemux.
 */

GType mdw_default_demux_get_type(void);

/****************/
/* Constructors */
/****************/

/**
 * Constructs a new MdwDefaultDemux.
 * @return Returns a new MdwDefaultDemux.
 */
MdwDefaultDemux* mdw_default_demux_new();


G_END_DECLS

#endif /* MDW_DEFAULT_DEMUX_H */

