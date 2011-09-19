/***************************************************************************
 *            mdw_default_player_decoder.c
 *
 *  Mon Apr 16 13:46:55 UTC 2007 
 *  Copyright 2007 Iuri Gomes Diniz
 *  $Id$
 *  <iuridiniz@eitv.com.br>
****************************************************************************/
#include <avcodec.h>
//#include <avformat.h>
//#include <swscale.h>
//#include <avutil.h>
#include <unistd.h>

#include "mdw_default_player_decoder.h"
#include "mdw_default_player_private.h"

#include "mdw_util.h"

/* order: priority, lower first */
enum _MdwMessageType {
    MDW_MESSAGE_TYPE_NULL = 0, 
    /* only for directfb_queue */
    MDW_MESSAGE_TYPE_FINALIZE, 
    MDW_MESSAGE_TYPE_STOP,
    MDW_MESSAGE_TYPE_RESUME,
    MDW_MESSAGE_TYPE_PUSH_FRAME,
    /* only for main_queue */
    MDW_MESSAGE_TYPE_READY,
    MDW_MESSAGE_TYPE_FINALIZED
    //MDW_MESSAGE_TYPE_PUSH_FRAME_OK
};
#include "mdw_message.h"

struct _MdwDefaultPlayerDecoder {
    GThread* ffmpeg_thread;
    GAsyncQueue* main_queue;
    GAsyncQueue* ffmpeg_queue;
    guint frames;
};

struct frame {
    guint8* data;
    gsize size;
    MdwDefaultPlayerDisplay* display;
};

static gpointer
_do_decoding(gpointer data) {
    DEBUG_INFO("Thread _do_decoding is UP");

    MdwDefaultPlayerDecoder* decoder = (MdwDefaultPlayerDecoder*) data;
    g_assert(decoder != NULL);
    g_assert(decoder->ffmpeg_queue != NULL);
    g_assert(decoder->main_queue != NULL);
    //GAsyncQueue* queue = decoder->video_queue;
    
    gboolean to_exit = FALSE;
    gboolean stopped = FALSE;

    /* ffmpeg vars */
    AVCodec *codec = NULL;
    AVCodecContext *codec_context = NULL;
    AVFrame *frame = NULL;
    AVPicture converted_picture;
    converted_picture.data[0] = NULL;
    avcodec_init();

    register_avcodec(&mpegvideo_decoder);

    codec = avcodec_find_decoder(CODEC_ID_MPEG2VIDEO);
    if (G_UNLIKELY(codec == NULL)) {
        DEBUG_ERROR("Cannot find codec CODEC_ID_MPEG2VIDEO");
        goto LABEL_finalize;
    }

    codec_context = avcodec_alloc_context();

    if (G_UNLIKELY(codec_context == NULL)) {
        DEBUG_ERROR("avcodec_alloc_context failed");
        goto LABEL_finalize;
    }

    frame = avcodec_alloc_frame();
    if (G_UNLIKELY(frame == NULL)) {
        DEBUG_ERROR("avcodec_alloc_frame failed");
        goto LABEL_finalize;
    } 
    /*converted_frame = avcodec_alloc_frame();
    if (G_UNLIKELY(converted_frame == NULL)) {
        DEBUG_ERROR("avcodec_alloc_frame failed");
        goto LABEL_finalize;
    }*/

    g_assert(codec != NULL);
    if(codec->capabilities&CODEC_CAP_TRUNCATED) {
        g_assert(codec_context != NULL);
        codec_context->flags |= CODEC_FLAG_TRUNCATED;
    }

    /* open codec */
    if (G_UNLIKELY(avcodec_open(codec_context, codec) < 0)) {
        DEBUG_ERROR("avcodec_open failed");
        goto LABEL_finalize;
    }

    DEBUG_INFO("thread _do_decoding: Sinalizing that I'm ready");
    MdwMessage *m = mdw_message_new(MDW_MESSAGE_TYPE_READY, 0);
    mdw_message_put(m, decoder->main_queue);

    while (to_exit == FALSE) {
        m = mdw_message_get(decoder->ffmpeg_queue, TRUE);
        switch(m->type) {
            case MDW_MESSAGE_TYPE_FINALIZE:
            {
                g_assert(m->size == 0 && m->buffer == NULL);
                to_exit = TRUE;
                DEBUG_INFO("thread _do_decoding: request to finalize");
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
            case MDW_MESSAGE_TYPE_PUSH_FRAME:
            {
                g_assert(m->size == sizeof(struct frame) && m->buffer != NULL);
                struct frame *f = (struct frame*) m->buffer;
                g_assert(f->size > 0);
                g_assert(f->data != NULL);
                g_assert(f->display != NULL);
                if (stopped != TRUE) {
                    gint got_picture = 0;
                    gsize size = f->size;
                    guint8* buffer = f->data;
                    while(size > 0) {
                        //printf("bal\n");
                        int len = 0;
                        len = avcodec_decode_video(codec_context, frame, &got_picture, buffer, size);
                        if (len < 0) {
                            DEBUG_WARN("Error while decoding frame");
                        }
                        if (got_picture) {

                            guint linesize = codec_context->width * 4;
                            if (G_UNLIKELY(converted_picture.data[0] == NULL || converted_picture.linesize[0] != linesize)) {
                                g_free(converted_picture.data[0]);
                                guint lines = codec_context->height;
                                converted_picture.data[0] = g_new(guint8, lines * linesize);
                                converted_picture.linesize[0] = linesize;
                            }

                            img_convert(
                                &converted_picture, PIX_FMT_RGB32,
                                (AVPicture *)frame, codec_context->pix_fmt, 
                                codec_context->width, codec_context->height
                            ); 
                            
                            _mdw_default_player_display_show_picture(
                                f->display, 
                                converted_picture.data[0], 
                                codec_context->width, codec_context->height, 4
                            );
                            gint* p_frames = (gint *) &decoder->frames;
                            g_atomic_int_inc(p_frames);

                        }
                        size -= len;
                        buffer += len;
                    }
                }
                g_free(f->data);
                //MdwMessage *m2 = mdw_message_new(MDW_MESSAGE_TYPE_PUSH_FRAME_OK, 0);
                //mdw_message_put(m2, decoder->main_queue);
                break;
            }
            default:
            {
                DEBUG_ERROR("Message of type: %d not allowed", m->type);
                g_assert_not_reached();
                //to_exit = TRUE;
                break;
            }
        }
        mdw_message_free(m, TRUE);
    }


LABEL_finalize:
    if (codec) {
        avcodec_close(codec_context);
    }
    if (codec_context) {
        av_free(codec_context);
    }
    if (frame) {
        av_free(frame);
    }
    if (converted_picture.data[0]) {
        g_free(converted_picture.data[0]);
    }

    DEBUG_INFO("thread _do_decoding dropping %d remaning messages", g_async_queue_length(decoder->ffmpeg_queue));
    g_async_queue_lock(decoder->ffmpeg_queue); 
    while((m = mdw_message_get_unlocked(decoder->ffmpeg_queue, FALSE)) != NULL) {
        mdw_message_free(m, TRUE);
    }
    m = mdw_message_new(MDW_MESSAGE_TYPE_FINALIZED, 0);
    mdw_message_put(m, decoder->main_queue);
    g_async_queue_unlock(decoder->ffmpeg_queue); 

    DEBUG_INFO("thread _do_decoding is down");
    return NULL;
}
G_GNUC_INTERNAL
gboolean 
_mdw_default_player_decoder_init(MdwDefaultPlayerDecoder** p_decoder, GError **error) 
{
    g_assert(p_decoder != NULL && *p_decoder == NULL);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);
    DEBUG_INFO("Initializing FFMPEG stuff");

    MdwDefaultPlayerDecoder* decoder = g_new0(MdwDefaultPlayerDecoder, 1);

    decoder->ffmpeg_thread = g_thread_create(_do_decoding, decoder, FALSE, error);

    if (decoder->ffmpeg_thread == NULL) {
        g_assert (error == NULL || *error != NULL);  
        DEBUG_ERROR("Cannot create video thread");
        _mdw_default_player_decoder_finalize(&decoder);
        g_assert(decoder == NULL);
        return FALSE;
    }
  
    decoder->main_queue = g_async_queue_new();
    decoder->ffmpeg_queue = g_async_queue_new();

    DEBUG_INFO("Wait for decoding thread");
    /* wait for thread init */
    MdwMessage* message;
    message = mdw_message_get(decoder->main_queue, TRUE);
    if (G_UNLIKELY(message->type == MDW_MESSAGE_TYPE_FINALIZED)) {
        mdw_message_free(message, TRUE);
        decoder->ffmpeg_thread = NULL;
        _mdw_default_player_decoder_finalize(&decoder);
        g_assert(decoder == NULL);
        MDW_DEFAULT_PLAYER_RAISE_ERROR_DECODER(error, 
            "failed to init decoding thread");
    }
    g_assert(message->type == MDW_MESSAGE_TYPE_READY);

    if (G_UNLIKELY(message->type != MDW_MESSAGE_TYPE_READY)) {
        mdw_message_free(message, TRUE);
        _mdw_default_player_decoder_finalize(&decoder);
        g_assert(decoder == NULL);
        MDW_DEFAULT_PLAYER_RAISE_ERROR_DECODER(error, 
            "failed to init decoding thread");
    }

    mdw_message_free(message, TRUE);
    g_assert(g_async_queue_length(decoder->main_queue) == 0);

    DEBUG_INFO("FFMPEG inited");
    *p_decoder = decoder;

    return TRUE;

}

G_GNUC_INTERNAL
void 
_mdw_default_player_decoder_finalize(MdwDefaultPlayerDecoder **p_decoder) 
{
    g_assert(p_decoder != NULL && *p_decoder != NULL);
    
    MdwDefaultPlayerDecoder* decoder = *p_decoder;

    DEBUG_INFO("Deinitializing FFMPEG"); 
    if (decoder->ffmpeg_thread) {
        MdwMessage* m = mdw_message_new(MDW_MESSAGE_TYPE_FINALIZE, 0);
        g_assert(decoder->ffmpeg_queue != NULL);
        mdw_message_put(m, decoder->ffmpeg_queue);
        g_assert(decoder->main_queue != NULL);
        m = mdw_message_get(decoder->main_queue, TRUE);
        if (m->type == MDW_MESSAGE_TYPE_FINALIZED) {
            /* deinitialized */
            /* g_thread_join */
        } else {
            DEBUG_ERROR("ffmpeg_thread did not return MDW_MESSAGE_TYPE_FINALIZED");
            g_assert_not_reached();
        }
        g_assert(g_async_queue_length(decoder->main_queue) == 0);
        g_assert(g_async_queue_length(decoder->ffmpeg_queue) == 0);
    }

    if (decoder->main_queue) {
        g_assert(g_async_queue_length(decoder->main_queue) == 0);
        g_async_queue_unref(decoder->main_queue);
    }
    if (decoder->ffmpeg_queue) {
        g_assert(g_async_queue_length(decoder->ffmpeg_queue) == 0);
        g_async_queue_unref(decoder->ffmpeg_queue);
    }

    g_free(decoder);
    *p_decoder = NULL;


}
G_GNUC_INTERNAL
guint 
_mdw_default_player_decoder_get_frames(MdwDefaultPlayerDecoder* decoder) 
{
    return decoder->frames;
}

G_GNUC_INTERNAL
void 
_mdw_default_player_decoder_push_pes(MdwDefaultPlayerDecoder *decoder, MdwDefaultPlayerDisplay* display, const guint8 *data, const gsize size)
{
    g_assert(decoder != NULL);
    g_assert(display != NULL);
    g_assert(data != NULL);
    g_assert(size != 0);
    g_return_if_fail(size > 8);

    if (G_UNLIKELY(
        data[0] != 0 || data[1] != 0 || data[2] != 1 || 
        data[3] < 0xE0 || data[3] == 0xFF
    )) {
        DEBUG_WARN("Invalid PES packet");
        return;
    }

    guint16 pes_length = (data[4] << 8) | data[5];
    //guint16 pes_length = m->size - 6;
    if (G_UNLIKELY(pes_length != size - 6)) {
        DEBUG_WARN("Invalid PES packet");
        return;
    }

    const guint8* payload_start = data + data[8] + 8 + 1;
    const guint8* payload_end = data + size;

    if (G_UNLIKELY(payload_start > payload_end)) {
        DEBUG_WARN("Invalid PES packet");
        return;
    }

    /* pass only the payload of PES (the ES) */
    const gsize buffer_size = payload_end - payload_start;
    const guint8* buffer = payload_start;

    MdwMessage* m = mdw_message_new(MDW_MESSAGE_TYPE_PUSH_FRAME, sizeof(struct frame));
    struct frame* f = (struct frame*) m->buffer;
    f->data = g_memdup(buffer, buffer_size);
    f->size = buffer_size;
    //f->data = g_memdup(data, size);
    //f->size = size;
    f->display = display;

    mdw_message_put(m, decoder->ffmpeg_queue);   

    //m = mdw_message_get(decoder->main_queue, TRUE);
    //g_assert(m->type == MDW_MESSAGE_TYPE_PUSH_FRAME_OK);
    //mdw_message_free(m, TRUE);

    while(g_async_queue_length(decoder->ffmpeg_queue) > 30) {
        usleep(10);
    }

}

G_GNUC_INTERNAL
void 
_mdw_default_player_decoder_stop(MdwDefaultPlayerDecoder *decoder)
{
    g_assert(decoder != NULL);
    g_assert(decoder->ffmpeg_queue != NULL);

    MdwMessage* m = mdw_message_new(MDW_MESSAGE_TYPE_STOP, 0);
    mdw_message_put(m, decoder->ffmpeg_queue);   
}

G_GNUC_INTERNAL
void 
_mdw_default_player_decoder_resume(MdwDefaultPlayerDecoder *decoder)
{
    g_assert(decoder != NULL);
    g_assert(decoder->ffmpeg_queue != NULL);

    MdwMessage* m = mdw_message_new(MDW_MESSAGE_TYPE_RESUME, 0);
    mdw_message_put(m, decoder->ffmpeg_queue);   
}
