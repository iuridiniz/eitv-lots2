/***************************************************************************
 *  MdwPlayer
 *  Fri Mar 30 13:43:28 2007 
 *
 *  Copyright (c) 2007 Iuri Diniz,,,
 *  Authors: Iuri Diniz,,, <iuri@email.com.br> 2007
 * 
 *  VERSION CONTROL INFORMATION: $Id$
 *
****************************************************************************/

#ifndef MDW_PLAYER_H
#define MDW_PLAYER_H

#include <glib-object.h>
#include "mdw_demux.h"

G_BEGIN_DECLS

/* preambule init */
#define MDW_TYPE_PLAYER   (mdw_player_get_type())
#define MDW_PLAYER(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj), MDW_TYPE_PLAYER, MdwPlayer))
#define MDW_IS_PLAYER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), MDW_TYPE_PLAYER))
#define MDW_PLAYER_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), MDW_TYPE_PLAYER, MdwPlayerInterface))
/* preambule end */

typedef struct _MdwPlayer  MdwPlayer; /* dummy */
typedef struct _MdwPlayerInterface  MdwPlayerInterface;

/**
 * MdwPlayer Interface
 *
 */
struct _MdwPlayerInterface {
    GTypeInterface parent; /**< obligatory declaration */
    
    /* interface methods */
    MdwDemux* (*get_demux) (MdwPlayer* self);
    void      (*set_demux) (MdwPlayer* self, MdwDemux* demux);

    gboolean (*fullscreen) (MdwPlayer* self, GError** error);
    gboolean (*lower_to_bottom) (MdwPlayer* self, GError** error);
    gboolean (*raise_to_top) (MdwPlayer* self, GError** error);
    
    gboolean (*set_pos) (MdwPlayer* self, guint16 x, guint16 y, GError** error);
    gboolean (*get_pos) (MdwPlayer* self, guint16* x, guint16* y, GError** error);

    guint16 (*get_audio_pid) (MdwPlayer* self);
    void (*set_audio_pid) (MdwPlayer* self, guint16 pid);

    guint16 (*get_video_pid) (MdwPlayer* self);
    void (*set_video_pid) (MdwPlayer* self, guint16 pid);

    gboolean (*set_size) (MdwPlayer* self, guint16 width, guint16 height, GError** error);
    gboolean (*get_size) (MdwPlayer* self, guint16* width, guint16* height, GError** error);

    gboolean (*get_max_size) (MdwPlayer* self, guint16* width, guint16* height, GError** error);
    
    gboolean (*play) (MdwPlayer* self, GError** error);
    gboolean (*stop) (MdwPlayer* self, GError** error);

};

/** obligatory declaration */
GType mdw_player_get_type (void);

/* methods declarations */
MdwDemux* mdw_player_get_demux (MdwPlayer* self);
void      mdw_player_set_demux (MdwPlayer* self, MdwDemux* demux);

gboolean mdw_player_fullscreen (MdwPlayer* self, GError** error);
gboolean mdw_player_lower_to_bottom (MdwPlayer* self, GError** error);
gboolean mdw_player_raise_to_top (MdwPlayer* self, GError** error);

gboolean mdw_player_set_pos (MdwPlayer* self, guint16 x, guint16 y, GError** error);
gboolean mdw_player_get_pos (MdwPlayer* slef, guint16* x, guint16* y, GError** error);

void mdw_player_set_audio_pid (MdwPlayer* self, guint16 pid);
void mdw_player_set_video_pid (MdwPlayer* self, guint16 pid);

guint16 mdw_player_get_audio_pid (MdwPlayer* self);
guint16 mdw_player_get_video_pid (MdwPlayer* self);

gboolean mdw_player_set_size (MdwPlayer* self, guint16 width, guint16 height, GError** error);
gboolean mdw_player_get_size (MdwPlayer* self, guint16* width, guint16* height, GError** error);
gboolean mdw_player_get_max_size (MdwPlayer* self, guint16* width, guint16* height, GError** error);

gboolean mdw_player_play (MdwPlayer* self, GError** error);
gboolean mdw_player_stop (MdwPlayer* self, GError** error);

G_END_DECLS


#endif /* MDW_PLAYER_H */
