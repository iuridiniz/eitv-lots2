/*@COPYRIGHT*/
#ifndef MDW_TS_SOURCE_H
#define MDW_TS_SOURCE_H


#include <glib-object.h>

G_BEGIN_DECLS

/* preambule init */
#define MDW_TYPE_TS_SOURCE   (mdw_ts_source_get_type())
#define MDW_TS_SOURCE(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj), MDW_TYPE_TS_SOURCE, MdwTsSource))
#define MDW_IS_TS_SOURCE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), MDW_TYPE_TS_SOURCE))
#define MDW_TS_SOURCE_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), MDW_TYPE_TS_SOURCE, MdwTsSourceInterface))
/* preambule end */

typedef struct _MdwTsSource  MdwTsSource; /* dummy */
typedef struct _MdwTsSourceInterface  MdwTsSourceInterface;


/**
 * Source interface for get data.
 *
 */
struct _MdwTsSourceInterface {
    GTypeInterface parent; /** obligatory declaration */
    
    /* interface methods */
    gssize (*read) (MdwTsSource *self, guint8 *buf, gssize count);
};


/** obligatory declaration */
GType mdw_ts_source_get_type (void);

/* methods declarations */
/** 
 * Attempts to get count bytes from self
 * @param self MdwTsSource inst.
 * @param buffer Destination container.
 * @param count Number of bytes to get.
 * @returns Number of bytes in buffer, 0 if source is over and -1 if error.
 */
gssize mdw_ts_source_read(MdwTsSource *self, guint8 *buffer, const gssize count);

G_END_DECLS


#endif /* MDW_TS_SOURCE_H */
