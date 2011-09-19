/*@COPYRIGHT@*/

#ifndef MDW_SECTION_DUMPER_H
#define MDW_SECTION_DUMPER_H


#include <glib-object.h>
#include "mdw_demux_data_handler.h"


G_BEGIN_DECLS

/* preambule init */
#define MDW_TYPE_SECTION_DUMPER   (mdw_section_dumper_get_type())
#define MDW_SECTION_DUMPER(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj), MDW_TYPE_SECTION_DUMPER, MdwSectionDumper))
#define MDW_SECTION_DUMPER_CLASS(vtable)   (G_TYPE_CHECK_CLASS_CAST((vtable), MDW_TYPE_SECTION_DUMPER, MdwSectionDumperClass))
#define MDW_IS_SECTION_DUMPER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), MDW_TYPE_SECTION_DUMPER))
#define MDW_IS_SECTION_DUMPER_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE((vtable), MDW_TYPE_SECTION_DUMPER))

#define MDW_SECTION_DUMPER_GET_CLASS(inst) (G_TYPE_INSTANCE_GET_CLASS((inst), MDW_TYPE_SECTION_DUMPER, MdwSectionDumperClass))


/* preambule end */

typedef struct _MdwSectionDumper MdwSectionDumper;
typedef struct _MdwSectionDumperClass MdwSectionDumperClass;


/**
 *  Dump section instance.
 */
struct _MdwSectionDumper {
    GObject parent; /**< obligatory declaration */
    
    /* private members */
};


/**
 * Dump section class.
 */
struct _MdwSectionDumperClass {
    GObjectClass parent; /**< obligatory declaration */
    
    /* public methods */
    /* void */
};

/** 
 * (obligatory declaration) Get GType id associated with MdwSectionDumper.
 */

GType mdw_section_dumper_get_type(void);

/* Constructors */

/**
 * Constructs a new MdwSectionDumper.
 * @return Returns a new MdwSectionDumper .
 */
MdwSectionDumper* mdw_section_dumper_new();

/* public methods */
/* void */

G_END_DECLS


#endif /* MDW_SECTION_DUMPER_H */
