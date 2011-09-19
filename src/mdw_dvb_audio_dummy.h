/***************************************************************************
 *  MdwDvbAudioDummy
 *  Tue Mar 20 16:33:01 2007 
 *
 *  Copyright (c) 2007 Iuri Diniz,,,
 *  Authors: Iuri Diniz,,, <iuri@email.com.br> 2007
 * 
 *  VERSION CONTROL INFORMATION: $Id$
 *
****************************************************************************/

#ifndef MDW_DVB_AUDIO_DUMMY_H
#define MDW_DVB_AUDIO_DUMMY_H

#include <glib-object.h>

G_BEGIN_DECLS

/* preambule init */
#define MDW_TYPE_DVB_AUDIO_DUMMY   (mdw_dvb_audio_dummy_get_type())
#define MDW_DVB_AUDIO_DUMMY(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj), MDW_TYPE_DVB_AUDIO_DUMMY, MdwDvbAudioDummy))
#define MDW_DVB_AUDIO_DUMMY_CLASS(vtable)   (G_TYPE_CHECK_CLASS_CAST((vtable), MDW_TYPE_DVB_AUDIO_DUMMY, MdwDvbAudioDummyClass))
#define MDW_IS_DVB_AUDIO_DUMMY(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), MDW_TYPE_DVB_AUDIO_DUMMY))
#define MDW_IS_DVB_AUDIO_DUMMY_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE((vtable), MDW_TYPE_DVB_AUDIO_DUMMY))
#define MDW_DVB_AUDIO_DUMMY_GET_CLASS(inst) (G_TYPE_INSTANCE_GET_CLASS((inst), MDW_TYPE_DVB_AUDIO_DUMMY, MdwDvbAudioDummyClass))
/* preambule end */

/* class types */
typedef struct _MdwDvbAudioDummy MdwDvbAudioDummy;
typedef struct _MdwDvbAudioDummyClass MdwDvbAudioDummyClass;


/**
 *  A DvbAudioDummy instance.
 */
struct _MdwDvbAudioDummy {
    GObject parent; /**< pointer to parent (obligatory declaration) */
};


/**
 * A DvbAudioDummy class.
 */
struct _MdwDvbAudioDummyClass {
    GObjectClass parent; /**< pointer to parent (obligatory declaration) */
};

/** 
 * Get GType id associated with MdwDvbAudioDummy (obligatory declaration).
 * @return Returns the GType id associated with MdwDvbAudioDummy.
 */

GType mdw_dvb_audio_dummy_get_type(void);

/****************/
/* Constructors */
/****************/

/**
 * Constructs a new MdwDvbAudioDummy.
 * @return Returns a new MdwDvbAudioDummy.
 */
MdwDvbAudioDummy* mdw_dvb_audio_dummy_new();

G_END_DECLS

#endif /* MDW_DVB_AUDIO_DUMMY_H */

