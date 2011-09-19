/***************************************************************************
 *  SecFilter
 *  Tue Jun  5 13:59:27 2007 
 *
 *  Copyright (c) 2007 Iuri Diniz,,,
 *  Authors: Iuri Diniz,,, <iuri@email.com.br> 2007
 * 
 *  VERSION CONTROL INFORMATION: $Id$
 *
****************************************************************************/

#include "sec_filter.h"
#include "mdw_lots_dbus.h"

/********************/
/* local prototypes */
/********************/
/* interface overrided methods */
//static void _iface_namespace_my_interface_init (gpointer g_iface, gpointer data);
//static gboolean _iface_namespace_my_interface_method1 (NamespaceMyInterface *iface, guint param1);

/* destructors */
static void sec_filter_dispose (GObject *object);
static void sec_filter_finalize (GObject *object);

/* properties get/set functions*/
/*
static void sec_filter_set_property(GObject *object,
    guint property_id,
    const GValue *value,
    GParamSpec *pspec);
static void sec_filter_get_property(GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec);
*/
/* enum for enumerate properties */
/*
enum {
    SEC_FILTER_PROPERTY_1 = 1,
    SEC_FILTER_PROPERTY_2
};
*/
/* ERROR */
/*
GQuark 
sec_filter_error_quark() 
{
    static GQuark quark = 0;
    if (G_UNLIKELY(quark == 0)) {
        quark = g_quark_from_static_string("SecFilterError");
    }
    return quark;
}
#define SEC_FILTER_RAISE_ERROR(error, type, ...) \
{ \
    g_set_error((error), SEC_FILTER_ERROR, (type), __VA_ARGS__);\
    return FALSE; \
} while (0)

#define SEC_FILTER_RAISE_ERROR_IF_FAIL(error, cond, type, ...) \
{ \
    if (G_UNLIKELY(!(cond))) { \
         SEC_FILTER_RAISE_ERROR((error), (type), ...);\
    } \
} while (0)

// SEC_FILTER_ERROR_TYPE_1 
#define SEC_FILTER_RAISE_ERROR_TYPE_1_IF_FAIL(error, cond, ...) \
    SEC_FILTER_RAISE_ERROR_IF_FAILED((error), (cond), SEC_FILTER_ERROR_TYPE_1, __VA_ARGS__)
#define SEC_FILTER_RAISE_ERROR_TYPE_1(error, cond, ...) \
    SEC_FILTER_RAISE_ERROR((error), SEC_FILTER_ERROR_TYPE_1, __VA_ARGS__)

// SEC_FILTER_ERROR_TYPE_2
#define SEC_FILTER_RAISE_ERROR_TYPE_2_IF_FAIL(error, cond, ...) \
    SEC_FILTER_RAISE_ERROR_IF_FAIL((error), (cond), SEC_FILTER_ERROR_TYPE_2, __VA_ARGS__)
#define SEC_FILTER_RAISE_ERROR_TYPE_2(error, ...) \
    SEC_FILTER_RAISE_ERROR((error), SEC_FILTER_ERROR_TYPE_2, __VA_ARGS__)

// SEC_FILTER_ERROR_FAILED (General Failure)
#define SEC_FILTER_RAISE_ERROR_FAILED_IF_FAIL(error, cond, ...) \
    SEC_FILTER_RAISE_ERROR_IF_FAIL((error), (cond), SEC_FILTER_ERROR_FAILED, __VA_ARGS__)
#define SEC_FILTER_RAISE_ERROR_FAILED(error, cond, ...) \
    SEC_FILTER_RAISE_ERROR((error), SEC_FILTER_ERROR_FAILED, __VA_ARGS__)

// SEC_FILTER_ERROR_NOT_IMPLEMENTED (Not Implemented)
#define SEC_FILTER_RAISE_ERROR_NOT_IMPLEMENTED(error) \
    SEC_FILTER_RAISE_ERROR((error), SEC_FILTER_ERROR_NOT_IMPLEMENTED, "Not implemented: %s", __FUNC__)
*/

/*********** PRIVATE *************/
/* private field */

#define SEC_FILTER_GET_PRIVATE(o)  \
   (G_TYPE_INSTANCE_GET_PRIVATE ((o), SEC_TYPE_FILTER, SecFilterPrivate))

struct handler
{
    gint32 id;
    gint32 pid;
    gint32 table_id;
    gint32 table_id_ext;
    void (*callback)(const GByteArray*, void*);
    void *callback_data;
};

struct filter 
{
    GSList* handlers;
};

typedef struct _SecFilterPrivate SecFilterPrivate;
struct _SecFilterPrivate {
    GHashTable* filters; /* GHashTable<int, struct filter> */
    GHashTable* id_to_pid; /* GHashTable<int, int> */
    guint last_id;

    DBusGConnection *connection;
    DBusGProxy *demux;
};

/* private methods */

static void sec_filter_enable_watch(SecFilter*, guint);
static void sec_filter_new_section(SecFilter*, guint, const GByteArray*);
static void sec_filter_free_tables(SecFilter*);

/* auxiliary functions */

static void _watch_up_callback(DBusGProxy* demux, guint pid, gpointer user_data);
static void _watch_down_callback(DBusGProxy* demux, guint pid, gpointer user_data);
static void _section_callback(DBusGProxy* demux, guint pid, const GArray* array, gpointer user_data); 
static void _clean_up_id(gpointer key, gpointer value, gpointer user_data);


static guint g_uint16_hash(gconstpointer a);
static gboolean g_uint16_equal (gconstpointer a, gconstpointer b);
static guint g_uint_hash(gconstpointer a);
static gboolean g_uint_equal (gconstpointer a, gconstpointer b);

/***********************************************/
/* GObject obligatory definitions/declarations */
/***********************************************/

/* define my Object */
/*
G_DEFINE_TYPE(
    SecFilter, //my name in Camel case.
    sec_filter, // my name in lowercase, with words separated by '_'.
    G_TYPE_OBJECT //GType of my parent 
)
*/

/* define my Object (with interface)*/
/*
G_DEFINE_TYPE_WITH_CODE(
    SecFilter, //my name in Camel case.
    sec_filter, // my name in lowercase, with words separated by '_'.
    G_TYPE_OBJECT, //GType of my parent 
    G_IMPLEMENT_INTERFACE(NAMESPACE_TYPE_MY_INTERFACE, //interfaces that I implement
        _iface_namespace_my_interface_init)
)
*/

/* Init: define my Object (explict) */
static void     sec_filter_init       (SecFilter      *self);
static void     sec_filter_class_init (SecFilterClass *klass);
static gpointer sec_filter_parent_class = NULL;
static void     sec_filter_class_intern_init (gpointer klass)
{
  sec_filter_parent_class = g_type_class_peek_parent (klass);
  sec_filter_class_init ((SecFilterClass*) klass);
}

GType
sec_filter_get_type (void)
{
    static GType g_define_type_id = 0; 
    if (G_UNLIKELY (g_define_type_id == 0)) 
    { 
        static const GTypeInfo g_define_type_info = { 
            sizeof (SecFilterClass), 
            (GBaseInitFunc) NULL, 
            (GBaseFinalizeFunc) NULL, 
            (GClassInitFunc) sec_filter_class_intern_init, 
            (GClassFinalizeFunc) NULL, 
            NULL,   /* class_data */ 
            sizeof (SecFilter), 
            0,      /* n_preallocs */ 
            (GInstanceInitFunc) sec_filter_init, 
        }; 
        g_define_type_id = g_type_register_static (
            G_TYPE_OBJECT, //parent GType 
            "SecFilter", //my name in Camel case
            &g_define_type_info, 
            0
        ); 
        /* interfaces that I implement */
        /*
        {
            static const GInterfaceInfo g_implement_interface_info = {
                (GInterfaceInitFunc) _iface_namespace_my_interface_init 
            };
            g_type_add_interface_static (
                g_define_type_id, 
                NAMESPACE_TYPE_MY_INTERFACE, 
                &g_implement_interface_info
            );
        } 
        */
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
sec_filter_class_init (SecFilterClass *klass) 
{
    /* top ancestral */
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    /* private alocation */
    g_type_class_add_private (klass, sizeof (SecFilterPrivate));

    /* methods */
    klass->add_filter = sec_filter_add_filter;
    klass->remove = sec_filter_remove;

    /* destructors */
    gobject_class->dispose = sec_filter_dispose;
    gobject_class->finalize = sec_filter_finalize;

    /* properties */
    //GParamSpec *pspec;
    //gobject_class->set_property = sec_filter_set_property;
    //gobject_class->get_property = sec_filter_get_property;
    
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
        SEC_FILTER_PROPERTY_1,  //PROPERTY ID
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
        SEC_FILTER_PROPERTY_2,  //PROPERTY ID
        pspec 
    );
    g_param_spec_unref(pspec);
    */

    /* signals */

    /* chain-up */
}
/** 
 * Interface init: So let's override methods of NamespaceMyInterface 
 */
/*
static void 
_iface_namespace_my_interface_init (gpointer g_iface, gpointer data) 
{
    ((NamespaceMyInterfaceInterface*) g_iface)->method1 = 
        _iface_namespace_my_interface_method1; //override method1

}
*/

/**
 * GObject Internal Constructor 
 */
static void
sec_filter_init(SecFilter *self) 
{
    /* set the private members */
    //self->boolean_member = TRUE;
    //self->uint8_member = 42;


    SecFilterPrivate *priv = SEC_FILTER_GET_PRIVATE(self);
    priv->filters  = g_hash_table_new(g_uint16_hash, g_uint16_equal);
    priv->id_to_pid = g_hash_table_new(g_uint_hash, g_uint_equal);
    priv->last_id = 0;
}

/***************/
/* Destructors */
/***************/
static void    
sec_filter_dispose (GObject *object) 
{   
    //SecFilter *self =  SEC_FILTER(object);
    //SecFilterPrivate *priv = SEC_FILTER_GET_PRIVATE(self);
    /* 
    if (self->dispose_has_run) {
        return;
    }
    */

    /* chaining up */
    G_OBJECT_CLASS(sec_filter_parent_class)->dispose(object);
}
static void    
sec_filter_finalize (GObject *object) 
{
    SecFilter *self =  SEC_FILTER(object);
    //SecFilterPrivate *priv = SEC_FILTER_GET_PRIVATE(self);

    sec_filter_free_tables(self);

    /* chaining up */
    G_OBJECT_CLASS(sec_filter_parent_class)->finalize(object);
}

/* Public */
void
sec_filter_unref(SecFilter* filter)
{
    g_return_if_fail(filter != NULL);
    g_object_unref(G_OBJECT(filter));
}

/**********************/
/* PROPERTIES GET/SET */
/**********************/
/**
 * Properties set function
 */
/*
static void 
sec_filter_set_property(GObject *object,
    guint property_id,
    const GValue *value,
    GParamSpec *pspec) 
{
    SecFilter *self = SEC_FILTER(object);
    //SecFilterPrivate *priv = SEC_FILTER_GET_PRIVATE(self);
    switch (property_id) {
        case SEC_FILTER_PROPERTY_1:
            g_free(self->property1);
            self->property1 = g_value_dup_string(value);
            break;
        case SEC_FILTER_PROPERTY_2:
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
sec_filter_get_property(GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec) 
{
    SecFilter *self = SEC_FILTER(object);
    //SecFilterPrivate *priv = SEC_FILTER_GET_PRIVATE(self);
    switch (property_id) {
        case SEC_FILTER_PROPERTY_1:
            g_value_set_string(value, self->property1);
            break;
        case SEC_FILTER_PROPERTY_2:
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


SecFilter* 
sec_filter_new() {
    g_type_init();
    return (SecFilter*) g_object_new(SEC_TYPE_FILTER, NULL);
}


/*
SecFilter* 
sec_filter_new_with_arg1(guint8 param1) {
    return (SecFilter*) g_object_new(SEC_TYPE_FILTER, "property 1", param1, NULL);
}
*/


/**********************/
/* Overrided methods  */
/**********************/
/*
static gboolean _iface_namespace_my_interface_method1 (NamespaceMyInterface *iface, guint param1)
{
    SecFilter *self = SEC_FILTER(iface);
    //
    return TRUE;
}
*/

/******************/
/* PUBLIC METHODS */
/******************/
guint 
sec_filter_add_filter(
    SecFilter* self, 
    guint16 pid, gint32 table_id, gint32 table_id_ext, 
    void(*callback)(const GByteArray* __data, void* __user_data),
    void* user_data) 
{
    g_return_val_if_fail(self != NULL, 0);
    g_return_val_if_fail(pid < 8192, 0);
    g_return_val_if_fail(table_id < 256 && table_id >= -1, 0);
    g_return_val_if_fail(table_id_ext < 65536 && table_id_ext >= -1, 0);
    g_return_val_if_fail(callback != NULL, 0);
    
    /* extra check */
    g_return_val_if_fail(table_id != -1 || (table_id == -1 && table_id_ext == -1), 0);

    SecFilterPrivate* priv = SEC_FILTER_GET_PRIVATE(self);
    struct filter* filter = NULL;
    gboolean add_to_hash = FALSE;

    g_debug("Searching for the pid %u on filters", pid);
    filter = (struct filter*) g_hash_table_lookup(priv->filters, &pid);
    
    if (filter == NULL) {
        g_debug("Filter for pid %u not found, I must alloc a new one", pid);
        guint16* p_pid = g_new(guint16, 1);
        *p_pid = pid;
        add_to_hash = TRUE;
        filter = g_new0(struct filter, 1);
        g_hash_table_insert(priv->filters, p_pid, filter);
    }
    g_assert(filter != NULL);

    if (filter->handlers == NULL) {
        /* first handler */
        /* enable watch */
        sec_filter_enable_watch(self, pid);
    }
    
    struct handler* handler = g_new(struct handler, 1);
    
    /* FIXME: catch the max value of id */
    g_assert(G_MAXUINT != priv->last_id);

    handler->id = ++priv->last_id;
    handler->pid = pid;
    handler->table_id = table_id;
    handler->table_id_ext = table_id_ext;
    handler->callback = callback;
    handler->callback_data = user_data;

    filter->handlers = g_slist_append(filter->handlers, handler);

    /* id_to_pid_map */
    g_assert(g_hash_table_lookup(priv->id_to_pid, &handler->id) == NULL);
    {
        guint *p_id = g_new0(guint, 1);
        guint16 *p_pid = g_new0(guint16, 1);
        *p_id = handler->id;
        *p_pid = handler->pid;

        g_hash_table_insert(priv->id_to_pid, p_id, p_pid);
    } 

    return handler->id;
}

gboolean 
sec_filter_remove(SecFilter* self, guint id) 
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(id > 0, FALSE);

    
    SecFilterPrivate* priv = SEC_FILTER_GET_PRIVATE(self);

    guint16* p_pid = NULL;
    guint* p_id = NULL;

    g_assert(priv->id_to_pid != NULL);

    //gboolean has_key = g_hash_table_lookup_extended(priv->id_to_pid, &id, (gpointer) &p_id, (gpointer) &p_pid);
    //g_debug("has_key: %s", has_key == TRUE?"TRUE":"FALSE");
    //g_debug("p_id: %p", (void*) p_id);
    //g_debug("p_pid: %p", (void*) p_pid);
    g_debug("Trying to remove handler with id %u", id);
    g_hash_table_lookup_extended(priv->id_to_pid, &id, (gpointer) &p_id, (gpointer) &p_pid);
    if (p_pid == NULL) {
        g_debug("Asked to remove handler with id %u but it was not found", id); 
        return FALSE;    
    }
    /* remove handler */
    
    struct filter* filter = NULL;
    guint16* p_pid_orig = NULL;

    g_hash_table_lookup_extended(priv->filters, p_pid, (gpointer) &p_pid_orig, (gpointer) &filter);

    g_assert(p_pid_orig != NULL);
    g_assert(filter != NULL);
    g_assert(filter->handlers != NULL);
    
    GSList *l = filter->handlers;

    while (l != NULL) {
        struct handler* handler = (struct handler *) l->data;
        if (handler->id == id) {
            g_debug("Handler with id %u (for pid %u) was removed", handler->id, handler->pid);
            /* Was found, removing */
            filter->handlers = g_slist_remove(filter->handlers, handler);
            g_free(handler);
            break;
        }
        l = g_slist_next(l);
        
    }
    g_assert(l != NULL);

    if (filter->handlers == NULL) {
        g_debug("The last handler for the pid %u was removed, removing the filter too", *p_pid);
        /* remove entry from hash table priv->filters */ 
        g_hash_table_remove(priv->filters, p_pid);
        g_free(p_pid_orig);
        g_free(filter);

        /* disable watch */
        /* TODO: catch error */
        com_eitv_lots_demuxer_del_watch(priv->demux, *p_pid, NULL); 
    }
    /* remove entry from hash table priv->id_to_pid */
    {
        g_hash_table_remove(priv->id_to_pid, p_id);
        g_free(p_pid);
        g_free(p_id);
    }
    return TRUE;
    
}

/*******************/
/* PRIVATE METHODS */
/*******************/
static void 
sec_filter_enable_watch(SecFilter* self, guint pid)
{
    g_assert(self != NULL);
    g_assert(pid <= 8192);

    SecFilterPrivate *priv = SEC_FILTER_GET_PRIVATE(self);

    if (priv->connection == NULL) {
        priv->connection =  dbus_g_bus_get(DBUS_BUS_SESSION, NULL);
        if (priv->connection == NULL) {
            g_debug("Cannot connect to bus Session BUS");
            return;
        }
    }

    g_assert(priv->connection != NULL);

    /* TODO: get this from environment */
    const gchar* dbus_service = "com.eitv.lots";
    const gchar* dbus_remote_object = "/com/eitv/lots";
    const gchar* dbus_remote_interface = "com.eitv.lots.demuxer";

    if (priv->demux == NULL) {
        priv->demux = dbus_g_proxy_new_for_name(priv->connection, 
            dbus_service,
            dbus_remote_object,
            dbus_remote_interface
        );
        if (priv->demux == NULL) {
            g_debug("Cannot get a proxy for %s:%s:%s", 
                dbus_service, dbus_remote_object, dbus_remote_interface
            );
            return;
        }
        /* signals */
        dbus_g_object_register_marshaller(g_cclosure_marshal_VOID__UINT_POINTER,
                G_TYPE_NONE,
                G_TYPE_UINT, DBUS_TYPE_G_UCHAR_ARRAY,
                G_TYPE_INVALID
        );
        dbus_g_object_register_marshaller(g_cclosure_marshal_VOID__UINT,
                G_TYPE_NONE,
                G_TYPE_UINT,
                G_TYPE_INVALID
        );
        dbus_g_proxy_add_signal (priv->demux, "new_payload",
                G_TYPE_UINT,
                DBUS_TYPE_G_UCHAR_ARRAY,
                G_TYPE_INVALID
        );
        dbus_g_proxy_add_signal(priv->demux, "watch_pid_up", 
                G_TYPE_UINT,
                G_TYPE_INVALID
        );
        dbus_g_proxy_add_signal(priv->demux, "watch_pid_down", 
                G_TYPE_UINT,
                G_TYPE_INVALID
        );

        dbus_g_proxy_connect_signal(priv->demux, "new_payload", 
                G_CALLBACK(_section_callback),
                self,
                NULL
        );
        dbus_g_proxy_connect_signal(priv->demux, "watch_pid_up",
                G_CALLBACK(_watch_up_callback),
                self,
                NULL
        );
        dbus_g_proxy_connect_signal(priv->demux, "watch_pid_down",
                G_CALLBACK(_watch_down_callback),
                self,
                NULL
        );
    }
    g_assert(priv->demux != NULL);
    /* TODO: catch error */
    com_eitv_lots_demuxer_add_watch(priv->demux, pid, NULL);


}

static void 
sec_filter_new_section(SecFilter* self, guint pid, const GByteArray* data) 
{
    g_assert(self != NULL);
    g_assert(pid <= 8192);
    g_assert(data != NULL);

    SecFilterPrivate *priv = SEC_FILTER_GET_PRIVATE(self);
    
    struct filter* filter = NULL;
    filter = g_hash_table_lookup(priv->filters, &pid);

    if (filter == NULL) {
        g_debug("There's no handler for pid %u", pid); 
        return;
    }

    g_assert(filter != NULL);
    g_assert(filter->handlers != NULL);

    GSList *l = filter->handlers;
    /* filter by table_id and table_id_ext */
    g_assert(data->len > 4);
    guint8 table_id = data->data[0];
    /* TODO: verify if syntax indication is true */
    guint16 table_id_ext = (data->data[3] << 8)&& data->data[4];
    g_debug("new section: pid: %u, table_id: %u, table_id_ext: %u", pid, table_id, table_id_ext);
    while(l) {
        struct handler* handler = (struct handler *) l->data;
        if (
            handler->table_id == -1 || 
            (handler->table_id == table_id && 
                (handler->table_id_ext == -1 || handler->table_id_ext == table_id_ext)) 
        ){ 
            g_debug("Filter matched, calling filter with id %u", handler->id);
            handler->callback(data, handler->callback_data); 
        }
        l = g_slist_next(l);
    }
    
}
static void
sec_filter_free_tables(SecFilter *self)
{
    /* cleanup all hash tables */
    g_assert(self != NULL);
    SecFilterPrivate *priv = SEC_FILTER_GET_PRIVATE(self);

    g_assert(priv->filters != NULL);
    g_assert(priv->id_to_pid != NULL);

    g_hash_table_foreach(priv->id_to_pid, _clean_up_id, self); 

    g_hash_table_destroy(priv->filters);
    g_hash_table_destroy(priv->id_to_pid);

}

/***********************/
/* AUXILIARY FUNCTIONS */
/***********************/
static guint 
g_uint16_hash(gconstpointer a) {
    return *((guint16*) a) % 32;
}

static gboolean 
g_uint16_equal (gconstpointer a, gconstpointer b) 
{
    //g_debug("g_uint16_equal %ux%u", *((const guint16*) a), *((const guint16*)b));
    return *((const guint16*) a) == *((const guint16*)b);
}

static guint 
g_uint_hash(gconstpointer a) {
    return *((guint*) a) % 32;
}

static gboolean 
g_uint_equal (gconstpointer a, gconstpointer b) 
{
    //g_debug("g_uint_equal %ux%u", *((const guint*) a), *((const guint*)b));
    return *((const guint*) a) == *((const guint*)b);
}

static void
_clean_up_id (gpointer key, gpointer value, gpointer user_data)
{
    guint* p_id = (guint *) key;
    g_warning("Destroying filter with id %u (it was not removed at end)", *p_id);
    SecFilter* filter = SEC_FILTER(user_data);
    
    sec_filter_remove(filter, *p_id);
}

static void 
_watch_up_callback(DBusGProxy* demux, guint pid, gpointer user_data) 
{
    //g_debug("Watch up callback, pid %u", pid);
    /* IMHO implementing this is unnecessary) */
    g_debug("Watch up callback called, pid %u", pid); 
}

/* TODO: please someone, test it */
static void 
_watch_down_callback(DBusGProxy* demux, guint pid, gpointer user_data) 
{
    //g_debug("Watch down callback, pid %u", pid);
    /* FIXME: A pid may go down while in use by me */
    g_debug("Watch down callback called, pid %u", pid);
    g_assert(user_data != NULL);
    SecFilterPrivate* priv = SEC_FILTER_GET_PRIVATE(user_data);

    struct filter *filter = g_hash_table_lookup(priv->filters, &pid);
    if (filter != NULL) {
        g_warning("Reactivating the pid %u I need it :-P", pid);
        /* TODO: catch error */
        com_eitv_lots_demuxer_add_watch(demux, pid, NULL);
    }

}

static void 
_section_callback(DBusGProxy* demux, guint pid, const GArray* array, gpointer user_data) 
{
    g_return_if_fail(pid <= 8192);
    g_debug("New section for pid %u", pid);
    sec_filter_new_section(SEC_FILTER(user_data), pid, (const GByteArray*) array);
}
