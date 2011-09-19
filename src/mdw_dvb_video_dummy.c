/***************************************************************************
 *  MdwDvbVideoDummy
 *  Tue Mar 20 16:15:05 2007 
 *
 *  Copyright (c) 2007 Iuri Diniz,,,
 *  Authors: Iuri Diniz,,, <iuri@email.com.br> 2007
 * 
 *  VERSION CONTROL INFORMATION: $Id$
 *
****************************************************************************/

#include "mdw_dvb_video_dummy.h"
#include "mdw_demux_data_handler.h"

/********************/
/* local prototypes */
/********************/
/* interface overrided methods */
static void _iface_mdw_demux_data_handler_init (gpointer g_iface, gpointer data);
static gboolean _iface_mdw_demux_data_handler_new_data (MdwDemuxDataHandler *self, 
        const guint16 pid, const gsize size, const guint8* data);

/***********************************************/
/* GObject obrigatory definations/declarations */
/***********************************************/

/* define my Object (with interface)*/
G_DEFINE_TYPE_WITH_CODE(
    MdwDvbVideoDummy, //my name in Camel case.
    mdw_dvb_video_dummy, // my name in lowercase, with words separated by '_'.
    G_TYPE_OBJECT, //GType of my parent 
    G_IMPLEMENT_INTERFACE(MDW_TYPE_DEMUX_DATA_HANDLER,
        _iface_mdw_demux_data_handler_init)
)


/*************************/
/* Intenal Constructors */
/************************/
/**
 * initilize/override parent class methods, create signals, properties.
 */
static void
mdw_dvb_video_dummy_class_init (MdwDvbVideoDummyClass *klass) 
{
}

/**
 * GObject Internal Constructor 
 */
static void
mdw_dvb_video_dummy_init(MdwDvbVideoDummy *self) 
{
}

/** 
 * Overrides methods of NamespaceMyInterface 
 */
static void 
_iface_mdw_demux_data_handler_init (gpointer g_iface, gpointer data) 
{
    ((MdwDemuxDataHandlerInterface *) g_iface)->new_data = 
        _iface_mdw_demux_data_handler_new_data; //override new_data
}


/************************/
/* Public Constructors */
/***********************/

MdwDvbVideoDummy* 
mdw_dvb_video_dummy_new() {
    return (MdwDvbVideoDummy*) g_object_new(MDW_TYPE_DVB_VIDEO_DUMMY, NULL);
}

/**********************/
/* Overrided methods  */
/**********************/
static gboolean _iface_mdw_demux_data_handler_new_data (MdwDemuxDataHandler *self, 
        const guint16 pid, const gsize size, const guint8* data) 
{
    return TRUE;
}

/******************/
/* PUBLIC METHODS */
/******************/
