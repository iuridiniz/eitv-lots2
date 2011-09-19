/***************************************************************************
 *  MdwDvbDemux
 *  Mon Apr 23 14:45:24 2007 
 *
 *  Copyright (c) 2007 Iuri Diniz,,,
 *  Authors: Iuri Diniz,,, <iuri@email.com.br> 2007
 * 
 *  VERSION CONTROL INFORMATION: $Id$
 *
****************************************************************************/

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sched.h>

#include <glib/gprintf.h>

#include <linux/dvb/dmx.h>

#include "mdw_dvb_demux.h"
#include "mdw_dvb_audio_dummy.h"
#include "mdw_dvb_video_dummy.h"
#include "mdw_ts_source.h"
#include "mdw_ts_file_source.h"
#include "mdw_dvb_tuner.h"
#include "config.h"
#include "mdw_util.h"


/********************/
/* local prototypes */
/********************/
/* interface overrided methods */
static void _iface_mdw_demux_init (gpointer g_iface, gpointer data);

static gboolean _iface_mdw_demux_add_section_handler (MdwDemux *iface, MdwDemuxDataHandler *handler, guint16 pid); 
static gboolean _iface_mdw_demux_add_pes_handler (MdwDemux *iface, MdwDemuxDataHandler *handler, guint16 pid); 
static gboolean _iface_mdw_demux_set_ts_source (MdwDemux *iface, MdwTsSource* source); 
static gboolean _iface_mdw_demux_start (MdwDemux *iface); 
static gboolean _iface_mdw_demux_stop (MdwDemux *iface); 
//static void     _iface_mdw_demux_step (MdwDemux *iface); 
static gboolean _iface_mdw_demux_del_section_handler (MdwDemux *iface, MdwDemuxDataHandler *handler, guint16 pid);
static gboolean _iface_mdw_demux_del_pes_handler (MdwDemux *iface, MdwDemuxDataHandler *handler, guint16 pid); 


/* destructors */
static void mdw_dvb_demux_dispose (GObject *object);
static void mdw_dvb_demux_finalize (GObject *object);

/* properties get/set functions*/
static void mdw_dvb_demux_set_property(GObject *object,
    guint property_id,
    const GValue *value,
    GParamSpec *pspec);
static void mdw_dvb_demux_get_property(GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec);
/* enum for enumerate properties */
enum {
    MDW_DVB_DEMUX_DEMUX_DEVICE = 1,
    MDW_DVB_DEMUX_DVR_DEVICE
};

/* ERROR */

GQuark 
mdw_dvb_demux_error_quark() 
{
    static GQuark quark = 0;
    if (G_UNLIKELY(quark == 0)) {
        quark = g_quark_from_static_string("MdwDvbDemuxError");
    }
    return quark;
}
#define MDW_DVB_DEMUX_RAISE_ERROR(error, type, ...) \
{ \
    g_set_error((error), MDW_DVB_DEMUX_ERROR, (type), __VA_ARGS__);\
    return FALSE; \
} while (0)

#define MDW_DVB_DEMUX_RAISE_ERROR_IF_FAIL(error, cond, type, ...) \
{ \
    if (G_UNLIKELY(!(cond))) { \
         MDW_DVB_DEMUX_RAISE_ERROR((error), (type), ...); \
    } \
} while (0)

// MDW_DVB_DEMUX_ERROR_TYPE_1 
/*#define MDW_DVB_DEMUX_RAISE_ERROR_TYPE_1_IF_FAIL(error, cond, ...) \
    MDW_DVB_DEMUX_RAISE_ERROR_IF_FAILED((error), (cond), MDW_DVB_DEMUX_ERROR_TYPE_1, __VA_ARGS__)
#define MDW_DVB_DEMUX_RAISE_ERROR_TYPE_1(error, cond, ...) \
    MDW_DVB_DEMUX_RAISE_ERROR((error), MDW_DVB_DEMUX_ERROR_TYPE_1, __VA_ARGS__)
*/
// MDW_DVB_DEMUX_ERROR_TYPE_2
/*#define MDW_DVB_DEMUX_RAISE_ERROR_TYPE_2_IF_FAIL(error, cond, ...) \
    MDW_DVB_DEMUX_RAISE_ERROR_IF_FAIL((error), (cond), MDW_DVB_DEMUX_ERROR_TYPE_2, __VA_ARGS__)
#define MDW_DVB_DEMUX_RAISE_ERROR_TYPE_2(error, ...) \
    MDW_DVB_DEMUX_RAISE_ERROR((error), MDW_DVB_DEMUX_ERROR_TYPE_2, __VA_ARGS__)
*/
// MDW_DVB_DEMUX_ERROR_FAILED (General Failure)
#define MDW_DVB_DEMUX_RAISE_ERROR_FAILED_IF_FAIL(error, cond, ...) \
    MDW_DVB_DEMUX_RAISE_ERROR_IF_FAIL((error), (cond), MDW_DVB_DEMUX_ERROR_FAILED, __VA_ARGS__)
#define MDW_DVB_DEMUX_RAISE_ERROR_FAILED(error, cond, ...) \
    MDW_DVB_DEMUX_RAISE_ERROR((error), MDW_DVB_DEMUX_ERROR_FAILED, __VA_ARGS__)

// MDW_DVB_DEMUX_ERROR_NOT_IMPLEMENTED (Not Implemented)
/*#define MDW_DVB_DEMUX_RAISE_ERROR_NOT_IMPLEMENTED(error) \
    MDW_DVB_DEMUX_RAISE_ERROR((error), MDW_DVB_DEMUX_ERROR_NOT_IMPLEMENTED, "Not implemented: %s", __FUNC__)*/


/*********** PRIVATE *************/
/* private field */

#define MDW_DVB_DEMUX_GET_PRIVATE(o)  \
   (G_TYPE_INSTANCE_GET_PRIVATE ((o), MDW_TYPE_DVB_DEMUX, MdwDvbDemuxPrivate))
enum MdwDvbDemuxState {
    MDW_DVB_DEMUX_STATE_STOPPED = 0,
    MDW_DVB_DEMUX_STATE_PLAYING
};
typedef struct _MdwDvbDemuxPrivate MdwDvbDemuxPrivate;
struct _MdwDvbDemuxPrivate {
    GHashTable *filters;
    MdwTsSource *source;
    gchar* demux_device;
    gchar* dvr_device;
    enum MdwDvbDemuxState state;
    gint dvr_fd;
    guint read_write_source_id;
    gboolean dispose_has_run;
    //guint8* read_write_buffer;
    //guint8* read_write_buffer_size;
    //guint8* read_write_buffer_size;
    GThread* thread_read_write;
    GAsyncQueue* main_queue;
    GAsyncQueue* thread_read_write_queue;

};

struct filter_params {
    gint fd;
    guint16 pid;
    gboolean is_pes;
    guint8* buffer;
    guint8* p_buffer;
    gsize buffer_size;
    gsize buffer_max_size;
    GSList *handlers;
    GIOChannel* io_channel;
    guint source_id;
};
/* order: priority, lower first */
enum _MdwMessageType {
    MDW_MESSAGE_TYPE_NULL = 0, 
    MDW_MESSAGE_TYPE_READY,
    MDW_MESSAGE_TYPE_PAUSE,
    MDW_MESSAGE_TYPE_PAUSED,
    MDW_MESSAGE_TYPE_GO, 
    MDW_MESSAGE_TYPE_GOING, 
    MDW_MESSAGE_TYPE_FINALIZE,
    MDW_MESSAGE_TYPE_FINALIZED
};
#include "mdw_message.h"


/* private methods */
static struct filter_params* mdw_dvb_demux_get_filter (MdwDvbDemux *self, 
    guint16 pid, gboolean is_pes, 
    dmx_input_t input, dmx_output_t output, dmx_pes_type_t pes_type, 
    gboolean to_create, 
    GError** error);

static gboolean 
mdw_dvb_demux_remove_handler (
    MdwDvbDemux *self, 
    MdwDemuxDataHandler *handler, 
    guint16 pid, 
    gboolean is_pes);

static void mdw_dvb_demux_thread_read_write_init(MdwDvbDemux *self);
static void mdw_dvb_demux_thread_read_write_finalize(MdwDvbDemux *self);
static void mdw_dvb_demux_thread_read_write_stop(MdwDvbDemux *self);
static void mdw_dvb_demux_thread_read_write_go(MdwDvbDemux *self);

/* auxiliary functions */
static gpointer _thread_read_write_func(gpointer data);
static gboolean _read_write_func (gpointer data);
static gboolean _filter_func (GIOChannel *source, GIOCondition condition, gpointer data);
static void _filter_start(gpointer key, gpointer value, gpointer user_data);
static void _filter_free(gpointer key, gpointer value, gpointer user_data);
static void _filter_stop(gpointer key, gpointer value, gpointer user_data);
static void _try_flush_pes(struct filter_params *filter);
static void _try_flush_section(struct filter_params *filter);


/***********************************************/
/* GObject obligatory definitions/declarations */
/***********************************************/

/* define my Object */
/*
G_DEFINE_TYPE(
    MdwDvbDemux, //my name in Camel case.
    mdw_dvb_demux, // my name in lowercase, with words separated by '_'.
    G_TYPE_OBJECT //GType of my parent 
)
*/

/* define my Object (with interface)*/
/*
G_DEFINE_TYPE_WITH_CODE(
    MdwDvbDemux, //my name in Camel case.
    mdw_dvb_demux, // my name in lowercase, with words separated by '_'.
    G_TYPE_OBJECT, //GType of my parent 
    G_IMPLEMENT_INTERFACE(MDW_TYPE_DEMUX, //interfaces that I implement
        _iface_mdw_demux_init)
)
*/

/* Init: define my Object (explict) */
static void     mdw_dvb_demux_init       (MdwDvbDemux      *self);
static void     mdw_dvb_demux_class_init (MdwDvbDemuxClass *klass);
static gpointer mdw_dvb_demux_parent_class = NULL;
static void     mdw_dvb_demux_class_intern_init (gpointer klass)
{
  mdw_dvb_demux_parent_class = g_type_class_peek_parent (klass);
  mdw_dvb_demux_class_init ((MdwDvbDemuxClass*) klass);
}

GType
mdw_dvb_demux_get_type (void)
{
    static GType g_define_type_id = 0; 
    if (G_UNLIKELY (g_define_type_id == 0)) 
    { 
        static const GTypeInfo g_define_type_info = { 
            sizeof (MdwDvbDemuxClass), 
            (GBaseInitFunc) NULL, 
            (GBaseFinalizeFunc) NULL, 
            (GClassInitFunc) mdw_dvb_demux_class_intern_init, 
            (GClassFinalizeFunc) NULL, 
            NULL,   /* class_data */ 
            sizeof (MdwDvbDemux), 
            0,      /* n_preallocs */ 
            (GInstanceInitFunc) mdw_dvb_demux_init, 
        }; 
        g_define_type_id = g_type_register_static (
            G_TYPE_OBJECT, //parent GType 
            "MdwDvbDemux", //my name in Camel case
            &g_define_type_info, 
            0
        ); 
        /* interfaces that I implement */
        {
            static const GInterfaceInfo g_implement_interface_info = {
                (GInterfaceInitFunc) _iface_mdw_demux_init 
            };
            g_type_add_interface_static (
                g_define_type_id, 
                MDW_TYPE_DEMUX, 
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
mdw_dvb_demux_class_init (MdwDvbDemuxClass *klass) 
{
    /* top ancestral */
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    /* private alocation */
    g_type_class_add_private (klass, sizeof (MdwDvbDemuxPrivate));

    /* methods */
    //klass->method1 = mdw_dvb_demux_method1;
    //klass->method2 = mdw_dvb_demux_method2;

    /* destructors */
    gobject_class->dispose = mdw_dvb_demux_dispose;
    gobject_class->finalize = mdw_dvb_demux_finalize;

    /* properties */
    GParamSpec *pspec;
    gobject_class->set_property = mdw_dvb_demux_set_property;
    gobject_class->get_property = mdw_dvb_demux_get_property;
    
    /* Property demux-device (type: string) (contruct_only) (read/write) */
    pspec = g_param_spec_string(
        "demux-device", //name
        "Demux device", //nick
        "Set/Get the demux device", //description
        DVB_BASE"/demux0", //default value
        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY //flags
    );
    g_object_class_install_property(gobject_class, 
        MDW_DVB_DEMUX_DEMUX_DEVICE, pspec);
    g_param_spec_unref(pspec);


    /* Property dvr-device (type: string) (read/write) (contruct_only) */ 
    pspec = g_param_spec_string(
        "dvr-device", //name
        "DVR device", //nick
        "Set/Get the DVR device", //description
        DVB_BASE"/dvr0", //default value
        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY //flags
    );
    
    g_object_class_install_property(gobject_class, 
        MDW_DVB_DEMUX_DVR_DEVICE, pspec);
    g_param_spec_unref(pspec);

    /* signals */

    /* chain-up */
}
/** 
 * Interface init: So let's override methods of MdwDemux 
 */
static void 
_iface_mdw_demux_init (gpointer g_iface, gpointer data) 
{
    MdwDemuxInterface* iface = (MdwDemuxInterface*)g_iface; 

    iface->add_section_handler = _iface_mdw_demux_add_section_handler;
    iface->add_pes_handler = _iface_mdw_demux_add_pes_handler;
    iface->set_ts_source = _iface_mdw_demux_set_ts_source;
    iface->start = _iface_mdw_demux_start;
    iface->stop = _iface_mdw_demux_stop;
    //iface->step = _iface_mdw_demux_step;
    iface->del_section_handler = _iface_mdw_demux_del_section_handler;
    iface->del_pes_handler = _iface_mdw_demux_del_pes_handler;

}

/**
 * GObject Internal Constructor 
 */
static void
mdw_dvb_demux_init(MdwDvbDemux *self) 
{
    /* set the private members */
    //self->boolean_member = TRUE;
    //self->uint8_member = 42;

    MdwDvbDemuxPrivate *priv = MDW_DVB_DEMUX_GET_PRIVATE(self);
    priv->filters = g_hash_table_new(mdw_guint16_hash, mdw_guint16_equal);
    priv->dvr_fd = -1;

}

/***************/
/* Destructors */
/***************/
static void    
mdw_dvb_demux_dispose (GObject *object) 
{   
    MdwDvbDemux *self =  MDW_DVB_DEMUX(object);
    MdwDvbDemuxPrivate *priv = MDW_DVB_DEMUX_GET_PRIVATE(self);
     
    if (priv->dispose_has_run) {
        return;
    }
    priv->dispose_has_run = TRUE;
    if (priv->state == MDW_DVB_DEMUX_STATE_PLAYING) {
        mdw_demux_stop(MDW_DEMUX(self));
    }
    mdw_dvb_demux_thread_read_write_finalize(self);

    g_assert(priv->state == MDW_DVB_DEMUX_STATE_STOPPED);
    g_assert(priv->filters != NULL);

    g_hash_table_foreach(priv->filters, _filter_free, NULL);
    
    if (priv->source) {
        g_object_unref(priv->source);
        priv->source = NULL;
    }

    /* chaining up */
    G_OBJECT_CLASS(mdw_dvb_demux_parent_class)->dispose(object);
}
static void    
mdw_dvb_demux_finalize (GObject *object) 
{
    MdwDvbDemux *self =  MDW_DVB_DEMUX(object);
    MdwDvbDemuxPrivate *priv = MDW_DVB_DEMUX_GET_PRIVATE(self);

    g_assert(priv->state == MDW_DVB_DEMUX_STATE_STOPPED);
    g_assert(priv->filters != NULL);

    g_hash_table_unref(priv->filters);

    /* chaining up */
    G_OBJECT_CLASS(mdw_dvb_demux_parent_class)->finalize(object);
}

/**********************/
/* PROPERTIES GET/SET */
/**********************/
/**
 * Properties set function
 */

static void 
mdw_dvb_demux_set_property(GObject *object,
    guint property_id,
    const GValue *value,
    GParamSpec *pspec) 
{
    //MdwDvbDemux *self = MDW_DVB_DEMUX(object);
    MdwDvbDemuxPrivate *priv = MDW_DVB_DEMUX_GET_PRIVATE(object);
    switch (property_id) {
        case MDW_DVB_DEMUX_DVR_DEVICE:
            g_free(priv->dvr_device);
            priv->dvr_device = g_value_dup_string(value);
            break;
        case MDW_DVB_DEMUX_DEMUX_DEVICE:
            g_free(priv->dvr_device);
            priv->demux_device = g_value_dup_string(value);
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
mdw_dvb_demux_get_property(GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec) 
{
    //MdwDvbDemux *self = MDW_DVB_DEMUX(object);
    MdwDvbDemuxPrivate *priv = MDW_DVB_DEMUX_GET_PRIVATE(object);
    switch (property_id) {
        case MDW_DVB_DEMUX_DEMUX_DEVICE:
            g_value_set_string(value, priv->demux_device);
            break;
        case MDW_DVB_DEMUX_DVR_DEVICE:
            g_value_set_string(value, priv->dvr_device);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }

}




/************************/
/* Public Constructors */
/***********************/

MdwDvbDemux* 
mdw_dvb_demux_new() {
    return (MdwDvbDemux*) g_object_new(MDW_TYPE_DVB_DEMUX, NULL);
}

/*
MdwDvbDemux* 
mdw_dvb_demux_new_with_arg1(guint8 param1) {
    return (MdwDvbDemux*) g_object_new(MDW_TYPE_DVB_DEMUX, "property 1", param1, NULL);
}
*/


/**********************/
/* Overrided methods  */
/**********************/
static gboolean 
_iface_mdw_demux_add_pes_handler (MdwDemux *iface, MdwDemuxDataHandler *handler, guint16 pid) 
{
    g_return_val_if_fail(iface != NULL, FALSE); 
    g_return_val_if_fail(handler != NULL, FALSE); 
    g_return_val_if_fail(pid > 0 && pid < 8192, FALSE);

    MdwDvbDemux* self = MDW_DVB_DEMUX(iface);
    MdwDvbDemuxPrivate* priv = MDW_DVB_DEMUX_GET_PRIVATE(iface);

    DEBUG_INFO("Adding a PES handler %p for pid %u", (void *) handler, pid);

    dmx_input_t input;
    dmx_output_t output;
    dmx_pes_type_t type;

    if (G_UNLIKELY(priv->source == NULL)) {
        DEBUG_ERROR("self->source == NULL");
        return FALSE;
    }

    if (MDW_IS_TS_FILE_SOURCE(priv->source)) {
        input = DMX_IN_DVR;
    } else if (MDW_IS_DVB_TUNER(priv->source)) {
        input = DMX_IN_FRONTEND;
    } else {
        DEBUG_ERROR("Other type than not MdwTsFileSource or MdwDvbTuner is not supported yet");
        return FALSE;
    }

    if (MDW_IS_DVB_AUDIO_DUMMY(handler)) {
        type = DMX_PES_AUDIO;
        output = DMX_OUT_DECODER;
    } else if (MDW_IS_DVB_VIDEO_DUMMY(handler)) {
        type = DMX_PES_VIDEO;
        output = DMX_OUT_DECODER;
    } else {
        type = DMX_PES_OTHER;
        output = DMX_OUT_TAP;
    }
    /* fixme: use GError (the last argument) */
    struct filter_params *filter = mdw_dvb_demux_get_filter(self, pid, TRUE, input, output, type, TRUE, NULL);

    if (G_UNLIKELY(filter == NULL)) {
        return FALSE;
    }
    if (filter->handlers) {
        /* already have handlers */
        if (type == DMX_PES_AUDIO) {
            if (! MDW_IS_DVB_AUDIO_DUMMY(filter->handlers->data)) {
                DEBUG_ERROR("Cannot add this handler because there's another handler with a incompatible type");

                return FALSE;
            }
        } else if (type == DMX_PES_VIDEO) {
            if (! MDW_IS_DVB_VIDEO_DUMMY(filter->handlers->data)) {
                DEBUG_ERROR("Cannot add this handler because there's another handler with a incompatible type");

                return FALSE;
            }
        } else if (type == DMX_PES_OTHER) {
            if (MDW_IS_DVB_VIDEO_DUMMY(filter->handlers->data) || MDW_IS_DVB_AUDIO_DUMMY(filter->handlers->data) ) {
                DEBUG_ERROR("Cannot add this filter because there's another filter with a incompatible type");

                return FALSE;
            }
        }
    }

    gboolean to_start = FALSE;
    if (filter->handlers == NULL && priv->state == MDW_DVB_DEMUX_STATE_PLAYING) {
        to_start = TRUE;
    }


    g_object_ref(handler);
    filter->handlers = g_slist_append(filter->handlers, G_OBJECT(handler));
    if (to_start) {
        GError* error = NULL;
        _filter_start(&pid, filter, &error);
        if (error != NULL) {
            DEBUG_ERROR("Error while starting filters:%s", error->message);
            g_error_free(error);
        }
    }
    return TRUE;
}
static gboolean 
_iface_mdw_demux_del_pes_handler (MdwDemux *iface, MdwDemuxDataHandler *handler, guint16 pid)
{
    g_return_val_if_fail(iface != NULL, FALSE);
    g_return_val_if_fail(handler != NULL, FALSE);
    g_return_val_if_fail(pid > 0 && pid < 8192, FALSE);

    MdwDvbDemux *self = MDW_DVB_DEMUX(iface);
    return mdw_dvb_demux_remove_handler(self, handler, pid, TRUE);
}

static gboolean 
_iface_mdw_demux_set_ts_source (MdwDemux *iface, MdwTsSource* source) 
{
    g_return_val_if_fail(iface != NULL, FALSE);
    //g_return_val_if_fail(source != NULL, FALSE);

    MdwDvbDemuxPrivate *priv = MDW_DVB_DEMUX_GET_PRIVATE(iface);

    /* there's a dependency between source and handlers and the source cannot be changed to a incompatible type*/
    if (source != NULL && priv->source != NULL ) {
        //g_return_val_if_fail(priv->source == NULL, FALSE);
        //return FALSE;
        if (!(MDW_IS_TS_SOURCE(priv->source) && MDW_IS_TS_SOURCE(source))) {
            return FALSE;
        }

    }
    if (source && MDW_IS_TS_FILE_SOURCE(source)) {
        if (priv->dvr_fd == -1) {
            gint fd;
            if (G_UNLIKELY((fd = open(priv->dvr_device, O_WRONLY | O_NONBLOCK))== -1)) {
                gchar err_msg[1024];
                g_snprintf(err_msg, 1024, 
                    "Cannot open demux dvr_device '%s':%s", 
                    priv->dvr_device, g_strerror(errno));
                //g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno), err_msg);
                DEBUG_ERROR(err_msg);
                return FALSE;
            }
            priv->dvr_fd = fd;
        }
    }

    if (priv->source) {
        g_object_unref(priv->source);
    }

    if (source) {
        g_object_ref(source);
    }

    priv->source = source;

    return TRUE;


}
static gboolean 
_iface_mdw_demux_start (MdwDemux *iface)
{
    g_return_val_if_fail(iface != NULL, FALSE);
    MdwDvbDemux *self = MDW_DVB_DEMUX(iface);
    MdwDvbDemuxPrivate *priv = MDW_DVB_DEMUX_GET_PRIVATE(self);

    g_return_val_if_fail(priv->state == MDW_DVB_DEMUX_STATE_STOPPED, FALSE);

    GError *error = NULL;
    g_hash_table_foreach(priv->filters, _filter_start, &error);

    if (error != NULL)  {
        /* error while initing */
        DEBUG_ERROR("Error while starting filters:%s", error->message);
        g_error_free(error);
        return FALSE;
    }

    /* add the read/write call */
    //priv->read_write_source_id = g_idle_add(_read_write_func, iface);

    priv->state = MDW_DVB_DEMUX_STATE_PLAYING;

    mdw_dvb_demux_thread_read_write_go(self);

    return TRUE;
}

static gboolean 
_iface_mdw_demux_stop (MdwDemux *iface)
{
    g_return_val_if_fail(iface != NULL, FALSE);
    MdwDvbDemux *self = MDW_DVB_DEMUX(iface);
    MdwDvbDemuxPrivate *priv = MDW_DVB_DEMUX_GET_PRIVATE(self);
    g_return_val_if_fail(priv->state == MDW_DVB_DEMUX_STATE_PLAYING, FALSE);
    
    GError *error = NULL;
    mdw_dvb_demux_thread_read_write_stop(self);
    g_hash_table_foreach(priv->filters, _filter_stop, &error);

    if (error != NULL) {
        DEBUG_ERROR("Error while stopping filters: %s", error->message);
    }

    //gboolean ret = g_source_remove(priv->read_write_source_id);
    //g_assert(ret == TRUE);

    priv->state = MDW_DVB_DEMUX_STATE_STOPPED;

    return TRUE;
}


static gboolean 
_iface_mdw_demux_add_section_handler (MdwDemux *iface, MdwDemuxDataHandler *handler, guint16 pid) 
{
    g_return_val_if_fail(iface != NULL, FALSE);
    g_return_val_if_fail(handler != NULL, FALSE);
    g_return_val_if_fail(pid > 0 && pid < 8192, FALSE);
    MdwDvbDemux *self = MDW_DVB_DEMUX(iface);
    MdwDvbDemuxPrivate *priv = MDW_DVB_DEMUX_GET_PRIVATE(iface);

    struct filter_params *filter = mdw_dvb_demux_get_filter(self, pid, FALSE, 0, 0, 0, TRUE, NULL);

    if (filter == NULL) { 
        return FALSE;
    }
    gboolean to_start = FALSE;
    if (filter->handlers == NULL && priv->state == MDW_DVB_DEMUX_STATE_PLAYING) {
        to_start = TRUE;
    }

    g_object_ref(handler);
    filter->handlers = g_slist_append(filter->handlers, G_OBJECT(handler));
    if (to_start) {
        GError* error = NULL;
        _filter_start(&pid, filter, &error);
        if (error != NULL) {
            DEBUG_ERROR("Error while starting filters:%s", error->message);
            g_error_free(error);
        }
    }
    return TRUE;

}

static gboolean 
_iface_mdw_demux_del_section_handler (MdwDemux *iface, MdwDemuxDataHandler *handler, guint16 pid)
{
    g_return_val_if_fail(iface != NULL, FALSE);
    g_return_val_if_fail(handler != NULL, FALSE);
    g_return_val_if_fail(pid > 0 && pid < 8192, FALSE);
    MdwDvbDemux *self = MDW_DVB_DEMUX(iface);

    return mdw_dvb_demux_remove_handler(self, handler, pid, FALSE);
}


/******************/
/* PUBLIC METHODS */
/******************/

/*******************/
/* PRIVATE METHODS */
/*******************/
static void 
mdw_dvb_demux_thread_read_write_init(MdwDvbDemux *self) 
{
    g_assert(self != NULL);
    MdwDvbDemuxPrivate *priv = MDW_DVB_DEMUX_GET_PRIVATE(self);
    g_assert(priv->thread_read_write == NULL);
    g_assert(priv->main_queue == NULL);
    g_assert(priv->thread_read_write_queue == NULL);

    if(!g_thread_supported()) g_thread_init (NULL);
   
    DEBUG_INFO("Initing thread to read/write");
    priv->thread_read_write = g_thread_create(_thread_read_write_func, self, FALSE, NULL);
    priv->main_queue = g_async_queue_new();
    priv->thread_read_write_queue = g_async_queue_new();
    
    //MdwMessage *m = mdw_message_new();
    //mdw_message_put(m, priv->main_queue);
    //
    /* wait for init */
    MdwMessage *m = mdw_message_get(priv->main_queue, TRUE);
    
    g_assert(m->type == MDW_MESSAGE_TYPE_READY);
    g_assert(m->buffer == NULL);
    g_assert(m->size == 0);
    mdw_message_free(m, TRUE);
    
    g_assert(g_async_queue_length(priv->main_queue) == 0);
    
}

static void 
mdw_dvb_demux_thread_read_write_finalize(MdwDvbDemux *self) 
{
    g_assert(self != NULL);
    MdwDvbDemuxPrivate *priv = MDW_DVB_DEMUX_GET_PRIVATE(self);
    if (priv->thread_read_write == NULL) 
    {
        return;
    } 
    DEBUG_INFO("Finalizing thread to read/write");
    g_assert(priv->main_queue != NULL);
    g_assert(priv->thread_read_write_queue != NULL);

    MdwMessage *m = mdw_message_new(MDW_MESSAGE_TYPE_FINALIZE, 0);
    mdw_message_put(m, priv->thread_read_write_queue);
    
    m = mdw_message_get(priv->main_queue, TRUE);
    g_assert(m->type == MDW_MESSAGE_TYPE_FINALIZED);
    g_assert(m->buffer == NULL);
    g_assert(m->size == 0);
    mdw_message_free(m, TRUE);

    g_assert(g_async_queue_length(priv->main_queue) == 0);
    g_assert(g_async_queue_length(priv->thread_read_write_queue) == 0);

    g_async_queue_unref(priv->main_queue);
    g_async_queue_unref(priv->thread_read_write_queue);

    priv->main_queue = NULL;
    priv->thread_read_write_queue = NULL;
    priv->thread_read_write = NULL;

}
static void 
mdw_dvb_demux_thread_read_write_go(MdwDvbDemux *self) 
{
    g_assert(self != NULL);
    MdwDvbDemuxPrivate *priv = MDW_DVB_DEMUX_GET_PRIVATE(self);
    if (priv->thread_read_write == NULL) {
        mdw_dvb_demux_thread_read_write_init(self);
        g_assert(priv->thread_read_write != NULL);
        g_assert(priv->main_queue != NULL);
        g_assert(priv->thread_read_write_queue != NULL);
    }

    DEBUG_INFO("Asking to activate thread to read/write ");
    MdwMessage *m = mdw_message_new(MDW_MESSAGE_TYPE_GO, 0);
    mdw_message_put(m, priv->thread_read_write_queue);
    
    m = mdw_message_get(priv->main_queue, TRUE);
    g_assert(m->type == MDW_MESSAGE_TYPE_GOING);
    g_assert(m->buffer == NULL);
    g_assert(m->size == 0);
    mdw_message_free(m, TRUE);

    g_assert(g_async_queue_length(priv->main_queue) == 0); 

}

static void 
mdw_dvb_demux_thread_read_write_stop(MdwDvbDemux *self)
{
    DEBUG_INFO("Asking to deactivate thread to read/write ");
    g_assert(self != NULL);
    MdwDvbDemuxPrivate *priv = MDW_DVB_DEMUX_GET_PRIVATE(self);
    g_assert(priv->thread_read_write != NULL);
    g_assert(priv->main_queue != NULL);
    g_assert(priv->thread_read_write_queue != NULL);

    MdwMessage *m = mdw_message_new(MDW_MESSAGE_TYPE_PAUSE, 0);
    mdw_message_put(m, priv->thread_read_write_queue);
    
    m = mdw_message_get(priv->main_queue, TRUE);
    g_assert(m->type == MDW_MESSAGE_TYPE_PAUSED);
    g_assert(m->buffer == NULL);
    g_assert(m->size == 0);
    mdw_message_free(m, TRUE);

    g_assert(g_async_queue_length(priv->main_queue) == 0); 
    
}

static gboolean 
mdw_dvb_demux_remove_handler (
    MdwDvbDemux *self, 
    MdwDemuxDataHandler *handler, 
    guint16 pid, 
    gboolean is_pes) 
{
    g_assert(self != NULL);
    g_assert(handler != NULL);
    g_assert(pid > 0 && pid < 8192);

    DEBUG_INFO("Trying to remove handler: %p (for pid %u)", (void *)handler, pid );

    struct filter_params *filter = mdw_dvb_demux_get_filter(self, pid, is_pes, 0, 0, 0, FALSE, NULL);
    if (filter == NULL) {
        DEBUG_INFO("There's no filter allocated for pid %u", pid);
        return FALSE;
    }

    DEBUG_INFO("I have found a filter for pid %u, so let's search the handler %p", pid, (void *)handler);
    if (g_slist_find(filter->handlers, handler) == NULL) {
        DEBUG_INFO("Handler %p not found (for pid %u)", (void *)handler, pid);
        return FALSE;
    }
    filter->handlers = g_slist_remove(filter->handlers, handler); 

    if (filter->handlers == NULL) {
        MdwDvbDemuxPrivate *priv = MDW_DVB_DEMUX_GET_PRIVATE(self);
        if (priv->state == MDW_DVB_DEMUX_STATE_PLAYING) {
            gpointer orig_pid = NULL;
            gpointer p_filter = NULL;
            gboolean has_key = g_hash_table_lookup_extended(priv->filters, &pid, &orig_pid, &p_filter);
            g_assert(has_key == TRUE);
            has_key = g_hash_table_remove(priv->filters, orig_pid);
            g_assert(has_key == TRUE);
            _filter_stop(orig_pid, p_filter, NULL);
            _filter_free(orig_pid, p_filter, NULL);
        }
    }
    
    return TRUE;
}
static struct filter_params* 
mdw_dvb_demux_get_filter (MdwDvbDemux *self, 
    guint16 pid, gboolean is_pes, 
    dmx_input_t input, dmx_output_t output, dmx_pes_type_t pes_type, 
    gboolean to_create, 
    GError** error)
{
    g_assert(self != NULL);
    g_assert(pid > 0 && pid < 8192);
    g_assert(error == NULL || *error == NULL);
    
    MdwDvbDemuxPrivate *priv = MDW_DVB_DEMUX_GET_PRIVATE(self);

    gpointer orig_pid = NULL;
    gboolean has_key = FALSE;
    gpointer p_filter = NULL;

    DEBUG_INFO("Looking for pid %u", pid);

    g_assert(priv->filters != NULL);

    has_key = g_hash_table_lookup_extended(priv->filters, &pid, &orig_pid, &p_filter);
    struct filter_params *filter = (struct filter_params*) p_filter;
    if (has_key) {
        DEBUG_INFO(
            "Found a already allocated filter structure for pid %u (pid addr: %p filter addr: %p)", 
            *(guint16*)orig_pid, (void *)orig_pid, (void*) filter);
    } else {
        DEBUG_INFO("There isn't a filter for pid %u", pid);
        if (to_create == TRUE) {
            DEBUG_INFO("I must allocate a new one filter for pid: %u (PES=%s)", pid, is_pes?"TRUE":"FALSE");
        }
    }
    if (! has_key && is_pes == FALSE && to_create == TRUE) {

        /* open hardware demux */
        gint fd;
        if (G_UNLIKELY((fd = open(priv->demux_device, O_RDONLY | O_NONBLOCK)) == -1)) {
            gchar err_msg[1024];
            g_snprintf(err_msg, 1024, 
                "Cannot open demux device: %s for pid %u:%s", 
                priv->demux_device, pid, g_strerror(errno));
            g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno), err_msg);
            DEBUG_ERROR(err_msg);

            return NULL;
        }
        /* set filter */
        struct dmx_sct_filter_params dvb_params;
        memset(&dvb_params, 0, sizeof(struct dmx_sct_filter_params));
        dvb_params.pid = pid;
    
        dvb_params.flags |= DMX_CHECK_CRC;

        if (G_UNLIKELY(ioctl(fd, DMX_SET_FILTER, &dvb_params) != 0)) {
            gchar err_msg[1024];
            g_snprintf(err_msg, 1024, 
                "Cannot set filter on %s for pid %u:%s", 
                priv->demux_device, pid, g_strerror(errno));
            g_set_error(error, MDW_DVB_DEMUX_ERROR, MDW_DVB_DEMUX_ERROR_IOCTL_FAILED, err_msg);
            DEBUG_ERROR(err_msg);
            close(fd);

            return NULL;
        }
        filter = g_new0(struct filter_params, 1);
        filter->fd = fd;
        filter->pid = pid;
        //filter->is_pes = FALSE;
        //filter->buffer = NULL;
        //filter->buffer_size = 0;
        //filter->buffer_max_size = 0;
        //filter->handlers = NULL;
        
        guint16* p_pid = g_new(guint16, 1);
        *p_pid = pid;

        g_hash_table_insert(priv->filters, p_pid, filter);
        DEBUG_INFO("A new Section filter was allocated for pid: %u", pid);
    } else if (! has_key && is_pes == TRUE && to_create == TRUE) {
        /* open hardware demux */
        gint fd;
        if (G_UNLIKELY((fd = open(priv->demux_device, O_RDONLY | O_NONBLOCK)) == -1)) {
            gchar err_msg[1024];
            g_snprintf(err_msg, 1024, 
                "Cannot open demux device: %s for pid %u:%s", 
                priv->demux_device, pid, g_strerror(errno));
            g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno), err_msg);
            DEBUG_ERROR(err_msg);

            return NULL;
        }
        
        /* set filter */
        struct dmx_pes_filter_params dvb_params;
        memset(&dvb_params, 0, sizeof(struct dmx_pes_filter_params));
        dvb_params.pid = pid;
        dvb_params.input = input;
        dvb_params.output = output;
        dvb_params.pes_type = pes_type;

        if (G_UNLIKELY(ioctl(fd, DMX_SET_PES_FILTER, &dvb_params) != 0)) {
            gchar err_msg[1024];
            g_snprintf(err_msg, 1024, 
                "Cannot set filter on %s for pid %u:%s", 
                priv->demux_device, pid, g_strerror(errno));
            g_set_error(error, MDW_DVB_DEMUX_ERROR, MDW_DVB_DEMUX_ERROR_IOCTL_FAILED, err_msg);
            DEBUG_ERROR(err_msg);
            close(fd);

            return NULL;

        }
        filter = g_new0(struct filter_params, 1);
        filter->fd = fd;
        filter->pid = pid;
        filter->is_pes = TRUE;
        //filter->buffer = NULL;
        //filter->buffer_size = 0;
        //filter->buffer_max_size = 0;
        //filter->handlers = NULL;

        guint16* p_pid = g_new(guint16, 1);
        *p_pid = pid;

        g_hash_table_insert(priv->filters, p_pid, filter);
        DEBUG_INFO("A new PES filter was allocated for pid: %u", pid);
    } 
    if (filter && filter->is_pes == !is_pes)  {
        gchar err_msg[1024];
        g_sprintf(err_msg, "Trying to add a filter from incompatible type, it must be %s", filter->is_pes?"PES":"NOT PES");
        DEBUG_ERROR(err_msg);
        g_set_error(error, MDW_DVB_DEMUX_ERROR, MDW_DVB_DEMUX_ERROR_INVALID_CALL, err_msg);
        return NULL;
    }

    //if (! has_key && to_create == TRUE && (filter->is_pes == FALSE || output == DMX_OUT_TAP)) {
    return filter;
}
/***********************/
/* AUXILIARY FUNCTIONS */
/***********************/

static gpointer _thread_read_write_func(gpointer data)
{
    //
    //mdw_message_put(m, priv->main_queue);
    MdwDvbDemuxPrivate *priv = MDW_DVB_DEMUX_GET_PRIVATE(data);

    gboolean to_exit = FALSE;
    gboolean announce_ready = TRUE;
    gboolean announce_unpaused = FALSE;
    gboolean announce_paused = FALSE;

    gboolean paused = TRUE;
    while (to_exit == FALSE) {
        MdwMessage *m = NULL; 
        if (paused == FALSE) {
            _read_write_func(data);
        } else {
            usleep(10);
        }
        if (G_UNLIKELY(announce_ready == TRUE)) {
            DEBUG_INFO("thread_read_write is ready");
            m = mdw_message_new(MDW_MESSAGE_TYPE_READY, 0);
            mdw_message_put(m, priv->main_queue);
            announce_ready = FALSE;
        }
        if (G_UNLIKELY(announce_paused == TRUE)) {
            DEBUG_INFO("thread_read_write is paused");
            m = mdw_message_new(MDW_MESSAGE_TYPE_PAUSED, 0);
            mdw_message_put(m, priv->main_queue);
            announce_paused = FALSE;
        }
        if (G_UNLIKELY(announce_unpaused == TRUE)) {
            DEBUG_INFO("thread_read_write is unpaused");
            m = mdw_message_new(MDW_MESSAGE_TYPE_GOING, 0);
            mdw_message_put(m, priv->main_queue);
            announce_unpaused = FALSE;
        }
        m = NULL;
        m = mdw_message_get(priv->thread_read_write_queue, FALSE);
        if (G_UNLIKELY(m != NULL)) {
            g_assert(m->size == 0);
            g_assert(m->buffer == NULL);
            switch(m->type) {
                case MDW_MESSAGE_TYPE_PAUSE:
                    paused = TRUE;
                    announce_paused = TRUE;
                    //reply = mdw_message_new(MDW_MESSAGE_TYPE_PAUSED, 0);
                    break;
                case MDW_MESSAGE_TYPE_FINALIZE:
                    to_exit = TRUE;
                    break;
                case MDW_MESSAGE_TYPE_GO:
                    paused = FALSE;
                    announce_unpaused = TRUE;
                    //reply = mdw_message_new(MDW_MESSAGE_TYPE_GOING, 0);
                    break;
                default:
                    g_assert_not_reached();
            }
            //if (G_LIKELY(reply)) {
            //    mdw_message_put(reply, priv->main_queue);
            //}
            mdw_message_free(m, TRUE);
        }   
    }
    
    g_async_queue_lock(priv->thread_read_write_queue);
    {
        MdwMessage* m;
        while ((m = mdw_message_get_unlocked(priv->thread_read_write_queue, FALSE)) != NULL) {
            mdw_message_free(m, TRUE);
        }
        m = mdw_message_new(MDW_MESSAGE_TYPE_FINALIZED, 0);
        mdw_message_put(m, priv->main_queue);
    }
    g_async_queue_unlock(priv->thread_read_write_queue);

    DEBUG_INFO("thread_read_write is going is down");
    g_assert(g_async_queue_length(priv->thread_read_write_queue) == 0); 

    return NULL;
}

static gboolean 
_read_write_func (gpointer data) 
{
    MdwDvbDemux* self = MDW_DVB_DEMUX(data);
    MdwDvbDemuxPrivate* priv = MDW_DVB_DEMUX_GET_PRIVATE(self);
    guint8 buffer[65536];
    gint remain_bytes;

    g_assert(priv->state == MDW_DVB_DEMUX_STATE_PLAYING);

    if (G_UNLIKELY(priv->source == NULL)) {
        return TRUE;
    }

    if (MDW_IS_DVB_TUNER(priv->source)) {
        return TRUE;
    }
    
    g_assert(MDW_IS_TS_FILE_SOURCE(priv->source));

    if (G_UNLIKELY((remain_bytes = mdw_ts_source_read(priv->source, buffer, sizeof(buffer))) <= 0)) {
        DEBUG_WARN("ts_source returned: %d, going out", remain_bytes);
        return TRUE;
    }
    guint8 *p_buffer = buffer;
    gint bytes = 0; 
    while(remain_bytes > 0) {
        g_assert(priv->dvr_fd >= 0);
        bytes = write(priv->dvr_fd, p_buffer, remain_bytes);
        if (bytes == -1) {
            DEBUG_WARN("error while writing to dvr: %s", g_strerror(errno));
            break;
        } 
        p_buffer += bytes;
        remain_bytes -= bytes;
    }

    return TRUE;
}

static gboolean 
_filter_func (GIOChannel *source, GIOCondition condition, gpointer data) {
    struct filter_params* filter = (struct filter_params*) data;
     
    if (G_UNLIKELY(filter->buffer == NULL)) {
        g_assert(filter->buffer_max_size == 0);
        g_assert(filter->buffer_size == 0);
        if (filter->is_pes) {
            filter->buffer_max_size = 65536;
        } else {
            filter->buffer_max_size = 4096;
        }
        filter->buffer = g_new(guint8, filter->buffer_max_size);
        filter->p_buffer = filter->buffer;
    }

    g_assert(filter->buffer != NULL);
    g_assert(filter->p_buffer != NULL);
    g_assert(filter->buffer_max_size > 0);
    g_assert(filter->fd > 0);

    gint bytes = read(filter->fd, filter->p_buffer, filter->buffer_max_size - filter->buffer_size);
    if (G_UNLIKELY(bytes == -1)) {
        if(errno == EOVERFLOW) {
            DEBUG_WARN("Overflow while reading from demux (pid: %u)",
                    filter->pid);

        } else if (errno == EAGAIN) {
            DEBUG_WARN("pid %u, no data (EAGAIN) bug?", filter->pid);
        } else {
            /* see errno: in /usr/include/asm-generic/errno.h */
            DEBUG_WARN("Error while reading from demux (pid: %u):%d:%s",
                    filter->pid, errno,
                    g_strerror(errno));

        }
        return TRUE;
    } 
    if (G_UNLIKELY(bytes == 0)) {
        return TRUE;
    }
    filter->buffer_size += bytes;
    if (filter->is_pes) {
        _try_flush_pes(filter);
    } else {
        _try_flush_section(filter);
    }
    filter->p_buffer = filter->buffer + filter->buffer_size;

    return TRUE;
}

static void 
_filter_free(gpointer key, 
    gpointer value, 
    gpointer user_data)
{
    
    g_assert(key != NULL);
    g_assert(value != NULL);

    struct filter_params *filter = (struct filter_params*) value;
    guint16 pid = *(guint16*) key;
    g_assert(pid == filter->pid);

    DEBUG_INFO("Freeing the filter for pid %u", pid);
    
    g_free(key);
   
    close(filter->fd);
    GSList* l = filter->handlers;
    while(l) {
        g_object_unref(G_OBJECT(l->data));
        l = g_slist_next(l);
    }
    g_slist_free(filter->handlers);

    if (filter->buffer_max_size) {
        g_free(filter->buffer);
    }
   
    if (filter->io_channel) {
       g_io_channel_unref(filter->io_channel); 
    }
    g_free(filter);


}

static void
_filter_stop (gpointer key, 
        gpointer value, 
        gpointer user_data) 
{
    GError** error = (GError**) user_data;
    g_return_if_fail(error == NULL || *error == NULL);
    g_assert(value != NULL);
    g_assert(key != NULL);

    struct filter_params *filter = (struct filter_params*) value;
    guint16 pid = *(guint16*) key;
    g_assert(pid == filter->pid);

    DEBUG_INFO("Deiniting the filter for pid %u", pid);
    DEBUG_INFO("Executing ioctl(DMX_STOP) for pid %u in fd %d", pid, filter->fd);

    if (G_UNLIKELY(ioctl(filter->fd, DMX_STOP))) {
        gchar err_msg[1024];
        g_snprintf(err_msg, 1024, 
            "Error while executing ioctl DMX_STOP on fd %d (pid %u): %s", 
            filter->fd, filter->pid, g_strerror(errno));
        DEBUG_ERROR(err_msg);
        g_set_error(error, MDW_DVB_DEMUX_ERROR, MDW_DVB_DEMUX_ERROR_IOCTL_FAILED, err_msg);
        return;
    }
    if (filter->io_channel != NULL) {
        gboolean ret = g_source_remove(filter->source_id);
        g_assert(ret == TRUE);
    }


}
static void 
_filter_start(gpointer key, 
        gpointer value, 
        gpointer user_data) 
{
    GError** error = (GError**) user_data;
    g_return_if_fail(error == NULL || *error == NULL);
    g_assert(value != NULL);
    g_assert(key != NULL);

    struct filter_params *filter = (struct filter_params*) value;
    guint16 pid = *(guint16*) key;
    g_assert(pid == filter->pid);

    DEBUG_INFO("Initing the filter for pid %d", pid);

    if (filter->handlers == NULL) {
        /* not necessary */
        DEBUG_INFO("Initing filter for pid %d is not necessary, there's no handlers for it", pid);
        return;
    }

    DEBUG_INFO("Executing ioctl(DMX_START) for pid %u in fd %d", pid, filter->fd);
    if (G_UNLIKELY(ioctl(filter->fd, DMX_START))) {
        gchar err_msg[1024];
        g_snprintf(err_msg, 1024, 
            "Error while executing ioctl DMX_START on fd %d (pid %u): %s", 
            filter->fd, filter->pid, g_strerror(errno));
        DEBUG_ERROR(err_msg);
        g_set_error(error, MDW_DVB_DEMUX_ERROR, MDW_DVB_DEMUX_ERROR_IOCTL_FAILED, err_msg);
        return;
    }

    if (
            filter->handlers && 
            (MDW_IS_DVB_AUDIO_DUMMY(filter->handlers->data) || 
            MDW_IS_DVB_VIDEO_DUMMY(filter->handlers->data))
       ){
        /* not necessary */
        DEBUG_INFO("Setup a io_channel for pid %d is not necessary, the output is send directly to decoder", pid);
        return;
    }

    /* create IO channel */
    if (filter->io_channel == NULL) {
        filter->io_channel = g_io_channel_unix_new (filter->fd);
    }
    filter->source_id = g_io_add_watch(filter->io_channel, G_IO_IN, _filter_func, filter);

}
static void 
_try_flush_section(struct filter_params *filter) 
{
    g_assert(filter != NULL);
    if (filter->buffer_size > 3) {
        /* can read section length */
        guint16 length = (filter->buffer[1] << 8) | filter->buffer[2];
        length &= 0x0fff;
        if (filter->buffer_size >= length + 3) {
            DEBUG_INFO_DETAILED("pid %u, calling the handlers", filter->pid);
            /* call filters */

            GSList* l = filter->handlers;
            GSList* to_remove = NULL; 
            while(l) {
                MdwDemuxDataHandler *handler = (MdwDemuxDataHandler*) l->data;
                gboolean ret = mdw_demux_data_handler_new_data(handler, filter->pid, length + 3, filter->buffer);
                if (ret == FALSE) {
                    DEBUG_INFO("pid %u, handler: %p has asked to stop anoying him", filter->pid, (void *)handler);
                    to_remove = g_slist_append(to_remove, handler);
                }
                l = g_slist_next(l);
            }
            /* remove the members that does not more want being called */
            GSList* r = to_remove;
            while(r) {
                g_object_unref(G_OBJECT(r->data));
                filter->handlers = g_slist_remove(filter->handlers, r->data);
                r = g_slist_next(r);
            }
            g_slist_free(to_remove);

            if (G_UNLIKELY(filter->buffer_size > length + 3)) {
                DEBUG_WARN("After calling the filters DVB buffer stills holding data, please fixme");
            }
            /* FIXME: if there's more than one section it is discarted */
            filter->buffer_size = 0;
        }
    }

    /* TODO: if last_handler stop and delete filter */
}
static void 
_try_flush_pes(struct filter_params *filter) 
{
    g_assert(filter != NULL);

    if (filter->buffer_size < 6) { 
        return;
    }
    /* skipping prefix, the linuxtv driver already does this */
    guint16 pes_length;
    pes_length = (filter->buffer[4] << 8) | filter->buffer[5];
    if (filter->buffer_size >= pes_length + 6) {
        /* we have at least one full pes packet */
        DEBUG_INFO_DETAILED("pid %u, calling the handlers", filter->pid);
        /* call the filters */

        GSList* l = filter->handlers;
        GSList* to_remove = NULL; 
        while(l) {
            MdwDemuxDataHandler *handler = (MdwDemuxDataHandler*) l->data;
            gboolean ret = mdw_demux_data_handler_new_data(handler, filter->pid, pes_length + 6, filter->buffer);
            if (ret == FALSE) {
                DEBUG_INFO("pid %u, handler: %p has asked to stop anoying him", filter->pid, (void *)handler);
                to_remove = g_slist_append(to_remove, handler);
            }
            l = g_slist_next(l);
        }
        /* remove the members that does not more want being called */
        GSList* r = to_remove;
        while(r) {
            g_object_unref(G_OBJECT(r->data));
            filter->handlers = g_slist_remove(filter->handlers, r->data);
            r = g_slist_next(r);
        }
        g_slist_free(to_remove);
        if (filter->buffer_size > pes_length + 6) {
            DEBUG_WARN("After calling the filters DVB buffer stills holding data, please fixme");
        }
        /* FIXME: if there's more than one section it is discarted */
        filter->buffer_size = 0;
    }

    /* TODO: if last_handler stop and delete filter */
}
