/***************************************************************************
 *  MdwDvbTuner
 *  Mon May 21 08:53:24 2007 
 *
 *  Copyright (c) 2007 iuri,,,
 *  Authors: iuri,,, <iuri@email.com.br> 2007
 * 
 *  VERSION CONTROL INFORMATION: $Id$
 *
****************************************************************************/

#include "mdw_dvb_tuner.h"

/********************/
/* local prototypes */
/********************/
/* interface overrided methods */
//static void _iface_namespace_my_interface_init (gpointer g_iface, gpointer data);
//static gboolean _iface_namespace_my_interface_method1 (NamespaceMyInterface *iface, guint param1);

/* destructors */
static void mdw_dvb_tuner_dispose (GObject *object);
static void mdw_dvb_tuner_finalize (GObject *object);

/* properties get/set functions*/
/*
static void mdw_dvb_tuner_set_property(GObject *object,
    guint property_id,
    const GValue *value,
    GParamSpec *pspec);
static void mdw_dvb_tuner_get_property(GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec);
*/
/* enum for enumerate properties */
/*
enum {
    MDW_DVB_TUNER_PROPERTY_1 = 1,
    MDW_DVB_TUNER_PROPERTY_2
};
*/
/* ERROR */
/*
GQuark 
mdw_dvb_tuner_error_quark() 
{
    static GQuark quark = 0;
    if (G_UNLIKELY(quark == 0)) {
        quark = g_quark_from_static_string("MdwDvbTunerError");
    }
    return quark;
}
#define MDW_DVB_TUNER_RAISE_ERROR(error, type, ...) \
{ \
    g_set_error((error), MDW_DVB_TUNER_ERROR, (type), __VA_ARGS__);\
    return FALSE; \
} while (0)

#define MDW_DVB_TUNER_RAISE_ERROR_IF_FAIL(error, cond, type, ...) \
{ \
    if (G_UNLIKELY(!(cond))) { \
         MDW_DVB_TUNER_RAISE_ERROR((error), (type), ...);\
    } \
} while (0)

// MDW_DVB_TUNER_ERROR_TYPE_1 
#define MDW_DVB_TUNER_RAISE_ERROR_TYPE_1_IF_FAIL(error, cond, ...) \
    MDW_DVB_TUNER_RAISE_ERROR_IF_FAILED((error), (cond), MDW_DVB_TUNER_ERROR_TYPE_1, __VA_ARGS__)
#define MDW_DVB_TUNER_RAISE_ERROR_TYPE_1(error, cond, ...) \
    MDW_DVB_TUNER_RAISE_ERROR((error), MDW_DVB_TUNER_ERROR_TYPE_1, __VA_ARGS__)

// MDW_DVB_TUNER_ERROR_TYPE_2
#define MDW_DVB_TUNER_RAISE_ERROR_TYPE_2_IF_FAIL(error, cond, ...) \
    MDW_DVB_TUNER_RAISE_ERROR_IF_FAIL((error), (cond), MDW_DVB_TUNER_ERROR_TYPE_2, __VA_ARGS__)
#define MDW_DVB_TUNER_RAISE_ERROR_TYPE_2(error, ...) \
    MDW_DVB_TUNER_RAISE_ERROR((error), MDW_DVB_TUNER_ERROR_TYPE_2, __VA_ARGS__)

// MDW_DVB_TUNER_ERROR_FAILED (General Failure)
#define MDW_DVB_TUNER_RAISE_ERROR_FAILED_IF_FAIL(error, cond, ...) \
    MDW_DVB_TUNER_RAISE_ERROR_IF_FAIL((error), (cond), MDW_DVB_TUNER_ERROR_FAILED, __VA_ARGS__)
#define MDW_DVB_TUNER_RAISE_ERROR_FAILED(error, cond, ...) \
    MDW_DVB_TUNER_RAISE_ERROR((error), MDW_DVB_TUNER_ERROR_FAILED, __VA_ARGS__)

// MDW_DVB_TUNER_ERROR_NOT_IMPLEMENTED (Not Implemented)
#define MDW_DVB_TUNER_RAISE_ERROR_NOT_IMPLEMENTED(error) \
    MDW_DVB_TUNER_RAISE_ERROR((error), MDW_DVB_TUNER_ERROR_NOT_IMPLEMENTED, "Not implemented: %s", __FUNC__)
*/

/*********** PRIVATE *************/
/* private field */
/*

#define MDW_DVB_TUNER_GET_PRIVATE(o)  \
   (G_TYPE_INSTANCE_GET_PRIVATE ((o), MDW_TYPE_DVB_TUNER, MdwDvbTunerPrivate))

typedef struct _MdwDvbTunerPrivate MdwDvbTunerPrivate;
struct _MdwDvbTunerPrivate {
    int private_field;
};
*/

/* private methods */

/* auxiliary functions */


/***********************************************/
/* GObject obligatory definitions/declarations */
/***********************************************/

/* define my Object */
/*
G_DEFINE_TYPE(
    MdwDvbTuner, //my name in Camel case.
    mdw_dvb_tuner, // my name in lowercase, with words separated by '_'.
    G_TYPE_OBJECT //GType of my parent 
)
*/

/* define my Object (with interface)*/
/*
G_DEFINE_TYPE_WITH_CODE(
    MdwDvbTuner, //my name in Camel case.
    mdw_dvb_tuner, // my name in lowercase, with words separated by '_'.
    G_TYPE_OBJECT, //GType of my parent 
    G_IMPLEMENT_INTERFACE(NAMESPACE_TYPE_MY_INTERFACE, //interfaces that I implement
        _iface_namespace_my_interface_init)
)
*/

/* Init: define my Object (explict) */
static void     mdw_dvb_tuner_init       (MdwDvbTuner      *self);
static void     mdw_dvb_tuner_class_init (MdwDvbTunerClass *klass);
static gpointer mdw_dvb_tuner_parent_class = NULL;
static void     mdw_dvb_tuner_class_intern_init (gpointer klass)
{
  mdw_dvb_tuner_parent_class = g_type_class_peek_parent (klass);
  mdw_dvb_tuner_class_init ((MdwDvbTunerClass*) klass);
}

GType
mdw_dvb_tuner_get_type (void)
{
    static GType g_define_type_id = 0; 
    if (G_UNLIKELY (g_define_type_id == 0)) 
    { 
        static const GTypeInfo g_define_type_info = { 
            sizeof (MdwDvbTunerClass), 
            (GBaseInitFunc) NULL, 
            (GBaseFinalizeFunc) NULL, 
            (GClassInitFunc) mdw_dvb_tuner_class_intern_init, 
            (GClassFinalizeFunc) NULL, 
            NULL,   /* class_data */ 
            sizeof (MdwDvbTuner), 
            0,      /* n_preallocs */ 
            (GInstanceInitFunc) mdw_dvb_tuner_init, 
        }; 
        g_define_type_id = g_type_register_static (
            G_TYPE_OBJECT, //parent GType 
            "MdwDvbTuner", //my name in Camel case
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
mdw_dvb_tuner_class_init (MdwDvbTunerClass *klass) 
{
    /* top ancestral */
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    /* private alocation */
    //g_type_class_add_private (klass, sizeof (MdwDvbTunerPrivate));

    /* methods */
    //klass->method1 = mdw_dvb_tuner_method1;
    //klass->method2 = mdw_dvb_tuner_method2;

    /* destructors */
    gobject_class->dispose = mdw_dvb_tuner_dispose;
    gobject_class->finalize = mdw_dvb_tuner_finalize;

    /* properties */
    //GParamSpec *pspec;
    //gobject_class->set_property = mdw_dvb_tuner_set_property;
    //gobject_class->get_property = mdw_dvb_tuner_get_property;
    
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
        MDW_DVB_TUNER_PROPERTY_1,  //PROPERTY ID
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
        MDW_DVB_TUNER_PROPERTY_2,  //PROPERTY ID
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
mdw_dvb_tuner_init(MdwDvbTuner *self) 
{
    /* set the private members */
    //self->boolean_member = TRUE;
    //self->uint8_member = 42;

    //MdwDvbTunerPrivate *priv = MDW_DVB_TUNER_GET_PRIVATE(self);
}

/***************/
/* Destructors */
/***************/
static void    
mdw_dvb_tuner_dispose (GObject *object) 
{   
    //MdwDvbTuner *self =  MDW_DVB_TUNER(object);
    //MdwDvbTunerPrivate *priv = MDW_DVB_TUNER_GET_PRIVATE(self);
    /* 
    if (self->dispose_has_run) {
        return;
    }
    */

    /* chaining up */
    G_OBJECT_CLASS(mdw_dvb_tuner_parent_class)->dispose(object);
}
static void    
mdw_dvb_tuner_finalize (GObject *object) 
{
    //MdwDvbTuner *self =  MDW_DVB_TUNER(object);
    //MdwDvbTunerPrivate *priv = MDW_DVB_TUNER_GET_PRIVATE(self);

    /* chaining up */
    G_OBJECT_CLASS(mdw_dvb_tuner_parent_class)->finalize(object);
}

/**********************/
/* PROPERTIES GET/SET */
/**********************/
/**
 * Properties set function
 */
/*
static void 
mdw_dvb_tuner_set_property(GObject *object,
    guint property_id,
    const GValue *value,
    GParamSpec *pspec) 
{
    MdwDvbTuner *self = MDW_DVB_TUNER(object);
    //MdwDvbTunerPrivate *priv = MDW_DVB_TUNER_GET_PRIVATE(self);
    switch (property_id) {
        case MDW_DVB_TUNER_PROPERTY_1:
            g_free(self->property1);
            self->property1 = g_value_dup_string(value);
            break;
        case MDW_DVB_TUNER_PROPERTY_2:
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
mdw_dvb_tuner_get_property(GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec) 
{
    MdwDvbTuner *self = MDW_DVB_TUNER(object);
    //MdwDvbTunerPrivate *priv = MDW_DVB_TUNER_GET_PRIVATE(self);
    switch (property_id) {
        case MDW_DVB_TUNER_PROPERTY_1:
            g_value_set_string(value, self->property1);
            break;
        case MDW_DVB_TUNER_PROPERTY_2:
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

/*
MdwDvbTuner* 
mdw_dvb_tuner_new() {
    return (MdwDvbTuner*) g_object_new(MDW_TYPE_DVB_TUNER, NULL);
}
*/

/*
MdwDvbTuner* 
mdw_dvb_tuner_new_with_arg1(guint8 param1) {
    return (MdwDvbTuner*) g_object_new(MDW_TYPE_DVB_TUNER, "property 1", param1, NULL);
}
*/


/**********************/
/* Overrided methods  */
/**********************/
/*
static gboolean _iface_namespace_my_interface_method1 (NamespaceMyInterface *iface, guint param1)
{
    MdwDvbTuner *self = MDW_DVB_TUNER(iface);
    //
    return TRUE;
}
*/

/******************/
/* PUBLIC METHODS */
/******************/

/*******************/
/* PRIVATE METHODS */
/*******************/

/***********************/
/* AUXILIARY FUNCTIONS */
/***********************/
