/***************************************************************************
 *            mdw_default_player_decoder.h
 *
 *  Mon Apr 16 13:47:00 UTC 2007 
 *  Copyright 2007 Iuri Gomes Diniz
 *  $Id$
 *  <iuridiniz@eitv.com.br>
****************************************************************************/

#ifndef _MDW_DEFAULT_PLAYER_DECODER_H
#define _MDW_DEFAULT_PLAYER_DECODER_H

#include <glib.h>

#include "mdw_default_player.h"
#include "mdw_default_player_display.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef struct _MdwDefaultPlayerDecoder MdwDefaultPlayerDecoder; /*dummy*/

gboolean _mdw_default_player_decoder_init(MdwDefaultPlayerDecoder **self, GError** error);
void _mdw_default_player_decoder_finalize(MdwDefaultPlayerDecoder **self);

void _mdw_default_player_decoder_push_pes(MdwDefaultPlayerDecoder *decoder, MdwDefaultPlayerDisplay* display, const guint8 *data, const gsize size);
void _mdw_default_player_decoder_stop(MdwDefaultPlayerDecoder *decoder);
void _mdw_default_player_decoder_resume(MdwDefaultPlayerDecoder *decoder);
guint _mdw_default_player_decoder_get_frames(MdwDefaultPlayerDecoder* decoder);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _MDW_DEFAULT_PLAYER_DECODER_H */
