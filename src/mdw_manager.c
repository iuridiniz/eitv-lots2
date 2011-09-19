/***************************************************************************
 *  MdwManager
 *  Wed Apr  4 16:18:31 2007 
 *
 *  Copyright (c) 2007 Iuri Diniz,,,
 *  Authors: Iuri Diniz,,, <iuri@email.com.br> 2007
 * 
 *  VERSION CONTROL INFORMATION: $Id$
 *
****************************************************************************/
/* TODO: this class is a remote class, we must be very careful to do not crash, 
 * so we must change asserts and check of glib to own checks that returns a GError
 */
#include <dbus/dbus-glib-bindings.h>

#include "mdw_ts_source.h"
#include "mdw_ts_file_source.h"

#include "mdw_demux.h"

#include "mdw_player.h"
#include "mdw_util.h"

#include "mdw_manager.h"
#include "mdw_manager_dbus.h"
#include "mdw_marshallers.h"

/********************/
/* local prototypes */
/********************/
/* interface overrided methods */
static void _iface_mdw_demux_data_handler_init (gpointer g_iface, gpointer data);
static gboolean _iface_mdw_demux_data_handler_new_data (MdwDemuxDataHandler *iface, const guint16 pid, const gsize size, const guint8* data);

/* destructors */
static void mdw_manager_dispose (GObject *object);
static void mdw_manager_finalize (GObject *object);

/* properties get/set functions*/
static void mdw_manager_set_property(GObject *object,
    guint property_id,
    const GValue *value,
    GParamSpec *pspec);
static void mdw_manager_get_property(GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec);

/* enum for enumerate properties */
enum {
    MDW_MANAGER_PROPERTY_SOURCE_URI = 1,
    MDW_MANAGER_PROPERTY_BLA
};
/* enum for enumerate signals */
enum {
    MDW_MANAGER_NEW_PAYLOAD,
    MDW_MANAGER_WATCH_PID_UP,
    MDW_MANAGER_WATCH_PID_DOWN,
    MDW_MANAGER_LAST_SIGNAL
};

static guint mdw_manager_signal[MDW_MANAGER_LAST_SIGNAL] = { 0 };


/* error */
GQuark 
mdw_manager_error_quark() 
{
    static GQuark quark = 0;
    if (G_UNLIKELY(quark == 0)) {
        quark = g_quark_from_static_string("MdwManagerError");
    }
    return quark;
}
#define MDW_MANAGER_RAISE_ERROR_IF_FAILED(error, cond, type, ...) \
{ \
    if (G_UNLIKELY(!(cond))) { \
        g_set_error((error), MDW_MANAGER_ERROR, (type), __VA_ARGS__);\
        return FALSE; \
    } \
} while (0)
#define MDW_MANAGER_RAISE_ERROR_INVALID_PARAMS_IF_FAILED(error, cond, ...) \
    MDW_MANAGER_RAISE_ERROR_IF_FAILED((error), (cond), MDW_MANAGER_ERROR_INVALID_PARAMS, __VA_ARGS__)
#define MDW_MANAGER_RAISE_ERROR_INVALID_CALL_IF_FAILED(error, cond, ...) \
    MDW_MANAGER_RAISE_ERROR_IF_FAILED((error), (cond), MDW_MANAGER_ERROR_INVALID_CALL, __VA_ARGS__)
#define MDW_MANAGER_RAISE_ERROR_NOT_IMPLEMENTED(error) \
    MDW_MANAGER_RAISE_ERROR_IF_FAILED((error), FALSE, MDW_MANAGER_ERROR_NOT_IMPLEMENTED, "Not implemented")
    
/*********** PRIVATE *******************/
/* private field */


#define MDW_MANAGER_GET_PRIVATE(o)  \
   (G_TYPE_INSTANCE_GET_PRIVATE ((o), MDW_TYPE_MANAGER, MdwManagerPrivate))

typedef struct _MdwManagerPrivate MdwManagerPrivate;
struct _MdwManagerPrivate {
    gchar* source_uri;
    guint video_pid;
    guint audio_pid;
    guint pcr_pid;
    gboolean is_playing;

    guint max_w;
    guint max_h;

    MdwTsSource *source;
    MdwDemux *demux;
    MdwPlayer *player;

    gboolean dispose_has_run;

    DBusGConnection *connection;
    guint8 pids_count[8192];
};


/* private methods */
static inline gboolean mdw_manager_setup (MdwManager *self, GError** error);

/* auxiliary functions */




/***********************************************/
/* GObject obrigatory definations/declarations */
/***********************************************/

/* define my Object */
/*
G_DEFINE_TYPE(
    MdwManager, //my name in Camel case.
    mdw_manager, // my name in lowercase, with words separated by '_'.
    G_TYPE_OBJECT //GType of my parent 
)
*/

/* define my Object (with interface)*/
/*
G_DEFINE_TYPE_WITH_CODE(
    MdwManager, //my name in Camel case.
    mdw_manager, // my name in lowercase, with words separated by '_'.
    G_TYPE_OBJECT, //GType of my parent 
    G_IMPLEMENT_INTERFACE(NAMESPACE_TYPE_MY_INTERFACE, //interfaces that I implement
        _iface_namespace_my_interface_init)
)
*/

/* Init: define my Object (explict) */
static void     mdw_manager_init       (MdwManager      *self);
static void     mdw_manager_class_init (MdwManagerClass *klass);
//static void     mdw_manager_class_finalize (MdwManagerClass *klass);
static gpointer mdw_manager_parent_class = NULL;
static void     mdw_manager_class_intern_init (gpointer klass)
{
    mdw_manager_parent_class = g_type_class_peek_parent (klass);
    mdw_manager_class_init ((MdwManagerClass*) klass);
}
/*static void     mdw_manager_class_intern_finalize (gpointer klass) 
{
    mdw_manager_class_finalize ((MdwManagerClass*) klass);
}*/
GType
mdw_manager_get_type (void)
{
    static GType g_define_type_id = 0; 
    if (G_UNLIKELY (g_define_type_id == 0)) 
    { 
        static const GTypeInfo g_define_type_info = { 
            sizeof (MdwManagerClass), 
            (GBaseInitFunc) NULL, 
            (GBaseFinalizeFunc) NULL, 
            (GClassInitFunc) mdw_manager_class_intern_init, 
            //(GClassFinalizeFunc) mdw_manager_class_intern_finalize, 
            (GClassFinalizeFunc) NULL, 
            NULL,   /* class_data */ 
            sizeof (MdwManager), 
            0,      /* n_preallocs */ 
            (GInstanceInitFunc) mdw_manager_init, 
        }; 
        g_define_type_id = g_type_register_static (
            G_TYPE_OBJECT, //parent GType 
            "MdwManager", //my name in Camel case
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
mdw_manager_class_init (MdwManagerClass *klass) 
{
    /* top ancestral */
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    /* private alocation */
    g_type_class_add_private (klass, sizeof (MdwManagerPrivate));

    /* methods */
    klass->play_with_pids = mdw_manager_play_with_pids;
    klass->stop = mdw_manager_stop;
    klass->set_size = mdw_manager_set_size;
    klass->set_pos = mdw_manager_set_pos;
    klass->raise_to_top = mdw_manager_raise_to_top;
    klass->lower_to_bottom = mdw_manager_lower_to_bottom;

    klass->add_watch = mdw_manager_add_watch;
    klass->del_watch = mdw_manager_del_watch;
    klass->count_watch = mdw_manager_count_watch;

    klass->play = mdw_manager_play;

    klass->is_playing = mdw_manager_is_playing;
    klass->get_size = mdw_manager_get_size;
    klass->get_max_size = mdw_manager_get_max_size;
    klass->get_pos = mdw_manager_get_pos;
    klass->get_video_pid = mdw_manager_get_video_pid;
    klass->set_video_pid = mdw_manager_set_video_pid;
    klass->get_audio_pid = mdw_manager_get_audio_pid;
    klass->set_audio_pid = mdw_manager_set_audio_pid;
    klass->get_pcr_pid = mdw_manager_get_pcr_pid;
    klass->set_pcr_pid = mdw_manager_set_pcr_pid;
    klass->set_uri = mdw_manager_set_uri;
    klass->get_uri = mdw_manager_get_uri;

    /* destructors */
    gobject_class->dispose = mdw_manager_dispose;
    gobject_class->finalize = mdw_manager_finalize;

    /* properties */
    GParamSpec *pspec;
    gobject_class->set_property = mdw_manager_set_property;
    gobject_class->get_property = mdw_manager_get_property;
    
    /* Property 1 (type: string) (can be defined on contruction) (read/write)*/
    pspec = g_param_spec_string( // string property 
        "uri", //name
        "uri", //nick
        "Set/Get the uri", //description
        "null", //default value
        G_PARAM_READWRITE | G_PARAM_CONSTRUCT //flags
    );
    g_object_class_install_property(
        gobject_class, 
        MDW_MANAGER_PROPERTY_SOURCE_URI,  //PROPERTY ID
        pspec 
    );
    g_param_spec_unref(pspec);
    

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
        MDW_MANAGER_PROPERTY_2,  //PROPERTY ID
        pspec 
    );
    g_param_spec_unref(pspec);
    */

    /* FIXME: GAMBIARRA: for have dbus types initialized I need to setup a connection */
    //dbus_g_type_specialized_init();
    //dbus_g_type_specialized_builtins_init();
    DBusGConnection *connection = NULL;
    connection = dbus_g_bus_get (DBUS_BUS_SESSION, NULL);
    if (connection) {
        dbus_g_connection_unref(connection);
    }

    /* signals */
    mdw_manager_signal[MDW_MANAGER_NEW_PAYLOAD] = g_signal_new(
        "new_payload",
        MDW_TYPE_MANAGER,
        G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
        0,
        NULL, NULL,
        g_cclosure_user_marshal_VOID__UINT_BOXED,
        G_TYPE_NONE,
        2, G_TYPE_UINT, DBUS_TYPE_G_UCHAR_ARRAY
    );
    mdw_manager_signal[MDW_MANAGER_WATCH_PID_UP] = g_signal_new(
        "watch_pid_up",
        MDW_TYPE_MANAGER,
        G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
        0,
        NULL, NULL,
        g_cclosure_marshal_VOID__UINT,
        G_TYPE_NONE,
        1, G_TYPE_UINT
    );
    mdw_manager_signal[MDW_MANAGER_WATCH_PID_DOWN] = g_signal_new(
        "watch_pid_down",
        MDW_TYPE_MANAGER,
        G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
        0,
        NULL, NULL,
        g_cclosure_marshal_VOID__UINT,
        G_TYPE_NONE,
        1, G_TYPE_UINT
    );
      
    /* DBus */
    dbus_g_object_type_install_info(MDW_TYPE_MANAGER, &dbus_glib_mdw_manager_object_info);
    /* chain-up */

}
/*static void     
mdw_manager_class_finalize (MdwManagerClass *klass) 
{
    if (klass->connection) 
    {
        dbus_g_connection_unref(klass->connection);
    }
}*/

/**
 * GObject Internal Constructor 
 */
static void
mdw_manager_init(MdwManager *self) 
{
    /* set the private members */
    //self->boolean_member = TRUE;
    //self->uint8_member = 42;

    //MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(self);

}

/** 
 * Interface init: So let's override methods of MdwDemuxDataHandler
 */
static void 
_iface_mdw_demux_data_handler_init (gpointer g_iface, gpointer data) 
{
    ((MdwDemuxDataHandlerInterface*) g_iface)->new_data = 
        _iface_mdw_demux_data_handler_new_data; //override new_data

}

/**********************/
/* PROPERTIES GET/SET */
/**********************/
/**
 * Properties set function
 */

static void 
mdw_manager_set_property(GObject *object,
    guint property_id,
    const GValue *value,
    GParamSpec *pspec) 
{
    //MdwManager *self = MDW_MANAGER(object);
    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(object);
    switch (property_id) {
        case MDW_MANAGER_PROPERTY_SOURCE_URI:
            g_free(priv->source_uri);
            priv->source_uri = g_value_dup_string(value);
            break;
        
        case MDW_MANAGER_PROPERTY_BLA:
            //self->property2 = g_value_get_int(value);
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
mdw_manager_get_property(GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec) 
{
    //MdwManager *self = MDW_MANAGER(object);
    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(object);
    switch (property_id) {
        case MDW_MANAGER_PROPERTY_SOURCE_URI:
            g_value_set_string(value, priv->source_uri);
            break;
        
        case MDW_MANAGER_PROPERTY_BLA:
            //g_value_set_int(value, self->property2);
            break;
        
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }

}


/************************/
/* Public Constructors */
/***********************/

MdwManager* 
mdw_manager_new() {
    return (MdwManager*) g_object_new(MDW_TYPE_MANAGER, NULL);
}

/*
MdwManager* 
mdw_manager_new_with_arg1(guint8 param1) {
    return (MdwManager*) g_object_new(MDW_TYPE_MANAGER, "property 1", param1, NULL);
}
*/


/***************/
/* Destructors */
/***************/
static void    
mdw_manager_dispose (GObject *object) 
{   
    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(object);
    if (priv->connection) {
        dbus_g_connection_unref(priv->connection);
        priv->connection = NULL;
    }
    if (priv->is_playing) {
        mdw_manager_stop(MDW_MANAGER(object), NULL);
    }
    if (priv->dispose_has_run) {
        return;
    }
    priv->dispose_has_run = TRUE;
    if (priv->demux) {
        g_object_unref(priv->demux);
        priv->demux = NULL;
    }
    if (priv->source) {
        g_object_unref(priv->source);
        priv->source = NULL;
    }
    if (priv->player) {
        g_object_unref(priv->player);
        priv->player = NULL;
    }

    /* chaining up */
    G_OBJECT_CLASS(mdw_manager_parent_class)->dispose(object);
}
static void    
mdw_manager_finalize (GObject *object) 
{

    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(object);
    if (priv->source_uri) {
        g_free(priv->source_uri);
        priv->source_uri = NULL;
    }
    /* chaining up */
    G_OBJECT_CLASS(mdw_manager_parent_class)->finalize(object);
}
/**********************/
/* Overrided methods  */
/**********************/
static gboolean 
_iface_mdw_demux_data_handler_new_data (MdwDemuxDataHandler *iface, const guint16 pid, const gsize size, const guint8* data)
{
    g_assert(iface != NULL);
    g_assert(pid <= 8192);
    g_assert(size <= 4096);
    g_assert(data != NULL);

    MdwManager *self = MDW_MANAGER(iface);
    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(self);
    
    /* FIXME: async calls may fail due the concorrency, needs a lock */
    if(G_UNLIKELY(priv->pids_count[pid] == 0)) {
        return FALSE;
    }

    GArray* a = g_array_sized_new(FALSE, FALSE, 1, size);
    a = g_array_append_vals(a, data, size);

    g_signal_emit(self, 
        mdw_manager_signal[MDW_MANAGER_NEW_PAYLOAD], 
        0,
        pid, a
    );
    g_array_free(a, TRUE);
    return TRUE;
}

/******************/
/* PUBLIC METHODS */
/******************/
/* not exported */
gboolean 
mdw_manager_dbus_register (MdwManager* self, GError **error)
{

    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(self);
    g_return_val_if_fail(priv->connection == NULL, FALSE);

    DBusGProxy *driver_proxy;
    guint request_ret;

    priv->connection = dbus_g_bus_get (DBUS_BUS_SESSION, error);
    if (priv->connection == NULL) {
        DEBUG_WARN("Unable to connect to dbus");
        return FALSE;
    }
    dbus_g_connection_register_g_object(priv->connection, "/com/eitv/lots", G_OBJECT(self));

    driver_proxy = dbus_g_proxy_new_for_name(priv->connection, DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS);

    if (!org_freedesktop_DBus_request_name(driver_proxy, "com.eitv.lots", 0, &request_ret, error)) {
        DEBUG_WARN("Unable to register name name on bus");
        return FALSE;
    }

    g_object_unref(driver_proxy);
    return TRUE;
}

/* exported methods */
/*
 * must report programming errors with GErrors and not by g_return_*  
 * (the self parameter and out parameters are ok to report by g_return_* if NULL ) 
 */
gboolean
mdw_manager_play_with_pids (MdwManager* self, const guint IN_video_pid, const guint IN_audio_pid, const guint IN_pcr_pid, GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    MDW_MANAGER_RAISE_ERROR_INVALID_PARAMS_IF_FAILED(
        error,
        (IN_video_pid && IN_video_pid < 8192), 
        "video_pid must be > 0 and < 8192"
    );
    MDW_MANAGER_RAISE_ERROR_INVALID_PARAMS_IF_FAILED(
        error,
        (IN_audio_pid && IN_audio_pid < 8192), 
        "audio_pid must be > 0 and < 8192"
    );
    MDW_MANAGER_RAISE_ERROR_INVALID_PARAMS_IF_FAILED(
        error, 
        (IN_pcr_pid && IN_pcr_pid < 8192), 
        "pcr_pid must be > 0 and < 8192"
    );

    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(self);

    priv->video_pid = IN_video_pid;
    priv->audio_pid = IN_audio_pid;
    priv->pcr_pid = IN_pcr_pid;
    return mdw_manager_play(self, error);
}

gboolean
mdw_manager_stop (MdwManager* self, GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    gboolean ret = FALSE;
    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(self);

    //g_return_val_if_fail(priv->is_playing == TRUE, FALSE);
    
    MDW_MANAGER_RAISE_ERROR_INVALID_CALL_IF_FAILED(
        error, 
        priv->is_playing == TRUE, 
        "the player must be playing"
    );
    
    g_assert(priv->player != NULL);

    ret = mdw_player_stop(priv->player, error);
    if (ret == FALSE) {
        return FALSE;
    }
    priv->is_playing = FALSE;

    return TRUE;
}

gboolean
mdw_manager_set_size (MdwManager *self, const guint IN_width, const guint IN_height, GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(self);

    if (!mdw_manager_setup(self, error)) {
        return FALSE;
    }

    //g_return_val_if_fail(IN_width <= priv->max_w, FALSE);
    //g_return_val_if_fail(IN_height <= priv->max_h, FALSE);
    
    MDW_MANAGER_RAISE_ERROR_INVALID_PARAMS_IF_FAILED(
        error, IN_width <= priv->max_w, 
        "width must be <= %u (max_width)", priv->max_w
    );  

    MDW_MANAGER_RAISE_ERROR_INVALID_PARAMS_IF_FAILED(
        error, IN_height <= priv->max_h, 
        "height must be <= %u (max_height)", priv->max_h
    );  

    return mdw_player_set_size(priv->player, IN_width, IN_height, error);

    //return TRUE;
}

gboolean
mdw_manager_set_pos (MdwManager *self, const guint IN_pos_x, const guint IN_pos_y, GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);
    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(self);

    if (!mdw_manager_setup(self, error)) {
        return FALSE;
    }

    //g_return_val_if_fail(IN_pos_x <= priv->max_w, FALSE);
    //g_return_val_if_fail(IN_pos_y <= priv->max_h, FALSE);

    MDW_MANAGER_RAISE_ERROR_INVALID_PARAMS_IF_FAILED(
        error, IN_pos_x <= priv->max_w, 
        "pos_x must be <= %u (max_width)", priv->max_w
    );  
    MDW_MANAGER_RAISE_ERROR_INVALID_PARAMS_IF_FAILED(
        error, IN_pos_y <= priv->max_h, 
        "pos_y must be <= %u (max_height)", priv->max_h
    );  

    return mdw_player_set_pos(priv->player, IN_pos_x, IN_pos_y, error);

}

gboolean
mdw_manager_raise_to_top (MdwManager *self, GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);
    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(self);
    if (!mdw_manager_setup(self, error)) {
        return FALSE;
    }
    return mdw_player_raise_to_top(priv->player, error);
}

gboolean
mdw_manager_lower_to_bottom (MdwManager *self, GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);
    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(self);
    if (!mdw_manager_setup(self, error)) {
        return FALSE;
    }
    return mdw_player_lower_to_bottom(priv->player, error);
}

gboolean
mdw_manager_add_watch  (MdwManager *self, const guint IN_pid, GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    MDW_MANAGER_RAISE_ERROR_INVALID_PARAMS_IF_FAILED(
        error, 
        (IN_pid < 8192), 
        "pid must be >= 0 and < 8192"
    );
    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(self);

    MDW_MANAGER_RAISE_ERROR_INVALID_CALL_IF_FAILED(
        error, 
        (priv->pids_count[IN_pid] <= 50),
        "the maximum watch for a pid is 50"
    );
    if (!mdw_manager_setup(self, error)) {
        return FALSE;
    }
    /* store the old_count due the concorrency */ 
    /* FIXME: use g_atomic_* */
    int old_count = priv->pids_count[IN_pid]++;
    if (old_count == 0) {
        g_signal_emit(self, 
            mdw_manager_signal[MDW_MANAGER_WATCH_PID_UP], 
            0,
            IN_pid);
        g_assert(priv->demux != NULL);
        mdw_demux_add_section_handler(priv->demux, MDW_DEMUX_DATA_HANDLER(self), IN_pid);

    }

    
    return TRUE;
}

gboolean
mdw_manager_del_watch  (MdwManager *self, const guint IN_pid, GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    MDW_MANAGER_RAISE_ERROR_INVALID_PARAMS_IF_FAILED(
        error, 
        (IN_pid < 8192), 
        "pid must be >= 0 and < 8192"
    );
    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(self);

    MDW_MANAGER_RAISE_ERROR_INVALID_CALL_IF_FAILED(
        error, 
        (priv->pids_count[IN_pid] > 0),
        "there's no registered watchs for this pid"
    );

    if (!mdw_manager_setup(self, error)) {
        return FALSE;
    }

    /* store the old_count due the concorrency */ 
    /* FIXME: use g_atomic_* */
    int old_count = priv->pids_count[IN_pid]--;

    if (old_count == 0) {
        g_signal_emit(self, 
            mdw_manager_signal[MDW_MANAGER_WATCH_PID_DOWN], 
            0,
            IN_pid);
        g_assert(priv->demux != NULL);
        mdw_demux_del_section_handler(priv->demux, MDW_DEMUX_DATA_HANDLER(self), IN_pid);
    }

    return TRUE;

}

gboolean 
mdw_manager_count_watch (MdwManager *self, const guint IN_pid, guint* OUT_count, GError **error) 
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(OUT_count != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    MDW_MANAGER_RAISE_ERROR_INVALID_PARAMS_IF_FAILED(
        error, 
        (IN_pid < 8192), 
        "pid must be > 0 and < 8192"
    );
    
    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(self);
    *OUT_count = priv->pids_count[IN_pid];

    return TRUE;

}

gboolean
mdw_manager_play (MdwManager* self, GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    gboolean ret = FALSE;
    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(self);
    
    MDW_MANAGER_RAISE_ERROR_INVALID_CALL_IF_FAILED(error, priv->audio_pid != 0, "Please set a audio pid")
    MDW_MANAGER_RAISE_ERROR_INVALID_CALL_IF_FAILED(error, priv->pcr_pid != 0, "Please set a pcr pid")
    MDW_MANAGER_RAISE_ERROR_INVALID_CALL_IF_FAILED(error, priv->video_pid != 0, "Please set a video pid")
    
    g_assert(priv->audio_pid < 8192);
    g_assert(priv->video_pid < 8192);
    g_assert(priv->pcr_pid < 8192);

    if (priv->is_playing == TRUE) {
        mdw_manager_stop(self, NULL);
    }

    g_assert(priv->is_playing == FALSE);

    if (!mdw_manager_setup(self, error)) {
        return FALSE;
    }

    mdw_player_set_video_pid(priv->player, priv->video_pid);
    mdw_player_set_audio_pid(priv->player, priv->audio_pid);
    ret = mdw_player_play(priv->player, error);
    if (ret == FALSE) {
        return FALSE;
    }
    priv->is_playing = TRUE; 
    return TRUE;
}

gboolean
mdw_manager_is_playing (MdwManager *self, gboolean* OUT_is_playing, GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(OUT_is_playing != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(self);
    *OUT_is_playing = priv->is_playing; 
    return TRUE;
}

gboolean
mdw_manager_get_size (MdwManager *self, guint* OUT_width, guint* OUT_height, GError **error)
{

    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(OUT_width != NULL, FALSE);
    g_return_val_if_fail(OUT_height != NULL, FALSE); 
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    if (!mdw_manager_setup(self, error)) {
        return FALSE;
    }

    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(self);
    return mdw_player_get_size(priv->player, (guint16*)OUT_width, (guint16*)OUT_height, error);

}

gboolean
mdw_manager_get_max_size (MdwManager *self, guint* OUT_max_width, guint* OUT_max_height, GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(OUT_max_width != NULL, FALSE);
    g_return_val_if_fail(OUT_max_height != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(self);

    if (!mdw_manager_setup(self, error)) {
        return FALSE;
    }
    *OUT_max_width = priv->max_w;
    *OUT_max_height = priv->max_h;

    return TRUE;
}

gboolean
mdw_manager_get_pos (MdwManager *self, guint* OUT_pos_x, guint* OUT_pos_y, GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(OUT_pos_x != NULL, FALSE);
    g_return_val_if_fail(OUT_pos_y != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);
    
    if (!mdw_manager_setup(self, error)) {
        return FALSE;
    }
    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(self);
    return mdw_player_get_pos(priv->player, (guint16*) OUT_pos_x, (guint16*) OUT_pos_y, error);
}

gboolean
mdw_manager_get_video_pid (MdwManager *self, guint* OUT_pid, GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(OUT_pid != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(self);
    *OUT_pid = priv->video_pid;

    return TRUE;
}

gboolean
mdw_manager_set_video_pid (MdwManager *self, const guint IN_pid, GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(IN_pid && IN_pid < 8192, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(self);
    priv->video_pid = IN_pid;
    if (priv->is_playing) {
        return mdw_manager_play(self, error);
    }

    return TRUE;
}

gboolean
mdw_manager_get_audio_pid (MdwManager *self, guint* OUT_pid, GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(OUT_pid != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(self);
    *OUT_pid = priv->audio_pid;

    return TRUE;
}

gboolean
mdw_manager_set_audio_pid (MdwManager *self, const guint IN_pid, GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(IN_pid && IN_pid < 8192, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(self);
    priv->audio_pid = IN_pid;
    if (priv->is_playing) {
        return mdw_manager_play(self, error);
    }
    return TRUE;
}

gboolean
mdw_manager_get_pcr_pid (MdwManager *self, guint* OUT_pid, GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(OUT_pid != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(self);
    *OUT_pid = priv->pcr_pid;

    return TRUE;
}

gboolean
mdw_manager_set_pcr_pid (MdwManager *self, const guint IN_pid, GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(IN_pid && IN_pid < 8192, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(self);
    priv->pcr_pid = IN_pid;
    if (priv->is_playing) {
        return mdw_manager_play(self, error);
    }
    return TRUE;
}

gboolean
mdw_manager_set_uri (MdwManager *self, const char *IN_uri, GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(IN_uri != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(self);

    DEBUG_INFO("Setting the URI");
    MDW_MANAGER_RAISE_ERROR_INVALID_CALL_IF_FAILED(error, mdw_verify_uri(IN_uri), "Invalid uri");
    g_object_set(self, "uri", IN_uri, NULL);

    if (priv->is_playing) {
        mdw_manager_stop(self, NULL);
    }
    g_assert(priv->is_playing == FALSE);
    priv->video_pid = 0;
    priv->audio_pid = 0;
    priv->pcr_pid = 0;

    if (priv->source) {
        g_object_unref(priv->source);
        priv->source = NULL;
        if (priv->demux) {
            mdw_demux_set_ts_source(priv->demux, priv->source);
        }
    }
    
    return TRUE;
}

gboolean
mdw_manager_get_uri (MdwManager *self, char **OUT_uri, GError **error)
{
    g_return_val_if_fail(self != NULL, FALSE);
    g_return_val_if_fail(OUT_uri != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    DEBUG_INFO("Getting the URI");
    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(self);
    *OUT_uri = g_strdup(priv->source_uri);
    return TRUE;
}


/*******************/
/* PRIVATE METHODS */
/*******************/
static inline gboolean
mdw_manager_setup (MdwManager *self, GError** error) 
{
    g_assert(self != NULL);
    g_assert(error == NULL || *error == NULL);

    MdwManagerPrivate *priv = MDW_MANAGER_GET_PRIVATE(self);
    gboolean set_source = FALSE;
    if (G_UNLIKELY(priv->source == NULL)) {
        MDW_MANAGER_RAISE_ERROR_INVALID_CALL_IF_FAILED(error, 
            mdw_verify_uri(priv->source_uri) == TRUE, 
            "uri '%s' is invalid", priv->source_uri);
        priv->source = mdw_get_most_suitable_ts_source(priv->source_uri);
        g_assert(priv->source != NULL);
        set_source = TRUE;
    }

    if (G_UNLIKELY(priv->demux == NULL)) {
        priv->demux = mdw_get_most_suitable_demux(priv->demux);
        g_assert(priv->demux != NULL);
        set_source = TRUE;
    }
    if (set_source) {
        mdw_demux_set_ts_source(priv->demux, priv->source);
    }
   
    if (G_UNLIKELY(priv->player == NULL)) {
        guint16 w,h;
        priv->player = mdw_get_most_suitable_player();
        g_assert(priv->player != NULL);
        mdw_player_set_demux(priv->player, priv->demux);
        mdw_player_get_max_size(priv->player, &w, &h, NULL);
        priv->max_w = w;
        priv->max_h = h;
    }
    
    return TRUE;
}

/***********************/
/* AUXILIARY FUNCTIONS */
/***********************/
