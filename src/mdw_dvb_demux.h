/***************************************************************************
 *  MdwDvbDemux
 *  Mon Apr 23 14:45:24 2007 
 *
 *  Copyright (c) 2007 Iuri Diniz,,,
 *  Authors: Iuri Diniz,,, <iuri@email.com.br> 2007
 * 
 *  VERSION CONTROL INFORMATION: $Id$
 *
****************************************************************************/

#ifndef MDW_DVB_DEMUX_H
#define MDW_DVB_DEMUX_H

#include <glib-object.h>

G_BEGIN_DECLS

/* preambule init */
#define MDW_TYPE_DVB_DEMUX   (mdw_dvb_demux_get_type())
#define MDW_DVB_DEMUX(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj), MDW_TYPE_DVB_DEMUX, MdwDvbDemux))
#define MDW_DVB_DEMUX_CLASS(vtable)   (G_TYPE_CHECK_CLASS_CAST((vtable), MDW_TYPE_DVB_DEMUX, MdwDvbDemuxClass))
#define MDW_IS_DVB_DEMUX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), MDW_TYPE_DVB_DEMUX))
#define MDW_IS_DVB_DEMUX_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE((vtable), MDW_TYPE_DVB_DEMUX))
#define MDW_DVB_DEMUX_GET_CLASS(inst) (G_TYPE_INSTANCE_GET_CLASS((inst), MDW_TYPE_DVB_DEMUX, MdwDvbDemuxClass))
/* preambule end */

/* class types */
typedef struct _MdwDvbDemux MdwDvbDemux;
typedef struct _MdwDvbDemuxClass MdwDvbDemuxClass;

/**
 *  A DvbDemux instance.
 */
struct _MdwDvbDemux {
    GObject parent; /**< pointer to parent (obligatory declaration) */
    
    /* private members */
    //gboolean boolean_member; /**< a boolean_member */
    //guint8 uint8_member; /**< a uint8_member */
    
};


/**
 * A DvbDemux class.
 */
struct _MdwDvbDemuxClass {
    GObjectClass parent; /**< pointer to parent (obligatory declaration) */
    
    /* public methods */
    //gboolean (*method1) (MdwDvbDemux* self, guint param1, guint param2, GError** error); /**< pointer to method1, @see mdw_dvb_demux_method1  */
    //gboolean (*method2) (MdwDvbDemux* self, guint param1, guint param2); /**< pointer to method2, @see mdw_dvb_demux_method2 */
};

/** 
 * Get GType id associated with MdwDvbDemux (obligatory declaration).
 * @return Returns the GType id associated with MdwDvbDemux.
 */

GType mdw_dvb_demux_get_type(void);

/****************/
/* Constructors */
/****************/

/**
 * Constructs a new MdwDvbDemux.
 * @return Returns a new MdwDvbDemux.
 */
MdwDvbDemux* mdw_dvb_demux_new();

/**
 * Constructs a new MdwDvbDemux.
 * @param param1 A guint8 parameter to construct a new MdwDvbDemux instance.
 * @return       Returns a new MdwDvbDemux.
 */
//MdwDvbDemux* mdw_dvb_demux_new_with_arg1(guint8 param1);


/*******************/
/* public methods */
/*******************/

/**
 * Method1 of MdwDvbDemux.
 * @param self A pointer to a MdwDvbDemux instance.
 * @param param1 A guint parameter.
 * @param param2 A guint parameter.
 * @param error If an error has ocurred, this variable will pointer to error ocurred, 
 *              this variable need to be free if a error has ocurred.
 * @Return Returns TRUE if no errors ocurred, otherwise returns FALSE.
 */
//gboolean (mdw_dvb_demux_method1) (MdwDvbDemux* self, guint param1, guint param2, GError** error);

/**
 * Method2 of MdwDvbDemux.
 * @param self A pointer to a MdwDvbDemux instance.
 * @param param1 A guint parameter.
 * @param param2 A guint parameter.
 * @Return Returns TRUE if no errors ocurred, otherwise returns FALSE.
 */

//gboolean (mdw_dvb_demux_method2) (MdwDvbDemux* self, guint param1, guint param2);

G_END_DECLS

/* Error */
#define MDW_DVB_DEMUX_ERROR mdw_dvb_demux_error_quark()
typedef enum
{
  //MDW_DVB_DEMUX_ERROR_TYPE_1,
  //MDW_DVB_DEMUX_ERROR_TYPE_2,
  //MDW_DVB_DEMUX_ERROR_NOT_IMPLEMENTED,
  MDW_DVB_DEMUX_ERROR_IOCTL_FAILED,
  MDW_DVB_DEMUX_ERROR_INVALID_CALL,
  MDW_DVB_DEMUX_ERROR_FAILED
} MdwDvbDemuxError;

GQuark mdw_dvb_demux_error_quark();


#endif /* MDW_DVB_DEMUX_H */

