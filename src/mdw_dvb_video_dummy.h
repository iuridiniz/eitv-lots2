/***************************************************************************
 *  MdwDvbVideoDummy
 *  Tue Mar 20 16:15:05 2007 
 *
 *  Copyright (c) 2007 Iuri Diniz,,,
 *  Authors: Iuri Diniz,,, <iuri@email.com.br> 2007
 * 
 *  VERSION CONTROL INFORMATION: $Id$
 *  
 *
****************************************************************************/

#ifndef MDW_DVB_VIDEO_DUMMY_H
#define MDW_DVB_VIDEO_DUMMY_H

#include <glib-object.h>

G_BEGIN_DECLS

/* preambule init */
#define MDW_TYPE_DVB_VIDEO_DUMMY   (mdw_dvb_video_dummy_get_type())
#define MDW_DVB_VIDEO_DUMMY(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj), MDW_TYPE_DVB_VIDEO_DUMMY, MdwDvbVideoDummy))
#define MDW_DVB_VIDEO_DUMMY_CLASS(vtable)   (G_TYPE_CHECK_CLASS_CAST((vtable), MDW_TYPE_DVB_VIDEO_DUMMY, MdwDvbVideoDummyClass))
#define MDW_IS_DVB_VIDEO_DUMMY(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), MDW_TYPE_DVB_VIDEO_DUMMY))
#define MDW_IS_DVB_VIDEO_DUMMY_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE((vtable), MDW_TYPE_DVB_VIDEO_DUMMY))
#define MDW_DVB_VIDEO_DUMMY_GET_CLASS(inst) (G_TYPE_INSTANCE_GET_CLASS((inst), MDW_TYPE_DVB_VIDEO_DUMMY, MdwDvbVideoDummyClass))
/* preambule end */

/* class types */
typedef struct _MdwDvbVideoDummy MdwDvbVideoDummy;
typedef struct _MdwDvbVideoDummyClass MdwDvbVideoDummyClass;


/**
 *  A DvbVideoDummy instance.
 */
struct _MdwDvbVideoDummy {
    GObject parent; /**< pointer to parent (obligatory declaration) */
    
    /* private members */
    /* void */
};


/**
 * A DvbVideoDummy class.
 */
struct _MdwDvbVideoDummyClass {
    GObjectClass parent; /**< pointer to parent (obligatory declaration) */
    
    /* public methods */
    /* void */
};

/** 
 * Get GType id associated with MdwDvbVideoDummy (obligatory declaration).
 * @return Returns the GType id associated with MdwDvbVideoDummy.
 */

GType mdw_dvb_video_dummy_get_type(void);

/****************/
/* Constructors */
/****************/

/**
 * Constructs a new MdwDvbVideoDummy.
 * @return Returns a new MdwDvbVideoDummy.
 */
MdwDvbVideoDummy* mdw_dvb_video_dummy_new();


/*******************/
/* public methods */
/*******************/

/* void */

G_END_DECLS

#endif /* MDW_DVB_VIDEO_DUMMY_H */

