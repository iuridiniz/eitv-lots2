/***************************************************************************
 *  MdwDefaultPlayer
 *  Fri Mar 30 13:00:34 2007 
 *
 *  Copyright (c) 2007 Iuri Diniz,,,
 *  Authors: Iuri Diniz,,, <iuridiniz@yahoo.com.br> 2007
 * 
 *  VERSION CONTROL INFORMATION: $Id$
 *
****************************************************************************/
/***
 * TODO: put one thread to manage all directfb events, this will fix some 
 * errors like 'resource is locked'
 */
//#include <directfb.h>
//#include <avcodec.h>
//#include <avformat.h>
//#include <swscale.h>
//#include <avutil.h>

#include "mdw_player.h"
#include "mdw_demux_data_handler.h"
#include "mdw_default_player_display.h"
#include "mdw_default_player_decoder.h"
#include "mdw_default_player_private.h"
#include "mdw_default_player.h"
#include "mdw_util.h"

/********************/
/* local prototypes */
/********************/
/* interface overrided methods */
/* demux data handler */
static void _iface_mdw_demux_data_handler_init (gpointer g_iface, gpointer data);
static gboolean _iface_mdw_demux_data_handler_new_data (MdwDemuxDataHandler *self, 
        const guint16 pid, const gsize size, const guint8* data);

/* player */
static void _iface_mdw_player_init (gpointer g_iface, gpointer data);

static MdwDemux* _iface_mdw_player_get_demux (MdwPlayer* self);
static void      _iface_mdw_player_set_demux (MdwPlayer* self, MdwDemux* demux);
//static void _iface_mdw_player_fullscreen (MdwPlayer* self);
//static void _iface_mdw_player_lower (MdwPlayer* self);
//static void _iface_mdw_player_raise (MdwPlayer* self);
//static void _iface_mdw_player_set_pos (MdwPlayer* self, guint16 x, guint16 y);
//static void _iface_mdw_player_get_pos (MdwPlayer* slef, guint16* x, guint16* y);
static void _iface_mdw_player_set_audio_pid (MdwPlayer* self, guint16 pid);
static void _iface_mdw_player_set_video_pid (MdwPlayer* self, guint16 pid);
static guint16 _iface_mdw_player_get_audio_pid (MdwPlayer* self);
static guint16 _iface_mdw_player_get_video_pid (MdwPlayer* self);
//static void _iface_mdw_player_set_size (MdwPlayer* self, guint16 width, guint16 height);
//static void _iface_mdw_player_get_size (MdwPlayer* self, guint16* width, guint16* height);
//static void _iface_mdw_player_get_max_size (MdwPlayer* self, guint16* width, guint16* height);
static gboolean _iface_mdw_player_play (MdwPlayer* self, GError** error);
static gboolean _iface_mdw_player_stop (MdwPlayer* self, GError** error);


/* destructors */
static void mdw_default_player_dispose (GObject *object);
static void mdw_default_player_finalize (GObject *object);

/* properties get/set functions*/
/*
static void mdw_default_player_set_property(GObject *object,
    guint property_id,
    const GValue *value,
    GParamSpec *pspec);
static void mdw_default_player_get_property(GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec);
*/
/* enum for enumerate properties */
/*
enum {
    MDW_DEFAULT_PLAYER_PROPERTY_1 = 1,
    MDW_DEFAULT_PLAYER_PROPERTY_2
};
*/

/* error */
GQuark
mdw_default_player_error_quark() 
{
    static GQuark quark = 0;
    if (G_UNLIKELY(quark == 0)) {
        quark = g_quark_from_static_string("MdwDefaultPlayerError");
    }
    return quark;
}


/* private methods */

/* auxiliary */
gpointer _do_video_decoding (gpointer data);

/***********************************************/
/* GObject obrigatory definations/declarations */
/***********************************************/
/* Init: define my Object (explict) */
static void     mdw_default_player_init       (MdwDefaultPlayer      *self);
static void     mdw_default_player_class_init (MdwDefaultPlayerClass *klass);
static gpointer mdw_default_player_parent_class = NULL;
static void     mdw_default_player_class_intern_init (gpointer klass)
{
  mdw_default_player_parent_class = g_type_class_peek_parent (klass);
  mdw_default_player_class_init ((MdwDefaultPlayerClass*) klass);
}

GType
mdw_default_player_get_type (void)
{
    static GType g_define_type_id = 0; 
    if (G_UNLIKELY (g_define_type_id == 0)) 
    { 
        static const GTypeInfo g_define_type_info = { 
            sizeof (MdwDefaultPlayerClass), 
            (GBaseInitFunc) NULL, 
            (GBaseFinalizeFunc) NULL, 
            (GClassInitFunc) mdw_default_player_class_intern_init, 
            (GClassFinalizeFunc) NULL, 
            NULL,   /* class_data */ 
            sizeof (MdwDefaultPlayer), 
            0,      /* n_preallocs */ 
            (GInstanceInitFunc) mdw_default_player_init, 
        }; 
        g_define_type_id = g_type_register_static (
            G_TYPE_OBJECT, //parent GType 
            "MdwDefaultPlayer", //my name in Camel case
            &g_define_type_info, 
            0
        ); 
        /* interfaces that I implement */
        {
            static const GInterfaceInfo g_implement_interface_info = {
                (GInterfaceInitFunc) _iface_mdw_demux_data_handler_init 
            };
            g_type_add_interface_static (
                g_define_type_id, 
                MDW_TYPE_DEMUX_DATA_HANDLER, 
                &g_implement_interface_info
            );
        } 
        {
            static const GInterfaceInfo g_implement_interface_info = {
                (GInterfaceInitFunc) _iface_mdw_player_init 
            };
            g_type_add_interface_static (
                g_define_type_id, 
                MDW_TYPE_PLAYER, 
                &g_implement_interface_info
            );
        } 

    } 
    return g_define_type_id; 
}

/* End: define my Object (explict) */

/*************************/
/* Internal Constructors */
/************************/
/**
 * initilize/override parent class methods, create signals, properties.
 */
static void
mdw_default_player_class_init (MdwDefaultPlayerClass *klass) 
{
    /* top ancestral */
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    /* private alocation */
    g_type_class_add_private (klass, sizeof (MdwDefaultPlayerPrivate));

    /* methods */
    //klass->method1 = mdw_default_player_method1;
    //klass->method2 = mdw_default_player_method2;

    /* destructors */
    gobject_class->dispose = mdw_default_player_dispose;
    gobject_class->finalize = mdw_default_player_finalize;

    /* properties */
    //GParamSpec *pspec;
    //gobject_class->set_property = mdw_default_player_set_property;
    //gobject_class->get_property = mdw_default_player_get_property;
    
    /* Property 1 (type: string) (contruct_only) (read/write)*/
    /*
    pspec = g_param_spec_string( // string property 
        "property1", //name
        "Property 1", //nick
        "Set/Get the property 1", //description
        "default", //default value
        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY //flags
    );
    g_object_class_install_property(
        gobject_class, 
        MDW_DEFAULT_PLAYER_PROPERTY_1,  //PROPERTY ID
        pspec 
    );
    g_param_spec_unref(pspec);
    */
    
    /* Property 2 (type: int) (read/write) (can be use in contruction)*/ 
    /*
    pspec = g_param_spec_int(
        "property2", //name
        "Property 2", //nick-name
        "Set/Get the property 2", //description
        0, //minimal value
        10, //maximal value
        0, //default value
        G_PARAM_READWRITE |G_PARAM_CONSTRUCT // flags
    );
    g_object_class_install_property(
        gobject_class, 
        MDW_DEFAULT_PLAYER_PROPERTY_2,  //PROPERTY ID
        pspec 
    );
    g_param_spec_unref(pspec);
    */

    /* signals */

    /* chain-up */

    /* threads */
    if(!g_thread_supported()) g_thread_init (NULL);
}

/**
 * GObject Internal Constructor 
 */
static void
mdw_default_player_init(MdwDefaultPlayer *self) 
{
    /* set the private members */
    //self->boolean_member = TRUE;
    //self->uint8_member = 42;

    MdwDefaultPlayerPrivate *priv = MDW_DEFAULT_PLAYER_GET_PRIVATE(self);
    //priv->decoder_queue = g_async_queue_new();
    //priv->f_inited = g_cond_new();
    //
    priv->mutex = g_mutex_new();
    //_mdw_default_player_decoder_init(&priv->decoder);
    //_mdw_default_player_display_init(&priv->display);
}

/** 
 * Interface init: So let's override methods of MdwDemuxDataHandlerInterface 
 */
static void 
_iface_mdw_demux_data_handler_init (gpointer g_iface, gpointer data) 
{
    ((MdwDemuxDataHandlerInterface*) g_iface)->new_data = 
        _iface_mdw_demux_data_handler_new_data;
}

/** 
 * Interface init: So let's override methods of MdwPlayer.
 */
static void 
_iface_mdw_player_init (gpointer g_iface, gpointer data) 
{
    MdwPlayerInterface* iface = (MdwPlayerInterface*) g_iface;

    iface->get_demux = _iface_mdw_player_get_demux;
    iface->set_demux = _iface_mdw_player_set_demux;

    iface->fullscreen = _iface_mdw_player_fullscreen;
    iface->lower_to_bottom = _iface_mdw_player_lower_to_bottom;
    iface->raise_to_top = _iface_mdw_player_raise_to_top;
    
    iface->set_pos = _iface_mdw_player_set_pos;
    iface->get_pos = _iface_mdw_player_get_pos;

    iface->set_audio_pid = _iface_mdw_player_set_audio_pid;
    iface->set_video_pid = _iface_mdw_player_set_video_pid;

    iface->get_audio_pid = _iface_mdw_player_get_audio_pid;
    iface->get_video_pid = _iface_mdw_player_get_video_pid;

    iface->set_size = _iface_mdw_player_set_size;
    iface->get_size = _iface_mdw_player_get_size;
    iface->get_max_size = _iface_mdw_player_get_max_size;
    
    iface->play = _iface_mdw_player_play;
    iface->stop = _iface_mdw_player_stop;

}


/**********************/
/* PROPERTIES GET/SET */
/**********************/
/**
 * Properties set function
 */
/*
static void 
mdw_default_player_set_property(GObject *object,
    guint property_id,
    const GValue *value,
    GParamSpec *pspec) 
{
    MdwDefaultPlayer *self = MDW_DEFAULT_PLAYER(object);
    switch (property_id) {
        case MDW_DEFAULT_PLAYER_PROPERTY_1:
            g_free(self->property1);
            self->property1 = g_value_dup_string(value);
            break;
        case MDW_DEFAULT_PLAYER_PROPERTY_2:
            self->property2 = g_value_get_int(value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }
}
*/

/**
 * Properties get function 
 */
/*
static void 
mdw_default_player_get_property(GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec) 
{
    MdwDefaultPlayer *self = MDW_DEFAULT_PLAYER(object);
    switch (property_id) {
        case MDW_DEFAULT_PLAYER_PROPERTY_1:
            g_value_set_string(value, self->property1);
            break;
        case MDW_DEFAULT_PLAYER_PROPERTY_2:
            g_value_set_int(value, self->property2);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }

}
*/



/************************/
/* Public Constructors */
/***********************/

MdwDefaultPlayer* 
mdw_default_player_new() {
    return (MdwDefaultPlayer*) g_object_new(MDW_TYPE_DEFAULT_PLAYER, NULL);
}

/*
MdwDefaultPlayer* 
mdw_default_player_new_with_arg1(guint8 param1) {
    return (MdwDefaultPlayer*) g_object_new(MDW_TYPE_DEFAULT_PLAYER, "property 1", param1, NULL);
}
*/


/***************/
/* Destructors */
/***************/
static void    
mdw_default_player_dispose (GObject *object) 
{   
    MdwDefaultPlayer *self =  MDW_DEFAULT_PLAYER(object);
    MdwDefaultPlayerPrivate *priv = MDW_DEFAULT_PLAYER_GET_PRIVATE(object);
     
    if (priv->dispose_has_run) {
        return;
    }
    priv->dispose_has_run = TRUE;
    mdw_player_set_demux(MDW_PLAYER(self),  NULL);
    if (priv->is_playing) {
        mdw_player_stop(MDW_PLAYER(self), NULL); 
    }

    /* chaining up */
    G_OBJECT_CLASS(mdw_default_player_parent_class)->dispose(object);
}
static void    
mdw_default_player_finalize (GObject *object) 
{
    MdwDefaultPlayerPrivate *priv = MDW_DEFAULT_PLAYER_GET_PRIVATE(object);
    if (priv->decoder) {
        _mdw_default_player_decoder_finalize(&priv->decoder);
        g_assert(priv->decoder == NULL);
    }
    if (priv->display) {
        _mdw_default_player_display_finalize(&priv->display);
        g_assert(priv->display == NULL);
    }
    if (priv->timer) {
        g_timer_destroy(priv->timer);
    }
    g_mutex_free(priv->mutex);    
    /* chaining up */
    G_OBJECT_CLASS(mdw_default_player_parent_class)->finalize(object);
}
/**********************/
/* Overrided methods  */
/**********************/
static gboolean
_iface_mdw_demux_data_handler_new_data (MdwDemuxDataHandler *iface, const guint16 pid, const gsize size, const guint8* data)
{
    //return FALSE;
    /* put data on queue */
    
    MdwDefaultPlayerPrivate *priv = MDW_DEFAULT_PLAYER_GET_PRIVATE(iface);
    LOCK(priv->mutex);
    {
        if (priv->display == NULL) {
            UNLOCK(priv->mutex);
            return TRUE;
        }
    }
    UNLOCK(priv->mutex);
    //struct message* m = NEW_MESSAGE(MSG_NEW_PES); 
    //m->size = size;
    //m->buffer = g_memdup(data, size);
    //PUT_MESSAGE_ON_QUEUE(m, priv->decoder_queue);
    g_assert(priv->display != NULL);
    g_assert(priv->decoder != NULL);
    _mdw_default_player_decoder_push_pes(priv->decoder, priv->display, data, size);
    if (G_UNLIKELY(priv->timer == NULL)) {
        priv->timer = g_timer_new();
        g_usleep(10000);
    }

    gdouble fps;
    do {
        gdouble time_elapsed = g_timer_elapsed(priv->timer, NULL);
        fps = _mdw_default_player_display_get_frames(priv->display) / time_elapsed;
        g_usleep(100);
        //g_debug("FPS: %lf", fps);
    /* FIXME:get the correct frame rate from decoder */
    } while (fps > 30.0);

    return TRUE;
}

static MdwDemux*
_iface_mdw_player_get_demux (MdwPlayer* iface)
{
    return MDW_DEFAULT_PLAYER_GET_PRIVATE(iface)->demux;
}

static void     
_iface_mdw_player_set_demux (MdwPlayer* iface, MdwDemux* demux)
{
    MdwDefaultPlayerPrivate *priv = MDW_DEFAULT_PLAYER_GET_PRIVATE(iface);
    g_assert(demux == NULL || MDW_IS_DEMUX(demux));
    if (demux) {
        g_object_ref(demux);
    }
    if (priv->demux) {
        g_object_unref(priv->demux);
    }
    priv->demux = demux;
}

static void
_iface_mdw_player_set_audio_pid (MdwPlayer* iface, guint16 pid)
{
    MdwDefaultPlayerPrivate *priv = MDW_DEFAULT_PLAYER_GET_PRIVATE(iface);
    g_return_if_fail(pid > 0);
    g_return_if_fail(priv->is_playing == FALSE);
    priv->audio_pid = pid;
}

static void
_iface_mdw_player_set_video_pid (MdwPlayer* iface, guint16 pid)
{
    MdwDefaultPlayerPrivate *priv = MDW_DEFAULT_PLAYER_GET_PRIVATE(iface);
    g_return_if_fail(pid > 0);
    g_return_if_fail(priv->is_playing == FALSE);
    priv->video_pid = pid;
}

static guint16
_iface_mdw_player_get_audio_pid (MdwPlayer* iface)
{
    return MDW_DEFAULT_PLAYER_GET_PRIVATE(iface)->audio_pid;
}

static guint16
_iface_mdw_player_get_video_pid (MdwPlayer* iface)
{
    return MDW_DEFAULT_PLAYER_GET_PRIVATE(iface)->video_pid;
}

static gboolean
_iface_mdw_player_play (MdwPlayer* iface, GError** error)
{
    gboolean ret = TRUE;
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE); 
    MdwDefaultPlayerPrivate *priv = MDW_DEFAULT_PLAYER_GET_PRIVATE(iface);
    MdwDefaultPlayer* self = MDW_DEFAULT_PLAYER(iface);
    
    g_return_val_if_fail(priv->demux && MDW_IS_DEMUX(priv->demux), FALSE);
    g_return_val_if_fail(priv->video_pid != 0, FALSE);
    g_return_val_if_fail(priv->audio_pid != 0, FALSE);
    g_return_val_if_fail(priv->is_playing != TRUE, FALSE);

    DEBUG_INFO("Request to play");
    if (priv->display == NULL) {
        DEBUG_INFO("Display not inited");
        ret = _mdw_default_player_display_init(&priv->display, error);
        g_assert(priv->display != NULL);
    }

    if (ret == FALSE) {
        g_assert(error == NULL || *error != NULL);
        DEBUG_ERROR("Error while trying to init display");
        return FALSE;
    }
    
    if (priv->decoder == NULL) {
        DEBUG_INFO("Mpeg decoder not inited");
        ret = _mdw_default_player_decoder_init(&priv->decoder, error);
        g_assert(priv->decoder != NULL);
    }
    if (ret == FALSE) {
        g_assert(error == NULL || *error != NULL);
        DEBUG_ERROR("Error while trying to init mpeg decoder");
        return FALSE;
    }

    _mdw_default_player_decoder_resume(priv->decoder);
    _mdw_default_player_display_resume(priv->display);

    ret = mdw_demux_add_pes_handler(priv->demux, MDW_DEMUX_DATA_HANDLER(self), priv->video_pid);
    g_assert(ret == TRUE);
    ret = mdw_demux_start(priv->demux);
    g_assert(ret == TRUE);

    priv->is_playing = TRUE;
    return TRUE;

}

static gboolean
_iface_mdw_player_stop (MdwPlayer* iface, GError** error)
{
    //return FALSE;
    gboolean ret = TRUE;
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE); 
    MdwDefaultPlayerPrivate *priv = MDW_DEFAULT_PLAYER_GET_PRIVATE(iface);
    MdwDefaultPlayer* self = MDW_DEFAULT_PLAYER(iface);
    
    g_return_val_if_fail(priv->is_playing == TRUE, FALSE);

    g_assert(priv->decoder != NULL);
    g_assert(priv->display != NULL);
    
    ret = mdw_demux_del_pes_handler(priv->demux, MDW_DEMUX_DATA_HANDLER(self), priv->video_pid);
    g_assert(ret == TRUE);

    ret = mdw_demux_stop(priv->demux);
    g_assert(ret == TRUE);
    
    _mdw_default_player_decoder_stop(priv->decoder);
    _mdw_default_player_display_stop(priv->display);

    priv->is_playing = FALSE;

    return TRUE;    
}
/*******************/
/* PRIVATE METHODS */
/*******************/

/***********************/
/* AUXILIARY FUNCTIONS */
/***********************/
/******************/
/* PUBLIC METHODS */
/******************/
