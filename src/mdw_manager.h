/***************************************************************************
 *  MdwManager
 *  Wed Apr  4 16:18:31 2007 
 *
 *  Copyright (c) 2007 Iuri Diniz,,,
 *  Authors: Iuri Diniz,,, <iuri@email.com.br> 2007
 * 
 *  VERSION CONTROL INFORMATION: $Id$
 *
****************************************************************************/

#ifndef MDW_MANAGER_H
#define MDW_MANAGER_H

#include <glib-object.h>
#include <dbus/dbus-glib.h>

G_BEGIN_DECLS

/* preambule init */
#define MDW_TYPE_MANAGER   (mdw_manager_get_type())
#define MDW_MANAGER(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj), MDW_TYPE_MANAGER, MdwManager))
#define MDW_MANAGER_CLASS(vtable)   (G_TYPE_CHECK_CLASS_CAST((vtable), MDW_TYPE_MANAGER, MdwManagerClass))
#define MDW_IS_MANAGER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), MDW_TYPE_MANAGER))
#define MDW_IS_MANAGER_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE((vtable), MDW_TYPE_MANAGER))
#define MDW_MANAGER_GET_CLASS(inst) (G_TYPE_INSTANCE_GET_CLASS((inst), MDW_TYPE_MANAGER, MdwManagerClass))
/* preambule end */

/* class types */
typedef struct _MdwManager MdwManager;
typedef struct _MdwManagerClass MdwManagerClass;

/**
 *  A Manager instance.
 */
struct _MdwManager {
    GObject parent; /**< pointer to parent (obligatory declaration) */
    
    /* private members */
    //gboolean boolean_member; /**< a boolean_member */
    //guint8 uint8_member; /**< a uint8_member */
    
};


/**
 * A Manager class.
 */
struct _MdwManagerClass {
    GObjectClass parent; /**< pointer to parent (obligatory declaration) */
    
    /* public methods */
    gboolean (*play_with_pids) (MdwManager* self,  const guint IN_audio_pid, const guint IN_video_pid, const guint IN_pcr_pid, GError **error); /**< pointer to play_with_pids, @see mdw_manager_play_with_pids  */

    gboolean (*stop) (MdwManager* self, GError **error); /**< pointer to stop, @see mdw_manager_play */
    gboolean (*set_size) (MdwManager *self, const guint IN_width, const guint IN_height, GError **error); /**< pointer to set_size, @see mdw_manager_set_size */
    gboolean (*set_pos) (MdwManager *self, const guint IN_pos_x, const guint IN_pos_y, GError **error); /**< pointer to set_pos, @see mdw_manager_set_pos */
    gboolean (*raise_to_top) (MdwManager *self, GError **error); /**< pointer to raise, @see mdw_manager_raise */
    gboolean (*lower_to_bottom) (MdwManager *self, GError **error); /**< pointer to lower, @see mdw_manager_lower */

    gboolean (*add_watch)  (MdwManager *self, const guint IN_pid, GError **error); /**< pointer to add_watch, @see mdw_manager_add_watch */
    gboolean (*del_watch)  (MdwManager *self, const guint IN_pid, GError **error); /**< pointer to del_watch, @see mdw_manager_del_watch */
    gboolean (*count_watch)  (MdwManager *self, const guint IN_pid, guint* OUT_count, GError **error); /**< pointer to count_watch, @see mdw_manager_count_watch */

    gboolean (*play) (MdwManager* self, GError **error); /**< pointer to play, @see mdw_manager_play  */

    gboolean (*is_playing) (MdwManager *self, gboolean* OUT_is_playing, GError **error);
    gboolean (*get_size) (MdwManager *self, guint* OUT_width, guint* OUT_height, GError **error);
    gboolean (*get_max_size) (MdwManager *self, guint* OUT_max_height, guint* OUT_max_width, GError **error);
    gboolean (*get_pos) (MdwManager *self, guint* OUT_pos_x, guint* OUT_pos_y, GError **error);
    gboolean (*get_video_pid) (MdwManager *self, guint* OUT_pid, GError **error);
    gboolean (*set_video_pid) (MdwManager *self, const guint IN_pid, GError **error);
    gboolean (*get_audio_pid) (MdwManager *self, guint* OUT_pid, GError **error);
    gboolean (*set_audio_pid) (MdwManager *self, const guint IN_pid, GError **error);
    gboolean (*get_pcr_pid) (MdwManager *self, guint* OUT_pid, GError **error);
    gboolean (*set_pcr_pid) (MdwManager *self, const guint IN_pid, GError **error);
    gboolean (*set_uri) (MdwManager *self, const char * IN_uri, GError **error);
    gboolean (*get_uri) (MdwManager *self, char ** OUT_uri, GError **error);

    gboolean (*dbus_register) (MdwManager *self, GError **error);


};

/** 
 * Get GType id associated with MdwManager (obligatory declaration).
 * @return Returns the GType id associated with MdwManager.
 */

GType mdw_manager_get_type(void);

/****************/
/* Constructors */
/****************/

/**
 * Constructs a new MdwManager.
 * @return Returns a new MdwManager.
 */
MdwManager* mdw_manager_new();


/*******************/
/* public methods */
/*******************/
gboolean mdw_manager_play_with_pids (MdwManager* self, const guint IN_audio_pid, const guint IN_video_pid, const guint IN_pcr_pid, GError **error); 
gboolean mdw_manager_stop (MdwManager* self, GError **error); 
gboolean mdw_manager_set_size (MdwManager *self, const guint IN_width, const guint IN_height, GError **error); 
gboolean mdw_manager_set_pos (MdwManager *self, const guint IN_pos_x, const guint IN_pos_y, GError **error); 
gboolean mdw_manager_raise_to_top (MdwManager *self, GError **error); 
gboolean mdw_manager_lower_to_bottom (MdwManager *self, GError **error); 

gboolean mdw_manager_add_watch  (MdwManager *self, const guint IN_pid, GError **error); 
gboolean mdw_manager_del_watch  (MdwManager *self, const guint IN_pid, GError **error); 
gboolean mdw_manager_count_watch  (MdwManager *self, const guint IN_pid, guint* OUT_count, GError **error); 

gboolean mdw_manager_play (MdwManager* self, GError **error); 
gboolean mdw_manager_is_playing (MdwManager *self, gboolean* OUT_is_playing, GError **error);
gboolean mdw_manager_get_size (MdwManager *self, guint* OUT_width, guint* OUT_height, GError **error);
gboolean mdw_manager_get_max_size (MdwManager *self, guint* OUT_max_height, guint* OUT_max_width, GError **error);
gboolean mdw_manager_get_pos (MdwManager *self, guint* OUT_pos_x, guint* OUT_pos_y, GError **error);
gboolean mdw_manager_get_video_pid (MdwManager *self, guint* OUT_pid, GError **error);
gboolean mdw_manager_set_video_pid (MdwManager *self, const guint IN_pid, GError **error);
gboolean mdw_manager_get_audio_pid (MdwManager *self, guint* OUT_pid, GError **error);
gboolean mdw_manager_set_audio_pid (MdwManager *self, const guint IN_pid, GError **error);
gboolean mdw_manager_get_pcr_pid (MdwManager *self, guint* OUT_pid, GError **error);
gboolean mdw_manager_set_pcr_pid (MdwManager *self, const guint IN_pid, GError **error);
gboolean mdw_manager_set_uri (MdwManager *self, const char * IN_uri, GError **error);
gboolean mdw_manager_get_uri (MdwManager *self, char ** OUT_uri, GError **error);

gboolean mdw_manager_dbus_register (MdwManager *self, GError **error);


/* Error */
#define MDW_MANAGER_ERROR mdw_manager_error_quark ()
typedef enum
{
  MDW_MANAGER_ERROR_INVALID_PARAMS,
  MDW_MANAGER_ERROR_INVALID_CALL,
  MDW_MANAGER_ERROR_NOT_IMPLEMENTED,
  MDW_MANAGER_ERROR_FAILED
} MdwManagerError;

GQuark mdw_manager_error_quark();

G_END_DECLS

#endif /* MDW_MANAGER_H */

