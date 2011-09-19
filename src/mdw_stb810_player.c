/***************************************************************************
 *  MdwStb810Player
 *  Thu Mar 22 07:33:28 2007 
 *
 *  Copyright (c) 2007 iuri,,,
 *  Authors: iuri,,, <iuri@email.com.br> 2007
 * 
 *  VERSION CONTROL INFORMATION: $Id$
 *
****************************************************************************/

#include <directfb.h>

#include "mdw_demux.h"
#include "mdw_player.h"
#include "mdw_dvb_audio_dummy.h"
#include "mdw_dvb_video_dummy.h"

#include "mdw_stb810_player.h"
#include "mdw_util.h"

#include "config.h"


/*************************/
/* local prototypes/data */
/*************************/

/* interfaces */
static void _iface_mdw_player_init (gpointer g_iface, gpointer data);

/* interface methods */
static MdwDemux* _iface_mdw_player_get_demux (MdwPlayer* self);
static void      _iface_mdw_player_set_demux (MdwPlayer* self, MdwDemux* demux);

static gboolean _iface_mdw_player_fullscreen (MdwPlayer* self, GError** error);
static gboolean _iface_mdw_player_lower_to_bottom (MdwPlayer* self, GError** error);
static gboolean _iface_mdw_player_raise_to_top (MdwPlayer* self, GError** error);

static gboolean _iface_mdw_player_set_pos (MdwPlayer* self, guint16 x, guint16 y, GError** error);
static gboolean _iface_mdw_player_get_pos (MdwPlayer* slef, guint16* x, guint16* y, GError** error);

static guint16 _iface_mdw_player_get_audio_pid (MdwPlayer* self);
static void _iface_mdw_player_set_audio_pid (MdwPlayer* self, guint16 pid);

static guint16 _iface_mdw_player_get_video_pid (MdwPlayer* self);
static void _iface_mdw_player_set_video_pid (MdwPlayer* self, guint16 pid);

static gboolean _iface_mdw_player_set_size (MdwPlayer* self, guint16 width, guint16 height, GError** error);
static gboolean _iface_mdw_player_get_size (MdwPlayer* self, guint16* width, guint16* height, GError** error);
static gboolean _iface_mdw_player_get_max_size (MdwPlayer* self, guint16* width, guint16* height, GError** error);

static gboolean _iface_mdw_player_play (MdwPlayer* self, GError** error);
static gboolean _iface_mdw_player_stop (MdwPlayer* self, GError** error);


/* destructors */
static void mdw_stb810_player_dispose (GObject *object);
static void mdw_stb810_player_finalize (GObject *object);

/* properties get/set functions*/
static void mdw_stb810_player_set_property(GObject *object,
    guint property_id,
    const GValue *value,
    GParamSpec *pspec);
static void mdw_stb810_player_get_property(GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec);

/* directfb init and deinit */
static void mdw_stb810_player_directfb_init(MdwStb810Player* self);
static void mdw_stb810_player_directfb_finalize(MdwStb810Player* self);
static void mdw_stb810_player_directfb_set_vp(MdwStb810Player* self, IDirectFBVideoProvider* vp);

/* others */
static void mdw_stb810_player_teardown_pes_handlers(MdwStb810Player *self);
static void mdw_stb810_player_setup_pes_handlers(MdwStb810Player* self);

/* enum for enumerate properties */
enum {
    MDW_STB810_PLAYER_LAYER = 1
};


typedef struct _MdwStb810PlayerPrivate MdwStb810PlayerPrivate;

/* hide private data related to directfb */
struct _MdwStb810PlayerPrivate {

    IDirectFB* dfb;
    IDirectFBDisplayLayer* video_layer;
    IDirectFBSurface* video_surface;
    IDirectFBVideoProvider* video_provider;

    guint16 max_w;
    guint16 max_h;

    guint8 layer;
    gint16 level;

    MdwDemux* demux; /**< the Demux instance */

    MdwDvbAudioDummy* audio_decoder; /**< the audio decoder instance */
    MdwDvbVideoDummy* video_decoder; /**< the video decoder instance */
    
    guint16 audio_pid;
    guint16 old_audio_pid;

    guint16 video_pid;
    guint16 old_video_pid;

    guint16 x;
    guint16 y;
    guint16 w;
    guint16 h;

    gboolean dispose_has_run;
    
    gboolean is_playing;

};
#define MDW_STB810_PLAYER_GET_PRIVATE(o)  \
   (G_TYPE_INSTANCE_GET_PRIVATE ((o), MDW_TYPE_STB810_PLAYER, MdwStb810PlayerPrivate))


/* interface */

static void _iface_mdw_player_init (gpointer g_iface, gpointer data) {
    MdwPlayerInterface* iface = (MdwPlayerInterface *) g_iface;

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
/***********************************************/
/* GObject obrigatory definations/declarations */
/***********************************************/

/* define my Object */

G_DEFINE_TYPE_EXTENDED(
    MdwStb810Player, //my name in Camel case.
    mdw_stb810_player, // my name in lowercase, with words separated by '_'.
    G_TYPE_OBJECT, //GType of my parent 
    0,
    G_IMPLEMENT_INTERFACE(MDW_TYPE_PLAYER, _iface_mdw_player_init)   
)


/*************************/
/* Internal Constructors */
/************************/
/**
 * initilize/override parent class methods, create signals, properties.
 */
static void
mdw_stb810_player_class_init (MdwStb810PlayerClass *klass) 
{
    /* top ancestral */
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    /* private */
    g_type_class_add_private (gobject_class, sizeof (MdwStb810PlayerPrivate));

    /* methods */
    klass->get_level = mdw_stb810_player_get_level;
    klass->set_level = mdw_stb810_player_set_level;
    
    /* destructors */
    gobject_class->dispose = mdw_stb810_player_dispose;
    gobject_class->finalize = mdw_stb810_player_finalize;

    /* properties */
    GParamSpec *pspec;
    gobject_class->set_property = mdw_stb810_player_set_property;
    gobject_class->get_property = mdw_stb810_player_get_property;
    
    /* Property layer (type: int) (read/write) (only in contruction)*/ 
    pspec = g_param_spec_uint(
        "layer", //name
        "Layer ID", //nick-name
        "Set/Get the layer", //description
        3, //minimal value
        4, //maximal value
        VIDEO_LAYER==4?4:3, //default value
        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY // flags
    );
    g_object_class_install_property(
        gobject_class, 
        MDW_STB810_PLAYER_LAYER,  //PROPERTY ID
        pspec 
    );
    g_param_spec_unref(pspec);

}

/**
 * GObject Internal Constructor 
 */
static void
mdw_stb810_player_init(MdwStb810Player *self) 
{
    /* set the private members */
    //priv->audio_decoder = mdw_
    
}

/**********************/
/* PROPERTIES GET/SET */
/**********************/
/**
 * Properties set function
 */

static void 
mdw_stb810_player_set_property(GObject *object,
    guint property_id,
    const GValue *value,
    GParamSpec *pspec) 
{
    MdwStb810Player *self = MDW_STB810_PLAYER(object);
    MdwStb810PlayerPrivate* priv = MDW_STB810_PLAYER_GET_PRIVATE(self);
    switch (property_id) {
        case MDW_STB810_PLAYER_LAYER:
            priv->layer = g_value_get_uint(value);
            //DEBUG_INFO("Setting Layer to %u", priv->layer);
            /* this property is construct only, can I init directfb now */
            mdw_stb810_player_directfb_init(self);
            _iface_mdw_player_fullscreen(MDW_PLAYER(self), NULL);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }
}

/**
 * Properties get function 
 */
static void 
mdw_stb810_player_get_property(GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec) 
{
    MdwStb810PlayerPrivate* priv = MDW_STB810_PLAYER_GET_PRIVATE(object);
    switch (property_id) {
        case MDW_STB810_PLAYER_LAYER:
            g_value_set_uint(value, priv->layer);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }

}



/************************/
/* Public Constructors */
/***********************/

MdwStb810Player* 
mdw_stb810_player_new() {
    DEBUG_INFO("Creating a MdwStb810Player with layer: %d", VIDEO_LAYER);

    MdwStb810Player* self;
    self = (MdwStb810Player*) g_object_new(MDW_TYPE_STB810_PLAYER, NULL);

    return self;
}

MdwStb810Player* 
mdw_stb810_player_new_with_layer(guint8 layer) {

    DEBUG_INFO("Creating a MdwStb810Player with layer: %d", layer);

    MdwStb810Player* self;
    self = (MdwStb810Player*) g_object_new(MDW_TYPE_STB810_PLAYER, "layer", layer, NULL);
    return self;
}


/***************/
/* Destructors */
/***************/
static void    
mdw_stb810_player_dispose (GObject *object) 
{   
    MdwStb810Player *self = MDW_STB810_PLAYER(object);

    MdwStb810PlayerPrivate* priv = MDW_STB810_PLAYER_GET_PRIVATE(object);
    
    _iface_mdw_player_stop(MDW_PLAYER(self), NULL);

    if (priv->dispose_has_run) {
        return;
    }
    priv->dispose_has_run = TRUE;

    if (priv->demux) {
        _iface_mdw_player_set_demux(MDW_PLAYER(self), NULL);
    }
    if (priv->audio_decoder) {
        g_object_unref(priv->audio_decoder);
        priv->audio_decoder = NULL;
    }
    if (priv->video_decoder) {
        g_object_unref(priv->video_decoder);
        priv->video_decoder = NULL;
    }
   
    /* chaining up */
    G_OBJECT_CLASS(mdw_stb810_player_parent_class)->dispose(object);
}
static void    
mdw_stb810_player_finalize (GObject *object) 
{

    MdwStb810Player *self = MDW_STB810_PLAYER(object);
    mdw_stb810_player_directfb_finalize(self);

    /* chaining up */
    G_OBJECT_CLASS(mdw_stb810_player_parent_class)->finalize(object);
}

/*******************/
/* PRIVATE METHODS */
/*******************/
void 
mdw_stb810_player_directfb_init(MdwStb810Player* self) 
{
    int level = 0;
    //int width = 0;
    //int height = 0;

    MdwStb810PlayerPrivate* priv = MDW_STB810_PLAYER_GET_PRIVATE(self);

    DFBResult err;
    DFBDisplayLayerConfig config;

    IDirectFBDisplayLayer* layer = NULL;
    IDirectFB* dfb = NULL;
    //IDirectFBScreen* screen = NULL;
    IDirectFBSurface* surface = NULL;

    DEBUG_INFO("Initializing DirectFB stuff");

    DEBUG_INFO("Initing DirectFB by DirectFBInit");
    if ((err = DirectFBInit(NULL, NULL)) != DFB_OK) {
        DEBUG_ERROR("DirectFBInit failed:%s", DirectFBErrorString(err));
        return;
    }

    DEBUG_INFO("Creating DirectFB super interface");
    if ((err = DirectFBCreate(&dfb)) != DFB_OK) {
        DEBUG_ERROR("DirectFBCreate failed:%s", DirectFBErrorString(err));
        return;
    }
    priv->dfb = dfb;

    DEBUG_INFO("Getting DirectFB video layer number: %u", priv->layer);
    if ((err = dfb->GetDisplayLayer(dfb, priv->layer, &layer)) != DFB_OK) {
        DEBUG_ERROR("GetDisplayLayer failed:%s", DirectFBErrorString(err));
        mdw_stb810_player_directfb_finalize(self);
        return;
    }
    priv->video_layer = layer;

    DEBUG_INFO("Setting the cooperative Level");
    if ((err = layer->SetCooperativeLevel(layer, DLSCL_EXCLUSIVE)) != DFB_OK) {
        DEBUG_WARN("SetCooperativeLevel failed:%s", DirectFBErrorString(err));
    }

    DEBUG_INFO("Getting the Surface of video layer");
    if ((err = layer->GetSurface(layer, &surface)) != DFB_OK) {
        DEBUG_ERROR("GetDisplayLayer failed:%s", DirectFBErrorString(err));
        mdw_stb810_player_directfb_finalize(self);
        return;
    }
    priv->video_surface = surface;
    
    DEBUG_INFO("Getting the default level of video layer");
    if ((err = layer->GetLevel(layer, &level))!=DFB_OK) {
        DEBUG_WARN("Cannot get default level of layer %u:%s", priv->layer, DirectFBErrorString(err));
    }
    DEBUG_INFO("The default level of video layer %u is %d", priv->layer, level);
    priv->level = level;

    //DEBUG_INFO("Getting the screen size of video layer");
    //if ((err = layer->GetScreen(layer, &screen)) != DFB_OK) {
    //    DEBUG_ERROR("GetScreen failed:%s", DirectFBErrorString(err));
    //    mdw_stb810_player_directfb_finalize(self);
    //    return;
    //}
    //if ((err = screen->GetSize(screen, &width, &height)) != DFB_OK) {
    //    DEBUG_WARN("GetSize failed:%s", DirectFBErrorString(err));
    //}

    DEBUG_INFO("Getting the configuration of video layer");
    if ((err = layer->GetConfiguration(layer, &config)) != DFB_OK) {
        DEBUG_ERROR("GetConfiguration failed:%s", DirectFBErrorString(err));
        mdw_stb810_player_directfb_finalize(self);
        return;
    }
    
    DEBUG_INFO("Maximum width %d, Maximum height: %d", config.width, config.height);
    priv->max_w = config.width;
    priv->max_h = config.height;
    
}
void
mdw_stb810_player_directfb_finalize(MdwStb810Player* self)
{

    MdwStb810PlayerPrivate* priv = MDW_STB810_PLAYER_GET_PRIVATE(self);

    if (priv->video_provider) {
        priv->video_provider->Stop(priv->video_provider);
        priv->video_provider->Release(priv->video_provider);
        priv->video_provider = NULL;
    }
    if (priv->video_surface) {
        priv->video_surface->Release(priv->video_surface);
        priv->video_surface = NULL;
    }
    if (priv->video_layer) {
        priv->video_layer->Release(priv->video_layer);
        priv->video_layer = NULL;
    }
    if(priv->dfb) {
        priv->dfb->Release(priv->dfb);
        priv->dfb = NULL;
    }

}

static void 
mdw_stb810_player_directfb_set_vp(MdwStb810Player* self, IDirectFBVideoProvider* vp) 
{
    MdwStb810PlayerPrivate* priv = MDW_STB810_PLAYER_GET_PRIVATE(self);

    if (priv->video_provider) {
        priv->video_provider->Release(priv->video_provider);
    }
    priv->video_provider = vp;

}

static void
mdw_stb810_player_teardown_pes_handlers(MdwStb810Player *self) 
{
    gboolean ret;

    MdwStb810PlayerPrivate* priv = MDW_STB810_PLAYER_GET_PRIVATE(self);

    if (G_LIKELY(priv->audio_decoder)) {
        /* remove from demux handlers */
        ret = mdw_demux_del_pes_handler(
            MDW_DEMUX(priv->demux), 
            MDW_DEMUX_DATA_HANDLER(priv->audio_decoder),
            priv->old_audio_pid
        );
        if (ret == FALSE) {
            DEBUG_WARN("Cannot remove previous AUDIO handler for pid %u", priv->old_audio_pid);
        }

        g_object_unref(priv->audio_decoder);
        priv->audio_decoder = NULL;
       
    }
    if (G_LIKELY(priv->video_decoder)) {
        ret = mdw_demux_del_pes_handler(
            MDW_DEMUX(priv->demux), 
            MDW_DEMUX_DATA_HANDLER(priv->video_decoder),
            priv->old_video_pid
        );
        if (ret == FALSE) {
            DEBUG_WARN("Cannot remove previous VIDEO handler for pid %u", priv->old_video_pid);
        }
        g_object_unref(priv->video_decoder);
        priv->video_decoder = NULL;
    }

}
static void 
mdw_stb810_player_setup_pes_handlers(MdwStb810Player* self) 
{
    gboolean ret;
    MdwStb810PlayerPrivate* priv = MDW_STB810_PLAYER_GET_PRIVATE(self);

    priv->audio_decoder = mdw_dvb_audio_dummy_new();
    if (G_UNLIKELY(priv->audio_decoder == NULL)) {
        DEBUG_ERROR("Cannot create a audio decoder");
        return;
    }
    priv->video_decoder = mdw_dvb_video_dummy_new();

    if (G_UNLIKELY(priv->video_decoder == NULL)) {
        DEBUG_ERROR("Cannot create a video decoder");
        return;
    }

    ret = mdw_demux_add_pes_handler(
        MDW_DEMUX(priv->demux), 
        MDW_DEMUX_DATA_HANDLER(priv->video_decoder),
        priv->video_pid
    );

    if (G_UNLIKELY(ret == FALSE)) {
        DEBUG_ERROR("Cannot add a pes handler for pid %u", priv->video_pid);
        return;
    }
    priv->old_video_pid = priv->video_pid;

    ret = mdw_demux_add_pes_handler(
        MDW_DEMUX(priv->demux), 
        MDW_DEMUX_DATA_HANDLER(priv->audio_decoder),
        priv->audio_pid
    );

    if (G_UNLIKELY(ret == FALSE)) {
        DEBUG_ERROR("Cannot add a pes handler for pid %u", priv->audio_pid);
        return;
    }
    priv->old_audio_pid = priv->audio_pid;

}
/******************/
/* PUBLIC METHODS */
/******************/
gint16
mdw_stb810_player_get_level (MdwStb810Player* self)
{
    /** 
     * @warn It's possible to another process to change the video level, 
     *       so priv->level may not reflect the actual level.
     */
    return MDW_STB810_PLAYER_GET_PRIVATE(self)->level; 
}
gboolean
mdw_stb810_player_set_level (MdwStb810Player* self, guint16 level, GError** error)
{

    MdwStb810PlayerPrivate* priv = MDW_STB810_PLAYER_GET_PRIVATE(self); 
    DFBResult err;

    IDirectFBDisplayLayer *layer = priv->video_layer;

    DEBUG_INFO("Trying to change the level of video layer");
    DEBUG_INFO("Old video layer = %d", priv->level);

    if (layer == NULL) {
        DEBUG_ERROR("Cannot change the level of video layer because layer is NULL");
        return FALSE;
    }
   
    if ((err = layer->SetLevel(layer, level)) != DFB_OK) {
        DEBUG_ERROR("Cannot change video level to %d:%s", level, DirectFBErrorString(err));
        return FALSE;
    }
    priv->level = level;

    DEBUG_INFO("New video level = %d", priv->level);
    DEBUG_INFO("The level of video player was changed");
    
    return TRUE;
}

/*********************/
/* OVERRIDED METHODS */
/*********************/
MdwDemux* 
_iface_mdw_player_get_demux (MdwPlayer* iface) 
{
    return MDW_STB810_PLAYER_GET_PRIVATE(iface)->demux;   
}

void 
_iface_mdw_player_set_demux (MdwPlayer* iface, MdwDemux* demux)
{
    MdwStb810PlayerPrivate* priv = MDW_STB810_PLAYER_GET_PRIVATE(iface);
    if (demux && ! MDW_IS_DVB_DEMUX(demux)) {
        DEBUG_ERROR("Cannot use a demux of this type, it must be a MdwDvbDemux");
        return;
    }

    if (priv->demux) {
        g_object_unref(priv->demux);
    }
    if (demux) {
        g_object_ref(demux);
    }
    priv->demux = demux;
    /* emit a signal */
}


gboolean 
_iface_mdw_player_fullscreen (MdwPlayer* iface, GError** error) {
    MdwStb810PlayerPrivate* priv = MDW_STB810_PLAYER_GET_PRIVATE(iface);
    DFBResult err;
    IDirectFBDisplayLayer* layer = priv->video_layer;
    guint16 max_w = priv->max_w;
    guint16 max_h = priv->max_h;

    DEBUG_INFO("Trying to switch to fullscreen");
    if (layer == NULL) {
        DEBUG_ERROR("Cannot switch to fullscreen because Layer is NULL");
        return FALSE;
    }
    DEBUG_INFO("Old pos and size: %u-%u:%ux%u", priv->x, priv->y, priv->w, priv->h);
    if ((err = layer->SetScreenRectangle(layer, 0, 0, max_w, max_h))!=DFB_OK) {
        DEBUG_ERROR("Cannot change to fullscren:%s", DirectFBErrorString(err));
        return FALSE;
    }

    priv->x = priv->y = 0;
    priv->w = max_w;
    priv->h = max_h;

    DEBUG_INFO("New pos and size: %u-%u:%ux%u", priv->x, priv->y, priv->w, priv->h);
    DEBUG_INFO("Switched to full screen");
    
    return TRUE;
}


gboolean
_iface_mdw_player_lower_to_bottom (MdwPlayer* iface, GError** error) 
{
    return mdw_stb810_player_set_level(MDW_STB810_PLAYER(iface), MDW_STB810_PLAYER_GET_PRIVATE(iface)->level - 1, error);
}

gboolean
_iface_mdw_player_raise_to_top (MdwPlayer* iface, GError** error) 
{
    return mdw_stb810_player_set_level(MDW_STB810_PLAYER(iface), MDW_STB810_PLAYER_GET_PRIVATE(iface)->level + 1, error);
}

gboolean
_iface_mdw_player_set_pos (MdwPlayer* iface, guint16 x, guint16 y, GError** error)
{

    MdwStb810PlayerPrivate* priv = MDW_STB810_PLAYER_GET_PRIVATE(iface);

    DFBResult err;
    IDirectFBDisplayLayer* layer = priv->video_layer;

    DEBUG_INFO("Trying to change the coordinates of video");
    if (layer == NULL) {
        DEBUG_ERROR("Cannot change coordinates because Layer is NULL");
        return FALSE;
    }
    DEBUG_INFO("Old pos: %u-%u", priv->x, priv->y);
    if ((err = layer->SetScreenRectangle(layer, x, y, priv->w, priv->h))!=DFB_OK) {
        DEBUG_ERROR("Cannot change coordinates:%s", DirectFBErrorString(err));
        return FALSE;
    }

    priv->x = x;
    priv->y = y;

    DEBUG_INFO("New pos : %u-%u", priv->x, priv->y);
    DEBUG_INFO("Coordinates changed");
    return TRUE;

}

gboolean
_iface_mdw_player_get_pos (MdwPlayer* iface, guint16* x, guint16* y, GError** error)
{
    MdwStb810PlayerPrivate* priv = MDW_STB810_PLAYER_GET_PRIVATE(iface);

    if (x != NULL) {
        *x = priv->x;
    } 
    if (y != NULL) {
        *y = priv->y;
    }
    return TRUE;
}

gboolean
_iface_mdw_player_set_size (MdwPlayer* iface, guint16 width, guint16 height, GError** error)
{
    MdwStb810PlayerPrivate* priv = MDW_STB810_PLAYER_GET_PRIVATE(iface);
    DFBResult err;
    IDirectFBDisplayLayer* layer = priv->video_layer;

    DEBUG_INFO("Trying to change the size of video");
    if (layer == NULL) {
        DEBUG_ERROR("Cannot change size because Layer is NULL");
        return FALSE;
    }
    DEBUG_INFO("Old size: %ux%u", priv->w, priv->h);
    if ((err = layer->SetScreenRectangle(layer, priv->x, priv->y, width, height))!=DFB_OK) {
        DEBUG_ERROR("Cannot change coordinates:%s", DirectFBErrorString(err));
        return FALSE;
    }

    priv->w = width;
    priv->h = height;

    DEBUG_INFO("New size : %ux%u", priv->w, priv->h);
    DEBUG_INFO("The size of video was changed");
    
    return TRUE;
}

gboolean
_iface_mdw_player_get_size (MdwPlayer* iface, guint16* width, guint16* height, GError** error)
{

    MdwStb810PlayerPrivate* priv = MDW_STB810_PLAYER_GET_PRIVATE(iface);
    *width = priv->w;
    *height = priv->h;

    return TRUE;
}

gboolean
_iface_mdw_player_get_max_size (MdwPlayer* iface, guint16* width, guint16* height, GError** error)
{

    MdwStb810PlayerPrivate* priv = MDW_STB810_PLAYER_GET_PRIVATE(iface);
    *width = priv->max_w;
    *height = priv->max_h;
    return TRUE;
}

void
_iface_mdw_player_set_audio_pid (MdwPlayer* iface, guint16 pid)
{
    MdwStb810PlayerPrivate* priv = MDW_STB810_PLAYER_GET_PRIVATE(iface);

    priv->audio_pid = pid;
}

void
_iface_mdw_player_set_video_pid (MdwPlayer* iface, guint16 pid)
{
    MdwStb810PlayerPrivate* priv = MDW_STB810_PLAYER_GET_PRIVATE(iface);
    priv->video_pid = pid;
}

guint16
_iface_mdw_player_get_audio_pid (MdwPlayer* iface)
{
    MdwStb810PlayerPrivate* priv = MDW_STB810_PLAYER_GET_PRIVATE(iface);
    return priv->audio_pid;
}

guint16
_iface_mdw_player_get_video_pid (MdwPlayer* iface)
{
    MdwStb810PlayerPrivate* priv = MDW_STB810_PLAYER_GET_PRIVATE(iface);
    return priv->video_pid;
}

gboolean
_iface_mdw_player_play (MdwPlayer* iface, GError** error)
{
    /* TODO: use error */
    MdwStb810PlayerPrivate* priv = MDW_STB810_PLAYER_GET_PRIVATE(iface);
    MdwStb810Player* self = MDW_STB810_PLAYER(iface);
    DFBResult err;
    IDirectFBVideoProvider* vp = priv->video_provider;
    IDirectFBSurface* surface = priv->video_surface;

    MdwDemux* demux = priv->demux;

    DEBUG_INFO("Request to play video");
    if (surface == NULL) {
        DEBUG_ERROR("Cannot play video because surface is NULL");
        return FALSE;
    }
    if (demux == NULL) {
            DEBUG_ERROR("Cannot play because Demux is NULL");
            return FALSE;
    }
    if (vp == NULL) {
        DEBUG_INFO("Video Provider is NULL, creating a new one");
        IDirectFB* dfb = priv->dfb;
        char* filename;
        g_object_get(G_OBJECT(demux), "demux-device", &filename, NULL);
        DEBUG_INFO("Demux filename: %s", filename);
        if ((err = dfb->CreateVideoProvider(dfb, filename, &vp)) != DFB_OK){
            DEBUG_ERROR("CreateVideoProvider failed:%s", DirectFBErrorString(err));
            return FALSE;
        }
        mdw_stb810_player_directfb_set_vp(self, vp);
    }
    /* stop play */
    if (priv->is_playing) {
        _iface_mdw_player_stop(iface, NULL);
    }

    if (priv->audio_pid == 0) {
        DEBUG_ERROR("Invalid audio pid");
        return FALSE;
    }
    if (priv->video_pid == 0) {
        DEBUG_ERROR("Invalid video pid");
        return FALSE;
    }   
    
    /* I can setup pes handlers 'cause almost things are ok*/
    mdw_stb810_player_setup_pes_handlers(self);

    /* start demuxing process */
    if (mdw_demux_start(MDW_DEMUX(demux)) == FALSE) {
        DEBUG_WARN("Cannot start the demuxing process");
    }
    /* TODO implement a thread to run demux step by step (clock) */
    //return FALSE;

    DEBUG_INFO("Calling VideoProvider::PlayTo");

    if ((err = vp->PlayTo(vp, surface, NULL, NULL, ":I2S1:IPStreaming"))!=DFB_OK) {
        DEBUG_ERROR("PlayTo failed: %s", DirectFBErrorString(err));
        return FALSE;
    }

    priv->is_playing = TRUE;
    
    return TRUE;
}

gboolean
_iface_mdw_player_stop (MdwPlayer* iface, GError** error)
{
    DFBResult err;
    MdwStb810PlayerPrivate* priv = MDW_STB810_PLAYER_GET_PRIVATE(iface);
    MdwStb810Player* self = MDW_STB810_PLAYER(iface);

    IDirectFBVideoProvider* vp = priv->video_provider;

    DEBUG_INFO("Stopping the video");

    if (!priv->is_playing) {
        return FALSE;
    }

    mdw_stb810_player_teardown_pes_handlers(self);

    if (mdw_demux_stop(MDW_DEMUX(priv->demux)) == FALSE) {
        DEBUG_WARN("Cannot stop the demuxing process");
    }
    /* TODO implement a thread to run demux step by step (clock) */
    //return FALSE;

    if ((err = vp->Stop(vp)) != DFB_OK) {
        DEBUG_WARN("IDirectfbVideoProvider::Stop failed:%s", DirectFBErrorString(err));
    }
    priv->is_playing = FALSE;

    return TRUE;
}


