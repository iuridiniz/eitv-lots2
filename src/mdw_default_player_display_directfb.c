/***************************************************************************
 *            mdw_default_player_display_directfb.c
 *
 *  Mon Apr 16 13:47:17 UTC 2007 
 *  Copyright 2007 Iuri Gomes Diniz
 *  $Id$
 *  <iuridiniz@eitv.com.br>
****************************************************************************/

#include <directfb.h>
#include <unistd.h>

#include "mdw_default_player_display.h"
#include "mdw_default_player_private.h"
#include "mdw_util.h"
#include "config.h"
/* order: priority, lower first */
enum _MdwMessageType {
    MDW_MESSAGE_TYPE_NULL = 0, 
    /* only for directfb_queue */
    MDW_MESSAGE_TYPE_FINALIZE, 
    MDW_MESSAGE_TYPE_UPDATE_POS,
    MDW_MESSAGE_TYPE_UPDATE_SIZE,
    MDW_MESSAGE_TYPE_RAISE_TO_TOP,
    MDW_MESSAGE_TYPE_LOWER_TO_BOTTOM,
    MDW_MESSAGE_TYPE_STOP,
    MDW_MESSAGE_TYPE_RESUME,
    MDW_MESSAGE_TYPE_DISPLAY_FRAME,
    /* only for main_queue */
    MDW_MESSAGE_TYPE_READY,
    MDW_MESSAGE_TYPE_FINALIZED
};
#include "mdw_message.h"

struct _MdwDefaultPlayerDisplay {
    IDirectFB *dfb;
    IDirectFBWindow *window;
    IDirectFBSurface *surface;
    IDirectFBDisplayLayer *layer;

    GAsyncQueue* main_queue;
    GAsyncQueue* directfb_queue;

    GThread* directfb_thread;

    guint16 x;
    guint16 y;
    guint16 h;
    guint16 w;
    guint16 max_w;
    guint16 max_h;

    guint frames;
};

struct picture {
    void* data;
    gint width;
    gint height;
    gint bytes_per_pixel;
};
static IDirectFBSurface* 
_create_surface(IDirectFB* dfb, guint width, guint height)
{
    g_assert(dfb != NULL);
    g_assert(width > 0);
    g_assert(height > 0);

    DFBResult err;

    IDirectFBSurface *ret = NULL;
    
    DFBSurfaceDescription sdesc;

    sdesc.flags = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT;
    sdesc.width = width;
    sdesc.height = height;
    sdesc.pixelformat = DSPF_RGB32;

    err = dfb->CreateSurface(dfb, &sdesc, &ret);

    if (err != DFB_OK) {
        DEBUG_WARN("DirectFB::CreateSurface failed:%s", DirectFBErrorString(err));
        return NULL;
    }

    return ret;

}
static gpointer
_do_display(gpointer data) 
{

    DFBResult err;
    DEBUG_INFO("thread _do_display inited");
    MdwDefaultPlayerDisplay* display = (MdwDefaultPlayerDisplay*) data;
    
    g_assert(display != NULL);
    g_assert(display->main_queue != NULL);
    
    gboolean to_exit = FALSE;
    gboolean stopped = FALSE;

    IDirectFBSurface *temp_surface = NULL;
    guint temp_surface_width = 0;
    guint temp_surface_height = 0;

    DEBUG_INFO("thread _do_display: Sinalizing that I'm ready");
    MdwMessage *m = mdw_message_new(MDW_MESSAGE_TYPE_READY, 0);
    mdw_message_put(m, display->main_queue);

    while (to_exit == FALSE) {
        m = mdw_message_get(display->directfb_queue, TRUE);
        switch(m->type) {
            case MDW_MESSAGE_TYPE_UPDATE_POS:
            {
                g_assert(m->size == 0 && m->buffer == NULL);
                g_assert(display->window != NULL);

                err = display->window->MoveTo(display->window, display->x, display->y);
                if (err != DFB_OK) {
                    DEBUG_WARN("Window::MoveTo failed:%s", DirectFBErrorString(err));
                }
                break;
            }
            case MDW_MESSAGE_TYPE_UPDATE_SIZE:
            {
                g_assert(m->size == 0 && m->buffer == NULL);
                g_assert(display->window != NULL);

                err = display->window->Resize(display->window, display->w, display->h);
                if (err != DFB_OK) {
                    DEBUG_WARN("Window::Resize failed:%s", DirectFBErrorString(err));
                }
                if (stopped) {
                    display->surface->Clear(display->surface, 0x30, 0x80, 0x30, 0xFF);
                    display->surface->Flip(display->surface, NULL, 0);
                }

                break;
            }
            case MDW_MESSAGE_TYPE_RAISE_TO_TOP:
            {
                g_assert(m->size == 0 && m->buffer == NULL);
                err = display->window->RaiseToTop(display->window);
                if (err != DFB_OK) {
                    DEBUG_WARN("Window::RaiseToTop failed:%s", DirectFBErrorString(err));
                }
                if (stopped) {
                    display->surface->Clear(display->surface, 0x30, 0x80, 0x30, 0xFF);
                    display->surface->Flip(display->surface, NULL, 0);
                }
                break;
            }
            case MDW_MESSAGE_TYPE_LOWER_TO_BOTTOM:
            {
                g_assert(m->size == 0 && m->buffer == NULL);
                g_assert(display->window != NULL);

                err = display->window->LowerToBottom(display->window);
                if (G_UNLIKELY(err != DFB_OK)) {
                    DEBUG_WARN("Window::LowerToBottom failed:%s", DirectFBErrorString(err));
                }
                break;
            }
            case MDW_MESSAGE_TYPE_FINALIZE:
            {
                g_assert(m->size == 0 && m->buffer == NULL);
                DEBUG_INFO("thread _do_display: request to finalize");
                to_exit = TRUE;
                break;
            }
            case MDW_MESSAGE_TYPE_STOP:
            {
                g_assert(m->size == 0 && m->buffer == NULL);

                stopped = TRUE;
                break;
            }
            case MDW_MESSAGE_TYPE_RESUME:
            {
                g_assert(m->size == 0 && m->buffer == NULL);

                stopped = FALSE;
                break;
            }
            case MDW_MESSAGE_TYPE_DISPLAY_FRAME:
            {
                g_assert(m->size == sizeof(struct picture) && m->buffer != NULL);

                struct picture *p = (struct picture*) m->buffer;
                g_assert(p->data != NULL);
                g_assert(p->width > 0);
                g_assert(p->height > 0);
                g_assert(p->bytes_per_pixel == 4);

                if (stopped != TRUE) {
                    if (G_UNLIKELY(temp_surface == NULL || temp_surface_height != p->height || temp_surface_width != p->width )) {
                        g_assert(display->dfb != NULL);
                        if (G_LIKELY(temp_surface != NULL)) {
                            temp_surface->Release(temp_surface);
                        }
                        temp_surface = _create_surface(display->dfb, p->width, p->height);
                        temp_surface_width = p->width;
                        temp_surface_height = p->height;
                    }
                    if (G_UNLIKELY(temp_surface == NULL)) {
                        goto LABEL_end_of_blit;
                    }
                    g_assert(temp_surface_width > 0);
                    g_assert(temp_surface_height > 0);
                    g_assert(temp_surface != NULL);
                        
                    //guint8* pixels;
                    void *pixels;

                    gint line_pitch;
                    const gint bytes_to_copy = p->width * p->height * p->bytes_per_pixel;

                    err = temp_surface->Lock(temp_surface, DSLF_WRITE, &pixels, &line_pitch);
                    if (G_UNLIKELY(err != DFB_OK)) {
                        DEBUG_WARN("IDirectFBSurface::Lock failed:%s", DirectFBErrorString(err));
                        goto LABEL_end_of_blit;
                    }
                    g_assert(bytes_to_copy == p->height * line_pitch);
                    memcpy(pixels, p->data, bytes_to_copy);
                    err = temp_surface->Unlock(temp_surface);
                    if (G_UNLIKELY(err != DFB_OK)) {
                        DEBUG_WARN("IDirectFBSurface::Unlock failed:%s", DirectFBErrorString(err));
                        goto LABEL_end_of_blit;
                    }
                    err = display->surface->StretchBlit(display->surface, temp_surface, NULL, NULL);
                    if (G_UNLIKELY(err != DFB_OK)) {
                        DEBUG_WARN("IDirectFBSurface::StretchBlit failed:%s", DirectFBErrorString(err));
                        goto LABEL_end_of_blit;
                    }
                    display->surface->Flip(display->surface, NULL, 0);
                    display->frames++;
                }
            LABEL_end_of_blit:
                g_free(p->data);
                break;
            }
            default:
            {
                DEBUG_ERROR("Message of type: %d not allowed", m->type);
                g_assert_not_reached();
                break;
            }

        }
        mdw_message_free(m, TRUE);
    }
    if (temp_surface) {
        temp_surface->Release(temp_surface);
    }

    DEBUG_INFO("thread _do_display dropping %d remaning messages", g_async_queue_length(display->directfb_queue));
    g_async_queue_lock(display->directfb_queue); 
    while((m = mdw_message_get_unlocked(display->directfb_queue, FALSE)) != NULL) {
        mdw_message_free(m, TRUE);
    }
    m = mdw_message_new(MDW_MESSAGE_TYPE_FINALIZED, 0);
    mdw_message_put(m, display->main_queue);
    g_async_queue_unlock(display->directfb_queue); 

    DEBUG_INFO("thread _do_display is down");
    return NULL;

}

G_GNUC_INTERNAL
guint _mdw_default_player_display_get_frames(MdwDefaultPlayerDisplay *display) 
{
    g_assert(display != NULL);
    return display->frames;
}

G_GNUC_INTERNAL
void 
_mdw_default_player_display_stop(MdwDefaultPlayerDisplay *display) {
    g_assert(display != NULL);
    g_assert(display->directfb_queue != NULL);

    MdwMessage* m = mdw_message_new(MDW_MESSAGE_TYPE_STOP, 0);
    mdw_message_put(m, display->directfb_queue);   
}

G_GNUC_INTERNAL
void 
_mdw_default_player_display_resume(MdwDefaultPlayerDisplay *display)
{
    g_assert(display != NULL);
    g_assert(display->directfb_queue != NULL);

    MdwMessage* m = mdw_message_new(MDW_MESSAGE_TYPE_RESUME, 0);
    mdw_message_put(m, display->directfb_queue);   
}

G_GNUC_INTERNAL
gboolean
_mdw_default_player_display_init(MdwDefaultPlayerDisplay** p_display, GError** error)
{
    g_assert(p_display != NULL && *p_display == NULL);
    g_assert(error == NULL || *error == NULL);

    DFBResult err;
    
    MdwDefaultPlayerDisplay* display = g_new0(MdwDefaultPlayerDisplay, 1);

    DEBUG_INFO("Initing DirectFB by DirectFBInit");
    if (G_UNLIKELY((err = DirectFBInit(NULL, NULL)) != DFB_OK)) {
        _mdw_default_player_display_finalize(&display);
        g_assert(display == NULL);
        MDW_DEFAULT_PLAYER_RAISE_ERROR_DISPLAY(error, 
            "DirectFBInit failed:%s", DirectFBErrorString(err));
    }

    DEBUG_INFO("Creating DirectFB super interface");
    if (G_UNLIKELY((err = DirectFBCreate(&display->dfb)) != DFB_OK)) {
        _mdw_default_player_display_finalize(&display);
        g_assert(display == NULL);
        MDW_DEFAULT_PLAYER_RAISE_ERROR_DISPLAY(error, 
            "DirectFBCreate failed:%s", DirectFBErrorString(err));
    }
    
    DEBUG_INFO("Getting DirectFB layer number: %u", VIDEO_LAYER);
    if (G_UNLIKELY((err = display->dfb->GetDisplayLayer(display->dfb, DLID_PRIMARY, &display->layer)) != DFB_OK)) {
        _mdw_default_player_display_finalize(&display);
        g_assert(display == 0);
        MDW_DEFAULT_PLAYER_RAISE_ERROR_DISPLAY(error, 
            "GetDisplayLayer failed:%s", DirectFBErrorString(err));
    }

    DEBUG_INFO("Getting the configuration of video layer");
    DFBDisplayLayerConfig config;
    if (G_UNLIKELY((err = display->layer->GetConfiguration(display->layer, &config)) != DFB_OK)) {
        _mdw_default_player_display_finalize(&display);
        g_assert(display == NULL);
        MDW_DEFAULT_PLAYER_RAISE_ERROR_DISPLAY(error, 
            "GetConfiguration failed:%s", DirectFBErrorString(err));
    }

    DEBUG_INFO("Maximum width %d, Maximum height: %d", config.width, config.height);
    display->max_w = config.width;
    display->max_h = config.height;

    DEBUG_INFO("Creating a window");

    DFBWindowDescription wdesc;
    wdesc.flags = DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_POSX | DWDESC_POSY | DWDESC_CAPS | DWDESC_PIXELFORMAT;
    wdesc.width = display->max_w;
    wdesc.height = display->max_h;
    wdesc.posx = 0;
    wdesc.posy = 0;
    wdesc.caps = DWCAPS_NODECORATION;
    wdesc.pixelformat = DSPF_RGB32;

    if (G_UNLIKELY((err = display->layer->CreateWindow(display->layer, &wdesc, &display->window)) != DFB_OK)) {
        _mdw_default_player_display_finalize(&display);
        g_assert(display == NULL);
        MDW_DEFAULT_PLAYER_RAISE_ERROR_DISPLAY(error, 
            "CreateWindow failed:%s", DirectFBErrorString(err));
    }
    display->window->SetOpacity(display->window, 0xFF);
    display->w = wdesc.width;
    display->h = wdesc.height;
    display->x = wdesc.posx;
    display->y = wdesc.posy;

    DEBUG_INFO("Getting the surface associated with my window");
    if (G_UNLIKELY((err = display->window->GetSurface(display->window, &display->surface)) != DFB_OK)) {
        _mdw_default_player_display_finalize(&display);
        g_assert(display == NULL);
        MDW_DEFAULT_PLAYER_RAISE_ERROR_DISPLAY(error, 
            "GetSurface failed:%s", DirectFBErrorString(err));
    }
    
    display->surface->Clear(display->surface, 0x30, 0x80, 0x30, 0xFF);
    display->surface->Flip(display->surface, NULL, 0);

    DEBUG_INFO("DirectFB full inited");
    *p_display = display;

    display->main_queue = g_async_queue_new();
    display->directfb_queue = g_async_queue_new();

    DEBUG_INFO("Initing display thread");

    display->directfb_thread = g_thread_create(_do_display, display, FALSE, error);

    if (G_UNLIKELY(display->directfb_thread == NULL)) {
        g_assert (error == NULL || *error != NULL);  
        _mdw_default_player_display_finalize(&display);
        g_assert(display == NULL);
        /*MDW_DEFAULT_PLAYER_RAISE_ERROR_DISPLAY(error, 
            "g_thread_create failed");*/
        return FALSE;
    }

    /* wait for thread init */
    /* TODO: implement like mdw_default_player_decoder_ffmpeg.c to handler thread errors while initing */
    MdwMessage* message;
    message = mdw_message_get(display->main_queue, TRUE);
    if (message->type != MDW_MESSAGE_TYPE_READY) {
        mdw_message_free(message, TRUE);
        _mdw_default_player_display_finalize(&display);
        g_assert(display == NULL);
        MDW_DEFAULT_PLAYER_RAISE_ERROR_DISPLAY(error, 
            "failed to init directfb thread");
    }
    mdw_message_free(message, TRUE);
    g_assert(g_async_queue_length(display->main_queue) == 0);

    return TRUE;
}

G_GNUC_INTERNAL
void
_mdw_default_player_display_show_picture(
    MdwDefaultPlayerDisplay* display, 
    const void* picture, 
    const guint width, const guint height, const guint bytes_per_pixel) 
{
    g_assert(display != NULL);
    g_assert(picture != NULL);
    g_assert(width != 0);
    g_assert(height != 0);
    g_assert(bytes_per_pixel != 0);
    

    MdwMessage* m = mdw_message_new(MDW_MESSAGE_TYPE_DISPLAY_FRAME, sizeof(struct picture));
    struct picture* p = (struct picture*) m->buffer;
    p->data = g_memdup(picture, width * height * bytes_per_pixel);
    p->width = width;
    p->height = height;
    p->bytes_per_pixel = bytes_per_pixel;
    g_assert(display->directfb_queue != NULL);
    
    mdw_message_put(m, display->directfb_queue);

    while(g_async_queue_length(display->directfb_queue) > 30) {
        usleep(10);
    }
}

G_GNUC_INTERNAL
void
_mdw_default_player_display_finalize(MdwDefaultPlayerDisplay** p_display)
{
    g_assert(p_display != NULL && *p_display != NULL);
    MdwDefaultPlayerDisplay* display = *p_display;

    DEBUG_INFO("Deinitializing DirectFB"); 
    if (display->directfb_thread) {
        MdwMessage* m = mdw_message_new(MDW_MESSAGE_TYPE_FINALIZE, 0);
        mdw_message_put(m, display->directfb_queue);
        g_assert(display->main_queue != NULL);
        m = mdw_message_get(display->main_queue, TRUE);
        if (m->type == MDW_MESSAGE_TYPE_FINALIZED) {
            /* deinitialized */
            /* g_thread_join ? */
        } else {
            DEBUG_ERROR("directfb_thread did not return MDW_MESSAGE_TYPE_FINALIZED");
            g_assert_not_reached();
        }
        g_assert(g_async_queue_length(display->main_queue) == 0);
        g_assert(g_async_queue_length(display->directfb_queue) == 0);
    }
    if (display->surface) {
        display->surface->Release(display->surface);
    }
    if (display->window) {
        display->window->Release(display->window);
    }
    if (display->layer) {
        display->layer->Release(display->layer);
    }
    if (display->dfb) {
        display->dfb->Release(display->dfb);
    }
    DEBUG_INFO("DirectFB released"); 
    if (display->main_queue) {
        g_assert(g_async_queue_length(display->main_queue) == 0);
        g_async_queue_unref(display->main_queue);
    }
    if (display->directfb_queue) {
        g_assert(g_async_queue_length(display->directfb_queue) == 0);
        g_async_queue_unref(display->directfb_queue);
    }
    g_free(display);
    *p_display = NULL;

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

    gboolean ret;
    ret = _iface_mdw_player_set_size(iface, display->max_w, display->max_h, error);
    if (ret == FALSE) {
        g_assert(error == NULL || *error != NULL);
        return FALSE;
    }
    return _iface_mdw_player_set_pos(iface, 0, 0, error);

}

G_GNUC_INTERNAL
gboolean
_iface_mdw_player_set_pos(MdwPlayer* iface, guint16 x, guint16 y, GError** error)
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

    g_return_val_if_fail(x <= display->max_w, FALSE);
    g_return_val_if_fail(y <= display->max_h, FALSE);

    display->x = x;
    display->y = y;

    MdwMessage* m = mdw_message_new(MDW_MESSAGE_TYPE_UPDATE_POS, 0);
    
    mdw_message_put(m, display->directfb_queue);

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
    if (G_UNLIKELY(display == NULL)) { 
        if (G_UNLIKELY(!_mdw_default_player_display_init(&display, error))) {
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
    if (G_UNLIKELY(display == NULL)) { 
        if (G_UNLIKELY(!_mdw_default_player_display_init(&display, error))) {
            g_assert(error == NULL || *error != NULL);
            return FALSE;
        }
        MDW_DEFAULT_PLAYER_GET_PRIVATE(iface)->display = display;
    }
    g_assert(display != NULL);

    MdwMessage* m = mdw_message_new(MDW_MESSAGE_TYPE_LOWER_TO_BOTTOM, 0);
    
    mdw_message_put(m, display->directfb_queue);

    return TRUE;
}

G_GNUC_INTERNAL
gboolean
_iface_mdw_player_raise_to_top(MdwPlayer* iface, GError** error)
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

    MdwMessage* m = mdw_message_new(MDW_MESSAGE_TYPE_RAISE_TO_TOP, 0);
    
    mdw_message_put(m, display->directfb_queue);

    return TRUE;
}

G_GNUC_INTERNAL
gboolean
_iface_mdw_player_set_size(MdwPlayer* iface, guint16 width, guint16 height, GError** error)
{
    g_return_val_if_fail(iface != NULL, FALSE);
    MdwDefaultPlayerDisplay* display = MDW_DEFAULT_PLAYER_GET_DISPLAY(iface);
    if (G_UNLIKELY(display == NULL)) { 
        if (G_UNLIKELY(!_mdw_default_player_display_init(&display, error))) {
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

    MdwMessage* m = mdw_message_new(MDW_MESSAGE_TYPE_UPDATE_SIZE, 0);
    mdw_message_put(m, display->directfb_queue);

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
    if (G_UNLIKELY(display == NULL)) { 
        if (G_UNLIKELY(!_mdw_default_player_display_init(&display, error))) {
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
    if (G_UNLIKELY(display == NULL)) { 
        if (G_UNLIKELY(!_mdw_default_player_display_init(&display, error))) {
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

