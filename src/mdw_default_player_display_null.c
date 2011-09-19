/***************************************************************************
 *            mdw_default_player_display.c
 *
 *  Mon Apr 16 13:47:17 UTC 2007 
 *  Copyright 2007 Iuri Gomes Diniz
 *  $Id$
 *  <iuridiniz@eitv.com.br>
****************************************************************************/


#include "mdw_default_player_display.h"
#include "mdw_default_player_private.h"

struct _MdwDefaultPlayerDisplay {
    guint16 x;
    guint16 y;
    guint16 h;
    guint16 w;
    guint16 max_w;
    guint16 max_h;
    guint frames;

};
G_GNUC_INTERNAL
guint _mdw_default_player_display_get_frames(MdwDefaultPlayerDisplay *display) 
{
    g_assert(display != NULL);
    return display->frames;
}

G_GNUC_INTERNAL
void 
_mdw_default_player_display_show_picture(MdwDefaultPlayerDisplay* display, const void* picture, const guint width, const guint height, const guint bytes_per_pixel) 
{
    g_assert(display != NULL);
    g_assert(picture != NULL);
    g_assert(width > 0);
    g_assert(height > 0);
    g_assert(bytes_per_pixel > 0);
    
    display->frames++;
}


G_GNUC_INTERNAL
void 
_mdw_default_player_display_stop(MdwDefaultPlayerDisplay *display) 
{

}

G_GNUC_INTERNAL
void 
_mdw_default_player_display_resume(MdwDefaultPlayerDisplay *display) 
{

}

G_GNUC_INTERNAL
gboolean
_mdw_default_player_display_init(MdwDefaultPlayerDisplay** p_display, GError** error)
{
    g_assert(p_display != NULL && *p_display == NULL);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    MdwDefaultPlayerDisplay* display = g_new(MdwDefaultPlayerDisplay, 1);
    display->x = 0;
    display->y = 0;
    display->w = 640;
    display->h = 480;
    display->max_w = 640;
    display->max_h = 480;
    display->frames = 0;

    *p_display = display;

    return TRUE;
}

G_GNUC_INTERNAL
void
_mdw_default_player_display_finalize(MdwDefaultPlayerDisplay** p_display)
{
    g_assert(p_display != NULL && *p_display != NULL);
    g_free(*p_display);
    *p_display = NULL;
    return;
}

G_GNUC_INTERNAL
gboolean
_iface_mdw_player_fullscreen(MdwPlayer* iface, GError** error)
{
    g_return_val_if_fail(iface != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    MdwDefaultPlayerDisplay* display = MDW_DEFAULT_PLAYER_GET_DISPLAY(iface);
    if (G_UNLIKELY(display == NULL)) { 
        if (G_UNLIKELY(!_mdw_default_player_display_init(&display, error))) {
            g_assert(error == NULL || *error != NULL);
            return FALSE;
        }
        MDW_DEFAULT_PLAYER_GET_PRIVATE(iface)->display = display;
    }
    g_assert(display != NULL);

    display->x = 0;
    display->y = 0;
    display->w = display->max_w;
    display->h = display->max_h;

    return TRUE;
}

G_GNUC_INTERNAL
gboolean
_iface_mdw_player_set_pos(MdwPlayer* iface, guint16 x, guint16 y, GError** error)
{
    g_return_val_if_fail(iface != NULL, FALSE);
    MdwDefaultPlayerDisplay* display = MDW_DEFAULT_PLAYER_GET_DISPLAY(iface);

    if (display == NULL) { 
        if (!_mdw_default_player_display_init(&display, error)) {
            g_assert(error == NULL || *error != NULL);
            return FALSE;
        }
        MDW_DEFAULT_PLAYER_GET_PRIVATE(iface)->display = display;
    }
    g_assert(display != NULL);

    g_return_val_if_fail(x <= display->max_w, FALSE);
    g_return_val_if_fail(y <= display->max_h, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);
    
    display->x = x;
    display->y = y;
    return TRUE;
}

G_GNUC_INTERNAL
gboolean
_iface_mdw_player_get_pos(MdwPlayer* iface, guint16 *x, guint16 *y, GError** error)
{
    g_return_val_if_fail(iface != NULL, FALSE);
    g_return_val_if_fail(x != NULL, FALSE);
    g_return_val_if_fail(y != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);
    
    MdwDefaultPlayerDisplay* display = MDW_DEFAULT_PLAYER_GET_DISPLAY(iface);
    if (display == NULL) { 
        if (!_mdw_default_player_display_init(&display, error)) {
            g_assert(error == NULL || *error != NULL);
            return FALSE;
        }
        MDW_DEFAULT_PLAYER_GET_PRIVATE(iface)->display = display;
    }
    g_assert(display != NULL);

    
    *x = display->x;
    *y = display->y;

    return TRUE;
}

G_GNUC_INTERNAL
gboolean
_iface_mdw_player_lower_to_bottom(MdwPlayer* iface, GError** error)
{
    g_return_val_if_fail(iface != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    MdwDefaultPlayerDisplay* display = MDW_DEFAULT_PLAYER_GET_DISPLAY(iface);
    if (display == NULL) { 
        if (!_mdw_default_player_display_init(&display, error)) {
            g_assert(error == NULL || *error != NULL);
            return FALSE;
        }
        MDW_DEFAULT_PLAYER_GET_PRIVATE(iface)->display = display;
    }
    g_assert(display != NULL);

    return TRUE;
}

G_GNUC_INTERNAL
gboolean
_iface_mdw_player_raise_to_top(MdwPlayer* iface, GError** error)
{
    g_return_val_if_fail(iface != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    MdwDefaultPlayerDisplay* display = MDW_DEFAULT_PLAYER_GET_DISPLAY(iface);
    if (display == NULL) { 
        if (!_mdw_default_player_display_init(&display, error)) {
            g_assert(error == NULL || *error != NULL);
            return FALSE;
        }
        MDW_DEFAULT_PLAYER_GET_PRIVATE(iface)->display = display;
    }
    g_assert(display != NULL);

    return TRUE;
}

G_GNUC_INTERNAL
gboolean
_iface_mdw_player_set_size(MdwPlayer* iface, guint16 width, guint16 height, GError** error)
{
    g_return_val_if_fail(iface != NULL, FALSE);
    MdwDefaultPlayerDisplay* display = MDW_DEFAULT_PLAYER_GET_DISPLAY(iface);
    if (display == NULL) { 
        if (!_mdw_default_player_display_init(&display, error)) {
            g_assert(error == NULL || *error != NULL);
            return FALSE;
        }
        MDW_DEFAULT_PLAYER_GET_PRIVATE(iface)->display = display;
    }
    g_assert(display != NULL);

    g_return_val_if_fail(width <= display->max_w, FALSE);
    g_return_val_if_fail(height <= display->max_h, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);
    
    display->w = width;
    display->h = height;
    return TRUE;
}

G_GNUC_INTERNAL
gboolean
_iface_mdw_player_get_size(MdwPlayer* iface, guint16* width, guint16* height, GError** error)
{
    g_return_val_if_fail(iface != NULL, FALSE);
    g_return_val_if_fail(width != NULL, FALSE);
    g_return_val_if_fail(height != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    MdwDefaultPlayerDisplay* display = MDW_DEFAULT_PLAYER_GET_DISPLAY(iface);
    if (display == NULL) { 
        if (!_mdw_default_player_display_init(&display, error)) {
            g_assert(error == NULL || *error != NULL);
            return FALSE;
        }
        MDW_DEFAULT_PLAYER_GET_PRIVATE(iface)->display = display;
    }
    g_assert(display != NULL);

    *height = display->h;
    *width = display->w;
    
    return TRUE;
}

G_GNUC_INTERNAL
gboolean
_iface_mdw_player_get_max_size(MdwPlayer* iface, guint16* width, guint16* height, GError** error)
{
    g_return_val_if_fail(iface != NULL, FALSE);
    g_return_val_if_fail(width != NULL, FALSE);
    g_return_val_if_fail(height != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    MdwDefaultPlayerDisplay* display = MDW_DEFAULT_PLAYER_GET_DISPLAY(iface);
    if (display == NULL) { 
        if (!_mdw_default_player_display_init(&display, error)) {
            g_assert(error == NULL || *error != NULL);
            return FALSE;
        }
        MDW_DEFAULT_PLAYER_GET_PRIVATE(iface)->display = display;
    }
    g_assert(display != NULL);

    *height = display->max_h;
    *width = display->max_w;

    return TRUE;
}

