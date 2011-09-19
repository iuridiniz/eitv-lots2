/***************************************************************************
 *  MdwDefaultPlayer
 *  Fri Mar 30 13:00:34 2007 
 *
 *  Copyright (c) 2007 Iuri Diniz,,,
 *  Authors: Iuri Diniz,,, <iuri@email.com.br> 2007
 * 
 *  VERSION CONTROL INFORMATION: $Id$
 *
****************************************************************************/

#ifndef MDW_DEFAULT_PLAYER_H
#define MDW_DEFAULT_PLAYER_H

#include <glib-object.h>

G_BEGIN_DECLS

/* preambule init */
#define MDW_TYPE_DEFAULT_PLAYER   (mdw_default_player_get_type())
#define MDW_DEFAULT_PLAYER(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj), MDW_TYPE_DEFAULT_PLAYER, MdwDefaultPlayer))
#define MDW_DEFAULT_PLAYER_CLASS(vtable)   (G_TYPE_CHECK_CLASS_CAST((vtable), MDW_TYPE_DEFAULT_PLAYER, MdwDefaultPlayerClass))
#define MDW_IS_DEFAULT_PLAYER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), MDW_TYPE_DEFAULT_PLAYER))
#define MDW_IS_DEFAULT_PLAYER_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE((vtable), MDW_TYPE_DEFAULT_PLAYER))
#define MDW_DEFAULT_PLAYER_GET_CLASS(inst) (G_TYPE_INSTANCE_GET_CLASS((inst), MDW_TYPE_DEFAULT_PLAYER, MdwDefaultPlayerClass))
/* preambule end */

/* class types */
typedef struct _MdwDefaultPlayer MdwDefaultPlayer;
typedef struct _MdwDefaultPlayerClass MdwDefaultPlayerClass;

/**
 *  A Player instance.
 */
struct _MdwDefaultPlayer {
    GObject parent; /**< pointer to parent (obligatory declaration) */
    
    /* private members */
    //gboolean boolean_member; /**< a boolean_member */
    //guint8 uint8_member; /**< a uint8_member */
    
};


/**
 * A Player class.
 */
struct _MdwDefaultPlayerClass {
    GObjectClass parent; /**< pointer to parent (obligatory declaration) */
    
    /* public methods */
    //gboolean (*method1) (MdwDefaultPlayer* self, guint param1, guint param2, GError** error); /**< pointer to method1, @see ffdfbplayermethod2  */
    //gboolean (*method2) (MdwDefaultPlayer* self, guint param1, guint param2); /**< pointer to method2, @see ffdfbplayermethod2 */
};

/** 
 * Get GType id associated with MdwDefaultPlayer (obligatory declaration).
 * @return Returns the GType id associated with MdwDefaultPlayer.
 */

GType mdw_default_player_get_type(void);

/****************/
/* Constructors */
/****************/

/**
 * Constructs a new MdwDefaultPlayer.
 * @return Returns a new MdwDefaultPlayer.
 */
MdwDefaultPlayer* mdw_default_player_new();

/**
 * Constructs a new MdwDefaultPlayer.
 * @param param1 A guint8 parameter to construct a new MdwDefaultPlayer instance.
 * @return       Returns a new MdwDefaultPlayer.
 */
//MdwDefaultPlayer* mdw_default_player_new_with_arg1(guint8 param1);


/*******************/
/* public methods */
/*******************/

/**
 * Method1 of MdwDefaultPlayer.
 * @param self A pointer to a MdwDefaultPlayer instance.
 * @param param1 A guint parameter.
 * @param param2 A guint parameter.
 * @param error If an error has ocurred, this variable will pointer to error ocurred, 
 *              this variable need to be free if a error has ocurred.
 * @Return Returns TRUE if no errors ocurred, otherwise returns FALSE.
 */
//gboolean (mdw_default_player_method1) (MdwDefaultPlayer* self, guint param1, guint param2, GError** error);

/**
 * Method2 of MdwDefaultPlayer.
 * @param self A pointer to a MdwDefaultPlayer instance.
 * @param param1 A guint parameter.
 * @param param2 A guint parameter.
 * @Return Returns TRUE if no errors ocurred, otherwise returns FALSE.
 */

//gboolean (mdw_default_player_method2) (MdwDefaultPlayer* self, guint param1, guint param2);

/* Error */
#define MDW_DEFAULT_PLAYER_ERROR mdw_default_player_error_quark()
typedef enum
{
  MDW_DEFAULT_PLAYER_ERROR_DISPLAY,
  MDW_DEFAULT_PLAYER_ERROR_DECODER,
  MDW_DEFAULT_PLAYER_ERROR_NOT_IMPLEMENTED,
  MDW_DEFAULT_PLAYER_ERROR_FAILED
} MdwDefaultPlayerError;

GQuark mdw_default_player_error_quark();

G_END_DECLS

#endif /* MDW_DEFAULT_PLAYER_H */

