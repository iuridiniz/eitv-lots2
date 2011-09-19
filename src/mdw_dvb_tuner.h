/***************************************************************************
 *  MdwDvbTuner
 *  Mon May 21 08:53:24 2007 
 *
 *  Copyright (c) 2007 iuri,,,
 *  Authors: iuri,,, <iuri@email.com.br> 2007
 * 
 *  VERSION CONTROL INFORMATION: $Id$
 *
****************************************************************************/

#ifndef MDW_DVB_TUNER_H
#define MDW_DVB_TUNER_H

#include <glib-object.h>

G_BEGIN_DECLS

/* preambule init */
#define MDW_TYPE_DVB_TUNER   (mdw_dvb_tuner_get_type())
#define MDW_DVB_TUNER(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj), MDW_TYPE_DVB_TUNER, MdwDvbTuner))
#define MDW_DVB_TUNER_CLASS(vtable)   (G_TYPE_CHECK_CLASS_CAST((vtable), MDW_TYPE_DVB_TUNER, MdwDvbTunerClass))
#define MDW_IS_DVB_TUNER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), MDW_TYPE_DVB_TUNER))
#define MDW_IS_DVB_TUNER_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE((vtable), MDW_TYPE_DVB_TUNER))
#define MDW_DVB_TUNER_GET_CLASS(inst) (G_TYPE_INSTANCE_GET_CLASS((inst), MDW_TYPE_DVB_TUNER, MdwDvbTunerClass))
/* preambule end */

/* class types */
typedef struct _MdwDvbTuner MdwDvbTuner;
typedef struct _MdwDvbTunerClass MdwDvbTunerClass;

/**
 *  A DvbTuner instance.
 */
struct _MdwDvbTuner {
    GObject parent; /**< pointer to parent (obligatory declaration) */
    
    /* private members */
    //gboolean boolean_member; /**< a boolean_member */
    //guint8 uint8_member; /**< a uint8_member */
    
};


/**
 * A DvbTuner class.
 */
struct _MdwDvbTunerClass {
    GObjectClass parent; /**< pointer to parent (obligatory declaration) */
    
    /* public methods */
    //gboolean (*method1) (MdwDvbTuner* self, guint param1, guint param2, GError** error); /**< pointer to method1, @see mdw_dvb_tuner_method1  */
    //gboolean (*method2) (MdwDvbTuner* self, guint param1, guint param2); /**< pointer to method2, @see mdw_dvb_tuner_method2 */
};

/** 
 * Get GType id associated with MdwDvbTuner (obligatory declaration).
 * @return Returns the GType id associated with MdwDvbTuner.
 */

GType mdw_dvb_tuner_get_type(void);

/****************/
/* Constructors */
/****************/

/**
 * Constructs a new MdwDvbTuner.
 * @return Returns a new MdwDvbTuner.
 */
//MdwDvbTuner* mdw_dvb_tuner_new();

/**
 * Constructs a new MdwDvbTuner.
 * @param param1 A guint8 parameter to construct a new MdwDvbTuner instance.
 * @return       Returns a new MdwDvbTuner.
 */
//MdwDvbTuner* mdw_dvb_tuner_new_with_arg1(guint8 param1);


/*******************/
/* public methods */
/*******************/

/**
 * Method1 of MdwDvbTuner.
 * @param self A pointer to a MdwDvbTuner instance.
 * @param param1 A guint parameter.
 * @param param2 A guint parameter.
 * @param error If an error has ocurred, this variable will pointer to error ocurred, 
 *              this variable need to be free if a error has ocurred.
 * @Return Returns TRUE if no errors ocurred, otherwise returns FALSE.
 */
//gboolean (mdw_dvb_tuner_method1) (MdwDvbTuner* self, guint param1, guint param2, GError** error);

/**
 * Method2 of MdwDvbTuner.
 * @param self A pointer to a MdwDvbTuner instance.
 * @param param1 A guint parameter.
 * @param param2 A guint parameter.
 * @Return Returns TRUE if no errors ocurred, otherwise returns FALSE.
 */

//gboolean (mdw_dvb_tuner_method2) (MdwDvbTuner* self, guint param1, guint param2);

G_END_DECLS

/* Error */
/*
#define MDW_DVB_TUNER_ERROR mdw_dvb_tuner_error_quark()
typedef enum
{
  MDW_DVB_TUNER_ERROR_TYPE_1,
  MDW_DVB_TUNER_ERROR_TYPE_2,
  MDW_DVB_TUNER_ERROR_NOT_IMPLEMENTED,
  MDW_DVB_TUNER_ERROR_FAILED
} MdwDvbTunerError;

GQuark mdw_dvb_tuner_error_quark();
*/


#endif /* MDW_DVB_TUNER_H */

