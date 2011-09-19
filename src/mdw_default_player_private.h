/***************************************************************************
 *            mdw_default_player_private.h
 *
 *  Mon Apr 16 16:47:50 UTC 2007 
 *  Copyright 2007 Iuri Gomes Diniz
 *  $Id$
 *  <iuridiniz@eitv.com.br>
****************************************************************************/

#ifndef _MDW_DEFAULT_PLAYER_PRIVATE_H
#define _MDW_DEFAULT_PLAYER_PRIVATE_H

#include <glib.h>

#include "mdw_default_player.h"
#include "mdw_default_player_display.h"
#include "mdw_default_player_decoder.h"

#include "mdw_util.h"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* private data */

#define MDW_DEFAULT_PLAYER_GET_PRIVATE(o)  \
    (G_TYPE_INSTANCE_GET_PRIVATE ((o), MDW_TYPE_DEFAULT_PLAYER, MdwDefaultPlayerPrivate))
#define MDW_DEFAULT_PLAYER_GET_DISPLAY(o)  \
    MDW_DEFAULT_PLAYER_GET_PRIVATE(o)->display
#define MDW_DEFAULT_PLAYER_GET_DECODER(o)  \
    MDW_DEFAULT_PLAYER_GET_PRIVATE(o)->decoder


typedef struct _MdwDefaultPlayerPrivate MdwDefaultPlayerPrivate;
struct _MdwDefaultPlayerPrivate {
    MdwDemux* demux;
    guint16 audio_pid;
    guint16 video_pid;
    gboolean is_playing;
    
    MdwDefaultPlayerDisplay* display;
    MdwDefaultPlayerDecoder* decoder;

    GMutex* mutex;
    GTimer* timer;

    gboolean dispose_has_run;

};

/* error */
#define MDW_DEFAULT_PLAYER_RAISE_ERROR(error, type, ...) \
    { \
        DEBUG_WARN(__VA_ARGS__); \
        g_set_error((error), MDW_DEFAULT_PLAYER_ERROR, (type), __VA_ARGS__); \
        return FALSE; \
    } while (0)
#define MDW_DEFAULT_PLAYER_RAISE_ERROR_IF_FAIL(error, cond, type, ...) \
{ \
    if (G_UNLIKELY(!(cond))) { \
        MDW_DEFAULT_PLAYER_RAISE_ERROR((error), (type)); \
    } \
} while (0)

// MDW_DEFAULT_PLAYER_ERROR_DECODER
#define MDW_DEFAULT_PLAYER_RAISE_ERROR_DECODER_IF_FAIL(error, cond, ...) \
    MDW_DEFAULT_PLAYER_RAISE_ERROR_IF_FAIL((error), (cond), MDW_DEFAULT_PLAYER_ERROR_DECODER, __VA_ARGS__)
#define MDW_DEFAULT_PLAYER_RAISE_ERROR_DECODER(error, ...) \
    MDW_DEFAULT_PLAYER_RAISE_ERROR((error), MDW_DEFAULT_PLAYER_ERROR_DECODER, __VA_ARGS__)

// MDW_DEFAULT_PLAYER_ERROR_DISPLAY
#define MDW_DEFAULT_PLAYER_RAISE_ERROR_DISPLAY_IF_FAIL(error, cond, ...) \
    MDW_DEFAULT_PLAYER_RAISE_ERROR_IF_FAIL((error), (cond), MDW_DEFAULT_PLAYER_ERROR_DISPLAY, __VA_ARGS__)
#define MDW_DEFAULT_PLAYER_RAISE_ERROR_DISPLAY(error, ...) \
    MDW_DEFAULT_PLAYER_RAISE_ERROR((error), MDW_DEFAULT_PLAYER_ERROR_DISPLAY, __VA_ARGS__)

// MDW_DEFAULT_PLAYER_ERROR_FAILED
#define MDW_DEFAULT_PLAYER_RAISE_ERROR_FAILED_IF_FAIL(error, cond, ...) \
    MDW_DEFAULT_PLAYER_RAISE_ERROR_IF_FAIL((error), (cond), MDW_DEFAULT_PLAYER_ERROR_FAILED, __VA_ARGS__)
#define MDW_DEFAULT_PLAYER_RAISE_ERROR_FAILED(error, ...) \
    MDW_DEFAULT_PLAYER_RAISE_ERROR((error), MDW_DEFAULT_PLAYER_ERROR_FAILED, __VA_ARGS__)

// MDW_DEFAULT_PLAYER_ERROR_NOT_IMPLEMENTED
#define MDW_DEFAULT_PLAYER_RAISE_ERROR_NOT_IMPLEMENTED(error, ...) \
    MDW_DEFAULT_PLAYER_RAISE_ERROR((error), MDW_DEFAULT_PLAYER_ERROR_NOT_IMPLEMENTED, "Not implemented: %s", __FUNC__)


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _MDW_DEFAULT_PLAYER_PRIVATE_H */
