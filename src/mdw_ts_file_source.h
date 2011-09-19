/*@COPYRIGHT*/
#ifndef MDW_TS_FILE_SOURCE_H
#define MDW_TS_FILE_SOURCE_H


#include <glib-object.h>
#include "mdw_ts_source.h"


G_BEGIN_DECLS

/* preambule init */
#define MDW_TYPE_TS_FILE_SOURCE   (mdw_ts_file_source_get_type())
#define MDW_TS_FILE_SOURCE(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj), MDW_TYPE_TS_FILE_SOURCE, MdwTsFileSource))
#define MDW_TS_FILE_SOURCE_CLASS(vtable)   (G_TYPE_CHECK_CLASS_CAST((vtable), MDW_TYPE_TS_FILE_SOURCE, MdwTsFileSourceClass))
#define MDW_IS_TS_FILE_SOURCE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), MDW_TYPE_TS_FILE_SOURCE))
#define MDW_IS_TS_FILE_SOURCE_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE((vtable), MDW_TYPE_TS_FILE_SOURCE))

#define MDW_TS_FILE_SOURCE_GET_CLASS(inst) (G_TYPE_INSTANCE_GET_CLASS((inst), MDW_TYPE_TS_FILE_SOURCE, MdwTsFileSourceClass))



/* preambule end */

typedef struct _MdwTsFileSource MdwTsFileSource;
typedef struct _MdwTsFileSourceClass MdwTsFileSourceClass;


/**
 * DVB demux instance.
 */
struct _MdwTsFileSource {
    GObject parent; /**< obligatory declaration */
    
    /* private members */
    gchar* filename; /**< filename */
    gint   fd; /**< file descriptor associated with filename */
    gboolean continuous; /**< defines the read behavior */
};


/**
 * DVB demux class.
 */
struct _MdwTsFileSourceClass {
    GObjectClass parent; /**< obligatory declaration */
    
    /* public methods */
    /* void */
};

/** 
 * (obligatory declaration) Get GType id associated with MdwTsFileSource.
 */

GType mdw_ts_file_source_get_type(void);

/* Constructors */

/**
 * Constructs a new MdwTsFileSource.
 * @return returns a MdwTsSource Interface.
 */
MdwTsFileSource* mdw_ts_file_source_new();

/**
 * Constructs a new MdwTsFileSource.
 * @param location filename to use.
 * @return returns a MdwTsSource Interface.
 */

MdwTsFileSource* mdw_ts_file_source_new_with_location(const gchar* location);

/* public methods */
/* void */

G_END_DECLS


#endif /* MDW_TS_FILE_SOURCE_H */
