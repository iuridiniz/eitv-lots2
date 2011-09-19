/***************************************************************************
 *            mdw_default_player_decoder.c
 *
 *  Mon Apr 16 13:46:55 UTC 2007 
 *  Copyright 2007 Iuri Gomes Diniz
 *  $Id$
 *  <iuridiniz@eitv.com.br>
****************************************************************************/

#include "mdw_default_player_private.h"
#include "mdw_default_player_decoder.h"
#include "mdw_default_player_display.h"

#define WIDTH 640
#define HEIGHT 400

struct _MdwDefaultPlayerDecoder {
    guint8 i;
};

G_GNUC_INTERNAL
gboolean 
_mdw_default_player_decoder_init(MdwDefaultPlayerDecoder** p_decoder, GError **error) 
{
    g_assert(p_decoder != NULL && *p_decoder == NULL);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);
    MdwDefaultPlayerDecoder* decoder = g_new(MdwDefaultPlayerDecoder, 1);

    *p_decoder = decoder;
    
    return TRUE;
}

G_GNUC_INTERNAL
void 
_mdw_default_player_decoder_push_pes(MdwDefaultPlayerDecoder *decoder, MdwDefaultPlayerDisplay *display, const guint8 *data, const gsize size)
{
    g_assert(decoder != NULL);
    g_assert(display != NULL);
    g_assert(data != NULL);
    g_assert(size > 0 && size < 65536);
    
    //guint8 shift;

    guint8 alpha = 0xFF;
    guint8 red   = data[size/2];
    guint8 green = data[size/3];
    guint8 blue  = data[size/4];

    /*shift = (data[size/2]%5)+1;
    red += (red + shift) < 0x100?shift:-shift;

    shift = (data[size/3]%5)+1;
    green += (green + shift) < 0x100?shift:-shift;

    shift = (data[size/4]%5)+1;
    blue += (blue + shift) < 0x100?shift:-shift;*/

    guint32 argb = (alpha << 24) | (red << 16) | (green < 8) | blue;

    static guint32 pixels[WIDTH * HEIGHT];
    for(register int i = 0; i < WIDTH * HEIGHT; i++) {
        pixels[i] = argb;
    }
    
    _mdw_default_player_display_show_picture(display, pixels, WIDTH, HEIGHT, 4);

}


G_GNUC_INTERNAL
void 
_mdw_default_player_decoder_stop(MdwDefaultPlayerDecoder *decoder) 
{

}

G_GNUC_INTERNAL
void 
_mdw_default_player_decoder_resume(MdwDefaultPlayerDecoder *decoder) 
{

}

G_GNUC_INTERNAL
void 
_mdw_default_player_decoder_finalize(MdwDefaultPlayerDecoder **p_decoder) 
{
    g_assert(p_decoder != NULL && *p_decoder != NULL);

    g_free(*p_decoder);
    *p_decoder = NULL;
}

