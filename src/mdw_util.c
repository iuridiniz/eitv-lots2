/***************************************************************************
 *            mdw_util.c
 *
 *  Thu Mar 29 22:46:58 UTC 2007 
 *  Copyright 2007 Iuri Gomes Diniz
 *  $Id$
 *  <iuridiniz@eitv.com.br>
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "mdw_util.h"
#include "mdw_default_demux.h"
#include "mdw_default_player.h"
#include "mdw_ts_file_source.h"

gboolean 
mdw_guint16_equal (gconstpointer a, gconstpointer b) 
{
    DEBUG_INFO_DETAILED("Comparing %u and %u", *((guint16*)a), *((guint16*)b)); 
    return *((const guint16*) a) == *((const guint16*)b);
}

guint 
mdw_guint16_hash (gconstpointer a) 
{
    DEBUG_INFO_DETAILED("Computing the hash value of: %u", *((guint16*)a)); 
    return *((guint16*) a) % 32;
}

gboolean
mdw_verify_uri(const gchar* uri) {
    g_return_val_if_fail(uri != NULL, FALSE);
    DEBUG_INFO("Verifing is the given uri (%s) is valid", uri);
    if (strncmp("file://", uri, 7) == 0) {
        return TRUE;
    }
    DEBUG_INFO("Invalid uri: %s", uri);
    return FALSE;
}

MdwDemux* 
mdw_get_most_suitable_demux() 
{
    #ifdef FOR_PC
    return MDW_DEMUX(mdw_default_demux_new());
    #endif
    #ifdef FOR_STB810
    return MDW_DEMUX(mdw_dvb_demux_new());
    #endif
    /* DEFAULT */
    return MDW_DEMUX(mdw_default_demux_new());
}

/*
MdwDataHandler*
mdw_get_most_suitable_mpeg_decoder() {
    return NULL;
}
MdwDataHanlder*
mdw_get_most_suitable_video_buffer() {
    return NULL;
}
*/

MdwPlayer* 
mdw_get_most_suitable_player() 
{
    #ifdef FOR_PC
    return MDW_PLAYER(mdw_default_player_new());
    #endif
    #ifdef FOR_STB810
    return MDW_PLAYER(mdw_stb810_player_new());
    #endif

    /* DEFAULT */
    return MDW_PLAYER(mdw_default_player_new());
}

MdwTsSource* 
mdw_get_most_suitable_ts_source(const gchar* uri) 
{
    g_return_val_if_fail(uri != NULL, NULL);
    g_return_val_if_fail(mdw_verify_uri(uri) == TRUE, NULL);

    if (strncmp("file://", uri, 7) == 0) {
        MdwTsSource *s = MDW_TS_SOURCE(mdw_ts_file_source_new_with_location(&uri[7]));
        g_object_set(G_OBJECT(s), "continuous", TRUE, NULL);
        return s;
    } 
    DEBUG_ERROR("Invalid uri: %s", uri);
    return NULL;
}

