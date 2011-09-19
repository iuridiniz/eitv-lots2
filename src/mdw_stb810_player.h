/***************************************************************************
 *  MdwStb810Player
 *  Thu Mar 22 07:33:28 2007 
 *
 *  Copyright (c) 2007 iuri,,,
 *  Authors: iuri,,, <iuri@email.com.br> 2007
 * 
 *  VERSION CONTROL INFORMATION: $Id$
 *
****************************************************************************/

#ifndef MDW_STB810_PLAYER_H
#define MDW_STB810_PLAYER_H

#include <glib-object.h>

#include "mdw_dvb_demux.h"

G_BEGIN_DECLS

/* preambule init */
#define MDW_TYPE_STB810_PLAYER   (mdw_stb810_player_get_type())
#define MDW_STB810_PLAYER(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj), MDW_TYPE_STB810_PLAYER, MdwStb810Player))
#define MDW_STB810_PLAYER_CLASS(vtable)   (G_TYPE_CHECK_CLASS_CAST((vtable), MDW_TYPE_STB810_PLAYER, MdwStb810PlayerClass))
#define MDW_IS_STB810_PLAYER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), MDW_TYPE_STB810_PLAYER))
#define MDW_IS_STB810_PLAYER_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE((vtable), MDW_TYPE_STB810_PLAYER))
#define MDW_STB810_PLAYER_GET_CLASS(inst) (G_TYPE_INSTANCE_GET_CLASS((inst), MDW_TYPE_STB810_PLAYER, MdwStb810PlayerClass))
/* preambule end */

/* class types */
typedef struct _MdwStb810Player MdwStb810Player;
typedef struct _MdwStb810PlayerClass MdwStb810PlayerClass;


/**
 *  A Player instance.
 */
struct _MdwStb810Player {
    GObject parent; /**< pointer to parent (obligatory declaration) */
};


/**
 * A Player class.
 */
struct _MdwStb810PlayerClass {
    GObjectClass parent; /**< pointer to parent (obligatory declaration) */
    
    /* public methods */
    gint16 (*get_level) (MdwStb810Player* self);
    gboolean   (*set_level) (MdwStb810Player* self, guint16 level, GError** error);
    
};

/** 
 * Get GType id associated with MdwStb810Player (obligatory declaration).
 * @return Returns the GType id associated with MdwStb810Player.
 */

GType mdw_stb810_player_get_type(void);

/****************/
/* Constructors */
/****************/

/**
 * Constructs a new MdwStb810Player.
 * @return Returns a new MdwStb810Player.
 */
MdwStb810Player* mdw_stb810_player_new();

/**
 * Constructs a new MdwStb810Player specifing the layer.
 * @param layer the layer to use
 * @return Returns a new MdwStb810Player.
 */
MdwStb810Player* mdw_stb810_player_new_with_layer(guint8 layer);


/*******************/
/* public methods */
/*******************/

gint16 mdw_stb810_player_get_level (MdwStb810Player* self);
gboolean   mdw_stb810_player_set_level (MdwStb810Player* self, guint16 level, GError** error);


G_END_DECLS

#endif /* MDW_STB810_PLAYER_H */

