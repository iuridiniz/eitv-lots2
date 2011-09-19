/***************************************************************************
 *  MdwDefaultDemux
 *  Thu Mar 29 17:44:47 2007 
 *
 *  Copyright (c) 2007 Iuri Diniz,,,
 *  Authors: Iuri Diniz,,, <iuri@digizap.com.br> 2007
 * 
 *  VERSION CONTROL INFORMATION: $Id$
 *
****************************************************************************/

#include <unistd.h>
#include <sched.h>
#include <string.h>

#include "mdw_demux.h"
#include "mdw_default_demux.h"
#include "mdw_util.h"

/********************/
/* local prototypes */
/********************/
/* interface overrided methods */
static void _iface_mdw_demux_init (gpointer g_iface, gpointer data);

/* interface methods */
static gboolean _iface_mdw_demux_add_section_handler (MdwDemux *self, MdwDemuxDataHandler *handler, guint16 pid);
static gboolean _iface_mdw_demux_add_pes_handler (MdwDemux *self, MdwDemuxDataHandler *handler, guint16 pid); 
static gboolean _iface_mdw_demux_del_section_handler (MdwDemux *self, MdwDemuxDataHandler *handler, guint16 pid);
static gboolean _iface_mdw_demux_del_pes_handler (MdwDemux *self, MdwDemuxDataHandler *handler, guint16 pid); 
static gboolean _iface_mdw_demux_set_ts_source (MdwDemux *self, MdwTsSource* source); 
static gboolean _iface_mdw_demux_start (MdwDemux *self); 
static gboolean _iface_mdw_demux_stop (MdwDemux *self); 

/* destructors */
static void mdw_default_demux_dispose (GObject *object);
static void mdw_default_demux_finalize (GObject *object);

/* properties get/set functions*/
/*
static void mdw_default_demux_set_property(GObject *object,
    guint property_id,
    const GValue *value,
    GParamSpec *pspec);
static void mdw_default_demux_get_property(GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec);
*/
/* enum for enumerate properties */
/*
enum {
    MDW_DEFAULT_DEMUX_PROPERTY_1 = 1,
    MDW_DEFAULT_DEMUX_PROPERTY_2
};
*/

/* Private Fields*/
#define MDW_DEFAULT_DEMUX_GET_PRIVATE(o)  \
   (G_TYPE_INSTANCE_GET_PRIVATE ((o), MDW_TYPE_DEFAULT_DEMUX, MdwDefaultDemuxPrivate))

typedef struct _MdwDefaultDemuxPrivate MdwDefaultDemuxPrivate;
struct _MdwDefaultDemuxPrivate {
    GHashTable *filters;
    MdwTsSource* source;
    gboolean is_started;
    gboolean dispose_has_run;
    guint read_write_source_id;
};

struct filter_params {
    gboolean is_pes;
    GSList* handlers;
    guint8* buffer;
    guint8* p_buffer;
    guint32 buffer_size;
    guint32 buffer_max_size; 
};

/* private methods */
static struct filter_params* mdw_default_demux_get_filter(MdwDefaultDemux* self, guint16 pid, gboolean is_pes, gboolean to_create);
static gboolean mdw_default_demux_remove_handler(MdwDefaultDemux *self, MdwDemuxDataHandler *handler, guint16 pid, gboolean is_pes);
static gboolean mdw_default_demux_add_handler(MdwDefaultDemux *self, MdwDemuxDataHandler *handler, guint16 pid, gboolean is_pes);

/* auxiliary */
static gboolean _read_write_func(gpointer data);
static void _flush_buffer(struct filter_params* filter, guint8* buffer, guint32 buffer_size, guint16 pid); 
static int _fill_section(guint8* buffer, guint32* p_buffer_size, guint8* payload, guint8 payload_size); 
static int _fill_pes(guint8* buffer, guint32* p_buffer_size, guint8* payload, guint8 payload_size); 
static void _free_filter (gpointer key, gpointer value, gpointer user_data);

/***********************************************/
/* GObject obrigatory definations/declarations */
/***********************************************/


/* Init: define my Object (explict) */
static void     mdw_default_demux_init       (MdwDefaultDemux      *self);
static void     mdw_default_demux_class_init (MdwDefaultDemuxClass *klass);
static gpointer mdw_default_demux_parent_class = NULL;
static void     mdw_default_demux_class_intern_init (gpointer klass)
{
  mdw_default_demux_parent_class = g_type_class_peek_parent (klass);
  mdw_default_demux_class_init ((MdwDefaultDemuxClass*) klass);
}

GType
mdw_default_demux_get_type (void)
{
    static GType g_define_type_id = 0; 
    if (G_UNLIKELY (g_define_type_id == 0)) 
    { 
        static const GTypeInfo g_define_type_info = { 
            sizeof (MdwDefaultDemuxClass), 
            (GBaseInitFunc) NULL, 
            (GBaseFinalizeFunc) NULL, 
            (GClassInitFunc) mdw_default_demux_class_intern_init, 
            (GClassFinalizeFunc) NULL, 
            NULL,   /* class_data */ 
            sizeof (MdwDefaultDemux), 
            0,      /* n_preallocs */ 
            (GInstanceInitFunc) mdw_default_demux_init, 
        }; 
        g_define_type_id = g_type_register_static (
            G_TYPE_OBJECT, //parent GType 
            "MdwDefaultDemux", //my name in Camel case
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
mdw_default_demux_class_init (MdwDefaultDemuxClass *klass) 
{
    /* top ancestral */
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    
    /* private */
    g_type_class_add_private (gobject_class, sizeof (MdwDefaultDemuxPrivate));

    /* methods */
    //klass->method1 = mdw_default_demux_method1;
    //klass->method2 = mdw_default_demux_method2;

    /* destructors */
    gobject_class->dispose = mdw_default_demux_dispose;
    gobject_class->finalize = mdw_default_demux_finalize;

    /* properties */
    //GParamSpec *pspec;
    //gobject_class->set_property = mdw_default_demux_set_property;
    //gobject_class->get_property = mdw_default_demux_get_property;
    
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
        MDW_DEFAULT_DEMUX_PROPERTY_1,  //PROPERTY ID
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
        MDW_DEFAULT_DEMUX_PROPERTY_2,  //PROPERTY ID
        pspec 
    );
    g_param_spec_unref(pspec);
    */

    /* signals */

    /* chain-up */

    /* threads */
    //if(!g_thread_supported()) g_thread_init (NULL);
}

/**
 * GObject Internal Constructor 
 */
static void
mdw_default_demux_init(MdwDefaultDemux *self) 
{
    MdwDefaultDemuxPrivate *priv =  MDW_DEFAULT_DEMUX_GET_PRIVATE(self);
    priv->filters = g_hash_table_new(mdw_guint16_hash, mdw_guint16_equal);
}

/** 
 * Interface init: So let's override methods of MdwDemux 
 */
static void 
_iface_mdw_demux_init (gpointer g_iface, gpointer data) 
{
    MdwDemuxInterface *demux_iface = (MdwDemuxInterface*) g_iface;
    demux_iface->add_section_handler = _iface_mdw_demux_add_section_handler;
    demux_iface->add_pes_handler = _iface_mdw_demux_add_pes_handler;
    demux_iface->set_ts_source = _iface_mdw_demux_set_ts_source;
    demux_iface->start = _iface_mdw_demux_start;
    demux_iface->stop = _iface_mdw_demux_stop;

    demux_iface->del_section_handler = _iface_mdw_demux_del_section_handler;
    demux_iface->del_pes_handler = _iface_mdw_demux_del_pes_handler;

}

/**********************/
/* PROPERTIES GET/SET */
/**********************/
/**
 * Properties set function
 */
/*
static void 
mdw_default_demux_set_property(GObject *object,
    guint property_id,
    const GValue *value,
    GParamSpec *pspec) 
{
    MdwDefaultDemux *self = MDW_DEFAULT_DEMUX(object);
    switch (property_id) {
        case MDW_DEFAULT_DEMUX_PROPERTY_1:
            g_free(self->property1);
            self->property1 = g_value_dup_string(value);
            break;
        case MDW_DEFAULT_DEMUX_PROPERTY_2:
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
mdw_default_demux_get_property(GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec) 
{
    MdwDefaultDemux *self = MDW_DEFAULT_DEMUX(object);
    switch (property_id) {
        case MDW_DEFAULT_DEMUX_PROPERTY_1:
            g_value_set_string(value, self->property1);
            break;
        case MDW_DEFAULT_DEMUX_PROPERTY_2:
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

MdwDefaultDemux* 
mdw_default_demux_new() {
    MdwDefaultDemux* obj = (MdwDefaultDemux*) g_object_new(MDW_TYPE_DEFAULT_DEMUX, NULL);

    return obj;
}

/*
MdwDefaultDemux* 
mdw_default_demux_new_with_arg1(guint8 param1) {
    return (MdwDefaultDemux*) g_object_new(MDW_TYPE_DEFAULT_DEMUX, "property 1", param1, NULL);
}
*/


/***************/
/* Destructors */
/***************/
static void    
mdw_default_demux_dispose (GObject *object) 
{   
    MdwDefaultDemux *self =  MDW_DEFAULT_DEMUX(object);
    MdwDefaultDemuxPrivate *priv =  MDW_DEFAULT_DEMUX_GET_PRIVATE(self);

    if (priv->dispose_has_run) {
        return;
    }
    priv->dispose_has_run = TRUE;

    if (priv->is_started) {
        mdw_demux_stop(MDW_DEMUX(self));
    }
    
    
    /* unref source */
    mdw_demux_set_ts_source(MDW_DEMUX(self), NULL);
    
    /* destroy hash_table */
    g_hash_table_foreach (priv->filters, _free_filter, NULL);

    /* chaining up */
    G_OBJECT_CLASS(mdw_default_demux_parent_class)->dispose(object);
}
static void    
mdw_default_demux_finalize (GObject *object) 
{

    MdwDefaultDemuxPrivate *priv = MDW_DEFAULT_DEMUX_GET_PRIVATE(object);
    
    g_hash_table_destroy(priv->filters);

    /* chaining up */
    G_OBJECT_CLASS(mdw_default_demux_parent_class)->finalize(object);
}
/**********************/
/* Overrided methods  */
/**********************/
static gboolean
_iface_mdw_demux_add_section_handler (MdwDemux *iface, MdwDemuxDataHandler *handler, guint16 pid)
{
    return mdw_default_demux_add_handler(MDW_DEFAULT_DEMUX(iface), handler, pid, FALSE);
}

static gboolean
_iface_mdw_demux_add_pes_handler (MdwDemux *iface, MdwDemuxDataHandler *handler, guint16 pid)
{
    return mdw_default_demux_add_handler(MDW_DEFAULT_DEMUX(iface), handler, pid, TRUE);
}

static gboolean
_iface_mdw_demux_set_ts_source (MdwDemux *iface, MdwTsSource* source)
{
    //return FALSE;
    MdwDefaultDemuxPrivate* priv = MDW_DEFAULT_DEMUX_GET_PRIVATE(iface);

    if (source != NULL && ! MDW_IS_TS_SOURCE(source)) {
        DEBUG_ERROR("source is not a valid ts source");
        return FALSE;
    }
    if (priv->source) {
        g_object_unref(priv->source);
    }
    priv->source = source;

    if (source) {
        g_object_ref(source);
    }

    return TRUE;
    
}

static gboolean
_iface_mdw_demux_start (MdwDemux *iface)
{
    //return FALSE;
    /* let's init the thread */
    MdwDefaultDemux* self = MDW_DEFAULT_DEMUX(iface);
    MdwDefaultDemuxPrivate* priv = MDW_DEFAULT_DEMUX_GET_PRIVATE(self);

    if(priv->is_started) {
        DEBUG_WARN("The demux process has already started");
        return FALSE;
    }

    priv->read_write_source_id = g_idle_add(_read_write_func, self);
    //priv->read_write_source_id = g_timeout_add(100, _read_write_func, self);

    priv->is_started = TRUE;

    return TRUE;
}

static gboolean
_iface_mdw_demux_stop (MdwDemux *iface)
{

    MdwDefaultDemuxPrivate* priv = MDW_DEFAULT_DEMUX_GET_PRIVATE(iface);
    if(!priv->is_started) {
        DEBUG_WARN("The demux process has not started");
        return FALSE;
    }
    gboolean ret = g_source_remove(priv->read_write_source_id);
    g_assert(ret == TRUE); 

    priv->is_started = FALSE;

    return TRUE;
}

static gboolean
_iface_mdw_demux_del_section_handler (MdwDemux *iface, MdwDemuxDataHandler *handler, guint16 pid)
{
    return mdw_default_demux_remove_handler(MDW_DEFAULT_DEMUX(iface), handler, pid, FALSE);
}

static gboolean
_iface_mdw_demux_del_pes_handler (MdwDemux *iface, MdwDemuxDataHandler *handler, guint16 pid)
{
    return mdw_default_demux_remove_handler(MDW_DEFAULT_DEMUX(iface), handler, pid, TRUE);
}

/*******************/
/* PRIVATE METHODS */
/*******************/
static gboolean 
mdw_default_demux_add_handler(MdwDefaultDemux *self, MdwDemuxDataHandler *handler, guint16 pid, gboolean is_pes)
{
    struct filter_params* filter = mdw_default_demux_get_filter(self, pid, is_pes, TRUE);
   
    if (filter == NULL) {
        DEBUG_INFO("Filter is NULL, cannot add a handler");
        return FALSE;
    }

    g_object_ref(handler);
    filter->handlers = g_slist_append(filter->handlers, handler);
    return TRUE;
}

static gboolean 
mdw_default_demux_remove_handler(MdwDefaultDemux *self, MdwDemuxDataHandler *handler, guint16 pid, gboolean is_pes)
{

    //MdwDefaultDemuxPrivate* priv = MDW_DEFAULT_DEMUX_GET_PRIVATE(self);
    struct filter_params* filter = mdw_default_demux_get_filter(self, pid, is_pes, FALSE);

    if (filter == NULL) {
        DEBUG_INFO("Filter not found for pid %u", pid);
        return FALSE;
    }

    DEBUG_INFO("I have found a filter for pid %u, so let's search the handler %p", pid, (void *)handler);
    if (g_slist_find(filter->handlers, handler) == NULL) {
        DEBUG_INFO("Handler %p not found (for pid %u)", (void *)handler, pid);
        return FALSE;
    }
    filter->handlers = g_slist_remove(filter->handlers, handler);
    g_object_unref(handler);
    DEBUG_INFO("Handler %p removed (for pid %u)", (void *)handler, pid);
    
    return TRUE;
}

static struct filter_params* 
mdw_default_demux_get_filter(MdwDefaultDemux* self, guint16 pid, gboolean is_pes, gboolean to_create)
{
    MdwDefaultDemuxPrivate *priv = MDW_DEFAULT_DEMUX_GET_PRIVATE(self);
    
    struct filter_params* filter = NULL;

    filter = g_hash_table_lookup(priv->filters, &pid);

    if (to_create && filter == NULL) {
        DEBUG_INFO("There isn't a filter for pid %u, I must allocate a new one", pid);
        guint16 *key = g_new(guint16, 1);
        *key = pid;
        filter = g_new0(struct filter_params, 1);
        filter->is_pes = is_pes;
        g_hash_table_insert(priv->filters, key, filter);
    }
    if (filter != NULL && filter->is_pes != is_pes) {
        DEBUG_INFO("Filter has a incompatible type (%s)", filter->is_pes?"PES":"NOT PES");
        return NULL;
    }
    return filter;
}

/***********************/
/* AUXILIARY FUNCTIONS */
/***********************/

static void 
_free_filter (gpointer key, gpointer value, gpointer user_data) 
{
    
    g_assert(key != NULL);
    g_assert(value != NULL);

    g_free(key);

    struct filter_params* filter = (struct filter_params*) value;

    GSList* l = filter->handlers;
    while(l) {
        g_object_unref(G_OBJECT(l->data));
        l = g_slist_next(l);
    }
    g_slist_free(filter->handlers);

    if (filter->buffer_max_size) {
        g_free(filter->buffer);
    }

    g_free(filter);
    
}

static int 
_fill_section(
    guint8* buffer,
    guint32* p_buffer_size,
    guint8* payload,
    guint8 payload_size) 
{
    guint32 buffer_size = *p_buffer_size;
    if (payload_size == 0) {
        return FALSE;
    }
    if (buffer_size == 0) {
        /* pointer field - shift it */
        guint8 shift = payload[0];
        if (shift + 1 > payload_size) {
            DEBUG_ERROR("Pointer field points to out of bounds");
            return FALSE;
        }
        guint8* section_start = payload + shift + 1;
        guint8 size = payload_size - (shift + 1);
        memcpy(buffer, section_start, size);
        buffer_size = size;
    } else {
        guint8 n_to_write = 0;
        guint8* buffer_offset = buffer + buffer_size;
        if (buffer_size + payload_size > 4096) {
            /* cannot write out to of my bounds */
            n_to_write = 4096 - buffer_size;
            /* will be flushed anyway */
        } else {
            n_to_write = payload_size;
        }
        memcpy(buffer_offset, payload, n_to_write);
        buffer_size += n_to_write;
    }
    if (buffer_size < 4) {
        /* cannot read section length */
        *p_buffer_size = buffer_size;
        return FALSE;
    }
    guint16 section_length = (buffer[1] << 8) | buffer[2];
    /* only 12 bits */
    section_length &= 0x0fff;
    if (buffer_size < section_length + 3) {
        /* cannot read a full section */
        if (section_length + 3 > 4096) {
            DEBUG_WARN("Bad section packet");
            /* discard all */
            buffer_size = 0;
        }
        *p_buffer_size = buffer_size;
        return FALSE;
    }
    /* we have a full section */
    /* discard pos section_length bytes */
    buffer_size = section_length + 3;
    *p_buffer_size = buffer_size;
    return TRUE;
}
static int 
_fill_pes(
    guint8* buffer,
    guint32* p_buffer_size,
    guint8* payload,
    guint8 payload_size) 
{
    guint32 buffer_size = *p_buffer_size;
    if (payload_size == 0) {
        return FALSE;
    }
    guint8 n_to_write = 0;
    guint8* buffer_offset = buffer + buffer_size;
    if (buffer_size + payload_size > 65536) {
        /* cannot write out to of my bounds */
        n_to_write = 65536 - buffer_size;
        /* will be flushed anyway */
    } else {
        n_to_write = payload_size;
    }
    memcpy(buffer_offset, payload, n_to_write);
    buffer_size += n_to_write;

    /* Let's verify if is full packet */
    if (buffer_size < 6) {
        /* cannot read pes length */
        *p_buffer_size = buffer_size;
        return FALSE;
    }
    /* sanity check */
    guint32 prefix = (buffer[0] << 16) | (buffer[1] << 8) | buffer[2];
    if (prefix != 0x000001) {
        *p_buffer_size = 0;
        DEBUG_ERROR("Cannot found prefix on PES packet, discarding all");
        return FALSE;
    }

    guint16 pes_length = (buffer[4] << 8) | buffer[5];
    if (buffer_size < pes_length + 6) {
        /* cannot read a full packet */
        if (pes_length + 6 > 65536) {
            DEBUG_WARN("Bad PES packet, discarding all");
            /* discard all */
            buffer_size = 0;
        }
        *p_buffer_size = buffer_size;
        return FALSE;
    }
    /* we have a full PES packet*/
    /* discard pos pes_length bytes */
    buffer_size = pes_length + 6;
    *p_buffer_size = buffer_size;
    return TRUE;
}

static void 
_flush_buffer(struct filter_params* filter, guint8* buffer, guint32 buffer_size, guint16 pid) 
{
    DEBUG_INFO_DETAILED("Let's flush a section for pid %u", pid);
    GSList* l = filter->handlers;
    GSList* to_remove = NULL; 
    while(l) {
        MdwDemuxDataHandler *handler = (MdwDemuxDataHandler*) l->data;
        gboolean ret = mdw_demux_data_handler_new_data(handler, pid, buffer_size, buffer);
        if (ret == FALSE) {
            DEBUG_INFO("pid %u, handler: %p has asked to stop anoying him", pid, (void *)handler);
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

    /* TODO: if last handler delete filter */
}

static gboolean
_read_write_func(gpointer data)
{
    MdwDefaultDemux* self = MDW_DEFAULT_DEMUX(data);
    MdwDefaultDemuxPrivate* priv = MDW_DEFAULT_DEMUX_GET_PRIVATE(self); 

    g_assert(priv->is_started == TRUE);

    if (priv->source == NULL) {
        return TRUE;
    }
    gint bytes;
    guint8 buffer[188];

    if (G_UNLIKELY((bytes = mdw_ts_source_read(priv->source, buffer, 188)) != 188)) {
        DEBUG_INFO_DETAILED("Read from source returned %u", bytes);
    } 

    guint8 sync_byte = buffer[0];
    if (G_UNLIKELY(sync_byte != 0x47)) {
        DEBUG_WARN("Wrong sync byte of TS Packet");
        return TRUE;
    }
    guint16 pid = ((buffer[1] & 0x1f) << 8) | buffer[2];
    struct filter_params* filter = NULL;
    filter = g_hash_table_lookup(priv->filters, &pid);
    if (filter == NULL) {
        DEBUG_INFO_DETAILED("There isn't a filter for pid %u", pid);
        return TRUE;
    }

    if (G_UNLIKELY(filter->buffer_max_size == 0)) {
        DEBUG_INFO("There isn't a buffer for pid %u allocating a new one", pid);
        
        filter->buffer_max_size = filter->is_pes?65536:4096;
        filter->buffer_size = 0;
        filter->buffer = g_new(guint8, filter->buffer_max_size);
    }
    guint8 adp_field = buffer[3] >> 4 & 0x3;
    guint8* payload_start = buffer + 4;
    guint8* payload_end;
    if (adp_field == 0 || /* reserved */
            adp_field == 2) { /* adaptation field only, no payload */
        DEBUG_INFO_DETAILED("no payload discarding packet");
        return TRUE;
    }
    if (adp_field == 3) {
        /* skip adaptation field */
        payload_start += buffer[4] + 1;
    } 

    payload_end = buffer + 188;
    if (payload_start > payload_end) {
        DEBUG_WARN("Adaptation field is crazy, I don't want to fo out of my bounds");
        return TRUE;
    }
    gboolean is_full = FALSE;
    if (filter->is_pes) {
        is_full = _fill_pes(
                filter->buffer,
                &(filter->buffer_size),
                payload_start,
                payload_end - payload_start
        );

    } else {
        is_full = _fill_section(
                filter->buffer,
                &(filter->buffer_size),
                payload_start,
                payload_end - payload_start
        );
    }
    if (is_full) {
        _flush_buffer(filter, filter->buffer, filter->buffer_size, pid);
        filter->buffer_size = 0;
    } 


    return TRUE;
}
/******************/
/* PUBLIC METHODS */
/******************/
