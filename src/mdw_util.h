/***************************************************************************
 *            demux_common.h
 *
 *  Thu Mar 29 22:47:02 UTC 2007 
 *  Copyright 2007 Iuri Gomes Diniz
 *  $Id$
 *  <iuridiniz@eitv.com.br>
****************************************************************************/

#ifndef _DEMUX_COMMON_H
#define _DEMUX_COMMON_H

#include <glib.h> 

#include "string.h"

#include "config.h"

#include "mdw_demux.h"
#include "mdw_player.h"
#include "mdw_ts_source.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* TODO: use a class for debug */ 

#ifdef MDW_DEBUG_LEVEL
#define DEBUG_MSG(type, ...) \
    { \
        const int n = 1024; \
        char msg_buffer[1024]; \
        g_snprintf(msg_buffer, n,"%s [%s:%d] ", (type), __FILE__, __LINE__ );\
        g_snprintf(msg_buffer+strlen(msg_buffer), n - strlen(msg_buffer), __VA_ARGS__); \
        g_debug(msg_buffer); \
    } while (0)
#else 
#define MDW_DEBUG_LEVEL 0
#define DEBUG_MSG(...) do {} while (0);
#endif /* MDW_DEBUG_LEVEL */

#if MDW_DEBUG_LEVEL > 0
#define DEBUG_ERROR(...) DEBUG_MSG("ERROR", __VA_ARGS__)
#else
#define DEBUG_ERROR(...) do {} while(0)
#endif /* MDW_DEBUG_LEVEL > 0 */

#if MDW_DEBUG_LEVEL > 1
#define DEBUG_WARN(...) DEBUG_MSG("WARNING", __VA_ARGS__)
#else
#define DEBUG_WARN(...) do {} while(0)
#endif /* MDW_DEBUG_LEVEL > 1 */

#if MDW_DEBUG_LEVEL > 2
#define DEBUG_INFO(...) DEBUG_MSG("INFO", __VA_ARGS__)
#else
#define DEBUG_INFO(...) do {} while (0)
#endif /* MDW_DEBUG_LEVEL > 2 */

#if MDW_DEBUG_LEVEL > 3
#define DEBUG_INFO_DETAILED(...) DEBUG_MSG("DETAILS", __VA_ARGS__)
#else
#define DEBUG_INFO_DETAILED(...) do {} while (0)
#endif /* MDW_DEBUG_LEVEL > 3 */

#ifdef MDW_DEBUG_MUTEX
#define LOCK(x) \
    DEBUG_MSG("Mutex", "locking %s (%p)", #x, (void*) x); \
    g_mutex_lock((x)); \
    DEBUG_MSG("Mutex", "acquired lock %s (%p)", #x, (void*) x); \
    sched_yield()
#define UNLOCK(x) \
    DEBUG_MSG("Mutex", "unlocking %s (%p)", #x, (void*) x); \
    g_mutex_unlock((x))
#else /* MDW_DEMUG_MUTEX */
#define LOCK(x) \
    g_mutex_lock((x))
#define UNLOCK(x) \
    g_mutex_unlock((x))
#endif /* MDW_DEBUG_MUTEX */

gboolean mdw_guint16_equal (gconstpointer a, gconstpointer b);
guint mdw_guint16_hash (gconstpointer a);
gboolean mdw_verify_uri(const gchar* uri); 

MdwDemux* mdw_get_most_suitable_demux();
MdwPlayer* mdw_get_most_suitable_player();
MdwTsSource* mdw_get_most_suitable_ts_source(const gchar* uri);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _DEMUX_COMMON_H */
