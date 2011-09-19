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

#include  "mdw_player.h"

static void
mdw_player_base_init (gpointer g_class) {
    static gboolean initialized = FALSE;

    if (G_UNLIKELY(initialized == FALSE)) {
        /* signals */
        initialized = TRUE;
    }
}


GType 
mdw_player_get_type(void) {
    static GType type = 0;

    if (G_UNLIKELY(type == 0)) {
        static const GTypeInfo info = {
            .class_size = sizeof(MdwPlayerInterface),
            .base_init = mdw_player_base_init,
            .base_finalize = NULL,
            .class_init = NULL,
            .class_finalize = NULL, 
            .class_data = NULL,
            .instance_size = 0,
            .n_preallocs = 0,
            .instance_init = NULL,
            .value_table = NULL
        };
        type = g_type_register_static(G_TYPE_INTERFACE, "MdwPlayer", &info, 0);
    }
    return type;
}

MdwDemux*
mdw_player_get_demux (MdwPlayer* self)
{
    g_return_val_if_fail(self != NULL, NULL);
    g_assert(MDW_PLAYER_GET_INTERFACE(self)->get_demux != NULL);

    return MDW_PLAYER_GET_INTERFACE(self)->get_demux (self);
}

void        
mdw_player_set_demux (MdwPlayer* self, MdwDemux* demux)
{
    g_return_if_fail(self != NULL);
    g_assert(MDW_PLAYER_GET_INTERFACE(self)->set_demux != NULL);

    MDW_PLAYER_GET_INTERFACE(self)->set_demux (self, demux);
}

gboolean
mdw_player_fullscreen (MdwPlayer* self, GError** error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_assert(MDW_PLAYER_GET_INTERFACE(self)->fullscreen != NULL);

    return MDW_PLAYER_GET_INTERFACE(self)->fullscreen (self, error);
}

gboolean
mdw_player_lower_to_bottom (MdwPlayer* self, GError** error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_assert(MDW_PLAYER_GET_INTERFACE(self)->lower_to_bottom != NULL);

    return MDW_PLAYER_GET_INTERFACE(self)->lower_to_bottom (self, error);
}

gboolean
mdw_player_raise_to_top (MdwPlayer* self, GError** error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_assert(MDW_PLAYER_GET_INTERFACE(self)->raise_to_top != NULL);

    return MDW_PLAYER_GET_INTERFACE(self)->raise_to_top (self, error);
}


gboolean
mdw_player_set_pos (MdwPlayer* self, guint16 x, guint16 y, GError** error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_assert(MDW_PLAYER_GET_INTERFACE(self)->set_pos != NULL);

    return MDW_PLAYER_GET_INTERFACE(self)->set_pos (self, x, y, error);
}

gboolean
mdw_player_get_pos (MdwPlayer* self, guint16* x, guint16* y, GError** error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_assert(MDW_PLAYER_GET_INTERFACE(self)->get_pos != NULL);

    return MDW_PLAYER_GET_INTERFACE(self)->get_pos (self, x, y, error);
}


void
mdw_player_set_audio_pid (MdwPlayer* self, guint16 pid)
{
    g_return_if_fail(self != NULL);
    g_assert(MDW_PLAYER_GET_INTERFACE(self)->set_audio_pid != NULL);

    MDW_PLAYER_GET_INTERFACE(self)->set_audio_pid (self, pid);
}

void
mdw_player_set_video_pid (MdwPlayer* self, guint16 pid)
{
    g_return_if_fail(self != NULL);
    g_assert(MDW_PLAYER_GET_INTERFACE(self)->set_video_pid != NULL);

    MDW_PLAYER_GET_INTERFACE(self)->set_video_pid (self, pid);
}


guint16
mdw_player_get_audio_pid (MdwPlayer* self)
{
    g_return_val_if_fail(self != NULL, 0);
    g_assert(MDW_PLAYER_GET_INTERFACE(self)->get_audio_pid != NULL);

    return MDW_PLAYER_GET_INTERFACE(self)->get_audio_pid (self);
}

guint16
mdw_player_get_video_pid (MdwPlayer* self)
{
    g_return_val_if_fail(self != NULL, 0);
    g_assert(MDW_PLAYER_GET_INTERFACE(self)->get_video_pid != NULL);

    return MDW_PLAYER_GET_INTERFACE(self)->get_video_pid (self);
}


gboolean
mdw_player_set_size (MdwPlayer* self, guint16 width, guint16 height, GError** error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_assert(MDW_PLAYER_GET_INTERFACE(self)->set_size != NULL);

    return MDW_PLAYER_GET_INTERFACE(self)->set_size (self, width, height, error);
}

gboolean
mdw_player_get_size (MdwPlayer* self, guint16* width, guint16* height, GError** error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_assert(MDW_PLAYER_GET_INTERFACE(self)->get_size != NULL);

    return MDW_PLAYER_GET_INTERFACE(self)->get_size (self, width, height, error);
}

gboolean
mdw_player_get_max_size (MdwPlayer* self, guint16* width, guint16* height, GError** error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_assert(MDW_PLAYER_GET_INTERFACE(self)->get_max_size != NULL);

    return MDW_PLAYER_GET_INTERFACE(self)->get_max_size (self, width, height, error);
}

gboolean
mdw_player_play (MdwPlayer* self, GError** error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_assert(MDW_PLAYER_GET_INTERFACE(self)->play != NULL);

    return MDW_PLAYER_GET_INTERFACE(self)->play (self, error);
}

gboolean
mdw_player_stop (MdwPlayer* self, GError** error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_assert(MDW_PLAYER_GET_INTERFACE(self)->stop != NULL);

    return MDW_PLAYER_GET_INTERFACE(self)->stop (self, error);
}



