/***************************************************************************
 *            mdw_default_player_display.h
 *
 *  Mon Apr 16 13:47:11 UTC 2007 
 *  Copyright 2007 Iuri Gomes Diniz
 *  $Id$
 *  <iuridiniz@eitv.com.br>
****************************************************************************/

#ifndef _MDW_DEFAULT_PLAYER_DISPLAY_H
#define _MDW_DEFAULT_PLAYER_DISPLAY_H

#include <glib.h> 
#include "mdw_player.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
typedef struct _MdwDefaultPlayerDisplay MdwDefaultPlayerDisplay; /*dummy*/

gboolean _mdw_default_player_display_init(MdwDefaultPlayerDisplay** self, GError** error);
void _mdw_default_player_display_finalize(MdwDefaultPlayerDisplay** self);

void _mdw_default_player_display_show_picture(MdwDefaultPlayerDisplay* display, 
    const void* picture, const guint width, const guint height, const guint bytes_per_pixel);

void _mdw_default_player_display_stop(MdwDefaultPlayerDisplay *display);
void _mdw_default_player_display_resume(MdwDefaultPlayerDisplay *display);

guint _mdw_default_player_display_get_frames(MdwDefaultPlayerDisplay *display);

gboolean _iface_mdw_player_fullscreen(MdwPlayer* iface, GError** error);
gboolean _iface_mdw_player_set_pos (MdwPlayer* iface, guint16 x, guint16 y, GError** error);
gboolean _iface_mdw_player_get_pos (MdwPlayer* iface, guint16 *x, guint16 *y, GError** error);
gboolean _iface_mdw_player_lower_to_bottom(MdwPlayer* iface, GError** error);
gboolean _iface_mdw_player_raise_to_top(MdwPlayer* iface, GError** error);
gboolean _iface_mdw_player_set_size(MdwPlayer* iface, guint16 width, guint16 height, GError** error);
gboolean _iface_mdw_player_get_size(MdwPlayer* iface, guint16* width, guint16* height, GError** error);
gboolean _iface_mdw_player_get_max_size(MdwPlayer* iface, guint16* width, guint16* height, GError** error);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _MDW_DEFAULT_PLAYER_DISPLAY_H */
