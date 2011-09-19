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

#ifndef SEC_FILTER_H
#define SEC_FILTER_H

#include <glib-object.h>

G_BEGIN_DECLS

/* preambule init */
#define SEC_TYPE_FILTER   (sec_filter_get_type())
#define SEC_FILTER(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj), SEC_TYPE_FILTER, SecFilter))
#define SEC_FILTER_CLASS(vtable)   (G_TYPE_CHECK_CLASS_CAST((vtable), SEC_TYPE_FILTER, SecFilterClass))
#define SEC_IS_FILTER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), SEC_TYPE_FILTER))
#define SEC_IS_FILTER_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE((vtable), SEC_TYPE_FILTER))
#define SEC_FILTER_GET_CLASS(inst) (G_TYPE_INSTANCE_GET_CLASS((inst), SEC_TYPE_FILTER, SecFilterClass))
/* preambule end */

/* class types */
typedef struct _SecFilter SecFilter;
typedef struct _SecFilterClass SecFilterClass;

/**
 *  A Filter instance.
 */
struct _SecFilter {
    GObject parent; /**< pointer to parent (obligatory declaration) */
    
    /* private members */
    //gboolean boolean_member; /**< a boolean_member */
    //guint8 uint8_member; /**< a uint8_member */
    
};

/**
 * A Filter class.
 */
struct _SecFilterClass {
    GObjectClass parent; /**< pointer to parent (obligatory declaration) */
    
    /* public methods */
    guint (*add_filter)(SecFilter* self, guint16 pid, gint32 table_id, gint32 table_id_ext, void(*callback)(const GByteArray* __data, void* __user_data), void* user_data); /**< @see sec_filter_add_filter */

    gboolean (*remove)(SecFilter* self, guint id); /**< @see sec_filter_remove */
};

/** 
 * Get GType id associated with SecFilter (obligatory declaration).
 * @return Returns the GType id associated with SecFilter.
 */

GType sec_filter_get_type(void);

/****************/
/* Constructors */
/****************/

/**
 * Constructs a new SecFilter.
 * @return Returns a new SecFilter.
 */
SecFilter* sec_filter_new();

/***************/
/* Destructors */
/***************/
/**
 * Unref the SecFilter.
 */
void sec_filter_unref(SecFilter* f);

/*******************/
/* public methods */
/*******************/

/** 
 * Adds a filter
 * @param self A SecFilter instance.
 * @param pid The pid to be filtered.
 * @param table_id if you don't need a filter here, use -1, otherwise a value >= 0.
 * @param table_id_ext if you don't need a filter here, use -1, otherwise a value >= 0.
 * @param callback callback function.
 * @param user_data callback data.
 * @return Returns the id (> 0) of the filter.
 */
guint sec_filter_add_filter(
    SecFilter* self, 
    guint16 pid, gint32 table_id, gint32 table_id_ext, 
    void(*callback)(const GByteArray* __data, void* __user_data),
    void* user_data
);

/** 
 * Remove a filter
 * @param self A SecFilter instance.
 * @param id the id to remove.
 * @return Returns true on success, otherwise false.
 */
gboolean sec_filter_remove(SecFilter* self, guint id);

G_END_DECLS

/* Error */
/*
#define SEC_FILTER_ERROR sec_filter_error_quark()
typedef enum
{
  SEC_FILTER_ERROR_TYPE_1,
  SEC_FILTER_ERROR_TYPE_2,
  SEC_FILTER_ERROR_NOT_IMPLEMENTED,
  SEC_FILTER_ERROR_FAILED
} SecFilterError;

GQuark sec_filter_error_quark();
*/


#endif /* SEC_FILTER_H */

