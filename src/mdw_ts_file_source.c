/*@COPYRIGHT@*/
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "mdw_ts_file_source.h"

/********************/
/* local prototypes */
/********************/
static void _iface_mdw_ts_source_init (gpointer g_iface, gpointer data); 

/* overrided method of interface */
static gssize _iface_mdw_ts_source_read(MdwTsSource *self, guint8 *buffer, const gssize count);


/* properties get/set functions */
static void mdw_ts_file_source_set_property(GObject *object,
    guint property_id,
    const GValue *value,
    GParamSpec *pspec);

static void mdw_ts_file_source_get_property(GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec);

static void mdw_ts_file_source_open_file(MdwTsFileSource *self, const GValue *value);

static void mdw_ts_file_source_close_file(MdwTsFileSource *self);
/**
 * properties enumeration 
 */
enum {
    MDW_TS_FILE_SOURCE_LOCATION = 1,
    MDW_TS_FILE_SOURCE_CONTINUOUS,

};

/* destructors */
static void mdw_ts_file_source_dispose(GObject *object);
static void mdw_ts_file_source_finalize(GObject *object);



/* define my Object */
G_DEFINE_TYPE_WITH_CODE(
    MdwTsFileSource,
    mdw_ts_file_source,
    G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE(MDW_TYPE_TS_SOURCE,
        _iface_mdw_ts_source_init)
)



/******************/

/**
 * initiliaze interfaces that I implement 
 */
static void 
_iface_mdw_ts_source_init (gpointer g_iface, gpointer data) {
    /* read */
    ((MdwTsSourceInterface *) g_iface)->read = _iface_mdw_ts_source_read;
}



/**
 * initilize/override parent class methods, create signals, properties 
 */
static void 
mdw_ts_file_source_class_init (MdwTsFileSourceClass *g_class) 
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
    //MdwTsFileSourceClass *klass = MDW_TS_FILE_SOURCE_CLASS(g_class);
    GParamSpec *pspec;

    /* override parent class methods related to properties */
    gobject_class->set_property = mdw_ts_file_source_set_property;
    gobject_class->get_property = mdw_ts_file_source_get_property;

    /* property: location */
    pspec = g_param_spec_string (
        "location", //name
        "File location", //nick
        "Set/Get the source file location", //description
        NULL, //default value
        G_PARAM_READWRITE //flags
    );
    g_object_class_install_property (gobject_class, 
        MDW_TS_FILE_SOURCE_LOCATION, pspec);
    g_param_spec_unref(pspec);

    pspec = g_param_spec_boolean (
        "continuous", //name
        "Is read continuous?", //nick
        "Set/Get the behavior of read", //description
        FALSE, //defaul value 
        G_PARAM_READWRITE //flags
    );

    g_object_class_install_property (gobject_class, 
        MDW_TS_FILE_SOURCE_CONTINUOUS, pspec);

    g_param_spec_unref(pspec);
    /* end of properties */

    /* override destructors */

    gobject_class->dispose = mdw_ts_file_source_dispose;
    gobject_class->finalize = mdw_ts_file_source_finalize;

}
/** 
 * set property function.
 */
static void 
mdw_ts_file_source_set_property(
    GObject *object,
    guint property_id,
    const GValue *value,
    GParamSpec *pspec) 
{
    MdwTsFileSource *self = MDW_TS_FILE_SOURCE(object);
    switch (property_id) {
        case MDW_TS_FILE_SOURCE_LOCATION:
            mdw_ts_file_source_close_file(self);
            //g_free(self->filename);
            mdw_ts_file_source_open_file(self, value);
            //self->filename = g_value_dup_string(value);
            break;
        case MDW_TS_FILE_SOURCE_CONTINUOUS:
            self->continuous = g_value_get_boolean(value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id,pspec);
            break;
    }
}

/** 
 * get property function.
 */
static void 
mdw_ts_file_source_get_property(
    GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec)
{
    MdwTsFileSource *self = MDW_TS_FILE_SOURCE(object);
    switch (property_id) {
        case MDW_TS_FILE_SOURCE_LOCATION:
            g_value_set_string(value, self->filename);
            break;
        case MDW_TS_FILE_SOURCE_CONTINUOUS:
            g_value_set_boolean(value, self->continuous);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id,pspec);
            break;
    }

}

/**
 * open file in self->fd and alloc self->filename for store the filename
 * @todo FIXME: not thread safe.
 */
static void 
mdw_ts_file_source_open_file(MdwTsFileSource *self, const GValue *value) 
{
    int fd;
    if (G_UNLIKELY(self->fd != -1)) {
        /** @todo TODO: write this attempt to log*/
        //g_printerr("self->fd == -1\n");
        return;
    }

    self->filename = g_value_dup_string(value);
    if(G_UNLIKELY(self->filename == NULL)) { 
        /** @todo TODO: write this attemp to log*/
        //g_printerr("self->filename == NULL\n");
        return;
    }
    if(G_UNLIKELY((fd = open(self->filename, O_RDONLY)) < 0)) {
        /** @todo TODO: write this fail to log*/
        //g_printerr("'%s' not opened\n", self->filename);
        return;  
    } 

    self->fd = fd;

}

/**
 * close file and dealloc self->filename 
 * @todo FIXME: not thread safe.
 */
static void 
mdw_ts_file_source_close_file(MdwTsFileSource *self) 
{
    if(G_UNLIKELY(self->fd == -1)) {
        /** @todo TODO: write this attempt to log*/
        return;
    }
    
    close(self->fd);
    self->fd = -1;
    g_free(self->filename);
    self->filename = NULL;
}


/**
 * Instance constructor 
 */
static void 
mdw_ts_file_source_init (MdwTsFileSource *self) 
{
    self->fd = -1;
    self->filename = NULL;
    self->continuous = FALSE;
}

/* destructors */
/**
 * Clean the references.
 */
static void 
mdw_ts_file_source_dispose(GObject *object) 
{
    G_OBJECT_CLASS(mdw_ts_file_source_parent_class)->dispose(object);
}

/**
 * Close files and dealloc memory.
 */
static void 
mdw_ts_file_source_finalize(GObject *object) 
{
    mdw_ts_file_source_close_file(MDW_TS_FILE_SOURCE(object));
    G_OBJECT_CLASS(mdw_ts_file_source_parent_class)->finalize(object);
}

/* methods overrided */

static gssize 
_iface_mdw_ts_source_read(MdwTsSource *inst, guint8 *buffer, const gssize count) 
{   
    int ret;
    int remain, written;

    MdwTsFileSource* self = MDW_TS_FILE_SOURCE(inst);
    if (G_UNLIKELY(self->fd == -1)) { 
        return -1;
    }
    if (G_UNLIKELY((ret = read(self->fd, buffer, count)) == -1)) {
        return -1;
    }
    written = ret;
    remain = count - written;
    
    while (self->continuous == TRUE && remain > 0 ) {
        if (ret == 0) { 
            if(G_UNLIKELY(lseek(self->fd, 0, SEEK_SET)) == -1) {
                return -1;
            }
        }
        if (G_UNLIKELY((ret = read(self->fd, buffer+written, remain)) == -1)) {
            return -1;
        }
        written += ret;
        remain = count - written;
    }
    return written;

}

/* public methods */
/* Contructor without location */
MdwTsFileSource* 
mdw_ts_file_source_new() 
{
    return (MdwTsFileSource*) g_object_new(MDW_TYPE_TS_FILE_SOURCE, NULL);
}


/* Contructor with location */
MdwTsFileSource* 
mdw_ts_file_source_new_with_location(const gchar* location) 
{
    return (MdwTsFileSource*) g_object_new(MDW_TYPE_TS_FILE_SOURCE, "location", location, NULL);
}


