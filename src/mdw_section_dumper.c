/*@COPYRIGHT@*/

#include <glib/gprintf.h>
#include "mdw_section_dumper.h"

/********************/
/* local prototypes */
/********************/
/* interface methods */
static void _iface_mdw_demux_data_handler_init (gpointer g_iface, gpointer data);
static gboolean _iface_mdw_demux_data_handler_new_data (
    MdwDemuxDataHandler *self, 
    const guint16 pid, 
    const gsize size, 
    const guint8* data);

/* define my Object */
G_DEFINE_TYPE_WITH_CODE(
    MdwSectionDumper,
    mdw_section_dumper,
    G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE(MDW_TYPE_DEMUX_DATA_HANDLER,
        _iface_mdw_demux_data_handler_init)
)



/** 
 * Overrides methods of MdwDemuxDataHandlerInterface 
 */
static void 
_iface_mdw_demux_data_handler_init (gpointer g_iface, gpointer data) 
{
    //new_data
    ((MdwDemuxDataHandlerInterface *) g_iface)->new_data = _iface_mdw_demux_data_handler_new_data;

}

static const gchar* table_id_to_name(guint8 id) {
    static gboolean table_inited = FALSE;
    static gchar* table[256];
    if (table_inited == FALSE) {
        table[0x00] = "Program Association Section";
        table[0x01] = "Conditonal Access Section";
        table[0x02] = "Programa Map Section";
        table[0x03] = "Transport Stream Description Section";
        table[0x04] = "Reserved";
        #define RESERVED table[0x04]
        for(int i = 0x05; i <= 0x3F; i++) {
                table[i] = RESERVED;
        }
        table[0x40] = "Network Information Section - Actual Network";
        table[0x41] = "Network Information Section - Other Network";
        table[0x42] = "Service Description Section - Actual Transport Stream";
        table[0x43] = "Reserved for future use";
        #define RESERVED_FOR_FUTURE table[0x43]
        table[0x44] = table[0x45] = RESERVED_FOR_FUTURE;

        table[0x46] = "Service Description Section - Other Transport Stream";
        table[0x47] = table[0x48] = table[0x48] = RESERVED_FOR_FUTURE;
        table[0x4A] = "Bouquet Association Section";
        table[0x4B] = table[0x4C] = table[0x4D] = RESERVED_FOR_FUTURE;

        table[0x4E] = "Event Information Section - Actual Transport Stream, Preset/Following";
        table[0x4F] = "Event Information Section - Other Transport Stream, Preset/Following";
        table[0x50] = "Event Information Section - Actual Transport Stream, Schedule";
        #define EIT_ACTUAL_SCHD table[0x50]
        for(int i = 0x51; i <= 0x5F; i++) {
            table[i] = EIT_ACTUAL_SCHD;
        }
        table[0x60] = "Event Information Section - Other Transport Stream, Schedule";
        #define EIT_OTHER_SCHD table[0x60]
        for(int i = 0x61; i <= 0x6F; i++) {
            table[i] = EIT_OTHER_SCHD;
        }
        table[0x70] = "Time Date Section";
        table[0x71] = "Running Status Section";
        table[0x72] = "Stuffing Section";
        table[0x73] = "Time Offset Section";
        for(int i = 0x74; i <= 0x7D; i++) {
            table[i] = RESERVED_FOR_FUTURE;
        }
        table[0x7E] = "Discontinuity Information Section";
        table[0x7F] = "Selection Information Section";
        table[0x80] = "User defined";
        #define USER_DEFINED table[0x80]
        for(int i = 0x81; i <= 0xFE; i++) {
            table[i] = USER_DEFINED;
        }
        table[0xFF] = RESERVED;

        table_inited = TRUE;
    }
    
    return table[id];


}

static const gchar* stream_type_desc(guint8 type) {
    static gboolean table_inited = FALSE;
    static gchar* table[256];
    if (table_inited == FALSE) {
        table[0x00] = "ITU-T | ISO/IEC reserved";
        table[0x01] = "ISO/IEC 11172-2 Video - H.261 - MPEG-1 Video";
        table[0x02] = "ITU-T H.262 | ISO/IEC 13818-2 Video - MPEG-2 Video | 11172-2 constrained parameter video stream";
        table[0x03] = "ISO/IEC 11172-3 Audio - MPEG-1 Audio Layer 2(MP2)";
        table[0x04] = "ISO/IEC 13818-3 Audio - MPEG-2 Audio Layer 3(MP3)";
        table[0x05] = "ITU-T H.222.0 | ISO/IEC 13818-1 private sections";
        table[0x06] = "ITU-T H.222.0 | ISO/IEC 13818-1 PES packets with private data";
        table[0x07] = "ISO/IEC 13522 MHEG";
        table[0x08] = "Annex A - DSM CC";
        table[0x09] = "ITU-T H222.1";
        table[0x0A] = "ISO 13818-6 type A - DSM CC";
        table[0x0B] = "ISO 13818-6 type B - DSM CC";
        table[0x0C] = "ISO 13818-6 type C - DSM CC";
        table[0x0D] = "ISO 13818-6 type D - DSM CC";
        table[0x0E] = "ISO 13818-6 auxiliary - DSM CC";
        table[0x0F] = "ITU-T H.222.0 | ISO/IEC 13818-1 reserved";
        for(int i = 0x10; i <= 0x7F; i++) {
            table[i] = table[0x0F];
        }
        table[0x80] = "User private";
        for(int i = 0x81; i <= 0xfF; i++) {
            table[i] = table[0x80];
        }
        /* from intel examples */
        table[0x0F] = "ISO/IEC 14496-3 Audio - AAC";
        table[0x81] = "Dolby Digital Audio - AC3";
        table[0x1B] = "ITU-T H.264 - ISO/IEC 14496-10 MPEG-4 Advanced Video Coding - AVC ";
    }
    return table[type];
}

#define CONVERT_8_TO_16(x,y) \
    (((x) << 8) | y) 
static void dump_pmt(const guint8* data, gsize size) {
    g_printf("program number: %u\n", CONVERT_8_TO_16(data[3], data[4]));
    g_printf("reserved: %u\n", data[5] >> 6);
    g_printf("version number: %u\n", (data[5] &0x3e) >> 1);
    g_printf("current next indication: %u\n", (data[5] & 0x01));
    g_printf("section number: %u\n", data[6]);
    g_printf("last section number: %u\n", data[7]);
    g_printf("reserved: %u\n", data[8] >> 5);
    g_printf("PCR_PID: %u\n", CONVERT_8_TO_16(data[8], data[9]) & 0x1fff);
    g_printf("reserved: %u\n", data[10] >> 4);

    guint16 info_lenght = CONVERT_8_TO_16(data[10], data[11]) & 0x0fff;
    g_printf("program info lenght: %u\n", info_lenght);
    /* for(int i = 0; i < info_lenght; i++) {
        
    }*/
    guint N = size - 4; 
    for(int i = 12 + info_lenght; i < N;) {
        guint8 stream_type = data[i];
        guint8 reserved1 = data[i+1] >> 5;
        guint16 pid = CONVERT_8_TO_16(data[i+1], data[i+2]) & 0x1fff;
        guint8 reserved2 = data[i+3] >> 4;
        guint16 es_info_lenght = CONVERT_8_TO_16(data[i+3], data[i+4]) & 0x0fff;

        g_printf("  stream type: %u 0x%02X (%s)\n", stream_type, stream_type, stream_type_desc(stream_type));
        g_printf("    reserved: %u\n", reserved1);
        g_printf("    PID: %u 0x%04X\n", pid, pid);
        g_printf("    reserved: %u\n", reserved2);
        g_printf("    ES info lenght: %u\n", es_info_lenght);

        g_assert(i + 5 + es_info_lenght < size);

        i += 5 + es_info_lenght;
    }
    guint32 crc = 0;
    crc |= data[N] << 24;
    crc |= data[N+1] << 16;
    crc |= data[N+2] << 8;
    crc |= data[N+3];
    g_printf("CRC: %u\n", crc);

}
static void dump_pat(const guint8* data, gsize size) {
    g_printf("transport stream id: %u\n", CONVERT_8_TO_16(data[3], data[4]));
    g_printf("reserved: %u\n", data[5] >> 6);
    g_printf("version number: %u\n", (data[5] &0x3e) >> 1);
    g_printf("current next indication: %u\n", (data[5] & 0x01));
    g_printf("section number: %u\n", data[6]);
    g_printf("last section number: %u\n", data[7]);
    //guint section_lenght = CONVERT_8_TO_16(data[1],data[2]) & 0x0fff;
    guint N = size - 4; 
    g_printf("number of programs: %u\n", (N-8)/4);
    for(int i = 8; i < N; i += 4) {
        guint8 program_number = CONVERT_8_TO_16(data[i], data[i+1]);
        guint8 reserved = data[i+2] >> 5;
        guint16 pid = CONVERT_8_TO_16(data[i+2], data[i+3]) & 0x1fff;
        g_printf("  program number: %u\n", program_number);
        g_printf("    reserved: %u\n", reserved);
        if (program_number == 0) {
            g_printf("    NETWORK PID: %u\n", pid);
        } else {
            g_printf("    PID: %u\n", pid);
        }
    }
    guint32 crc = 0;
    crc |= data[N] << 24;
    crc |= data[N+1] << 16;
    crc |= data[N+2] << 8;
    crc |= data[N+3];
    g_printf("CRC: %u\n", crc);
}

static void dump_generic(const guint8* data, gsize size) {

    guint8 has_table_id_ext = (data[1] & 0x80)?1:0;
    if (has_table_id_ext) {
        guint16 table_id_ext = (data[3] << 8) + data[3];
        g_printf("table id ext: %hu 0x%04X\n", table_id_ext, table_id_ext);
    }
    guint N = size - 4; 
    guint32 crc = 0;
    crc |= data[N] << 24;
    crc |= data[N+1] << 16;
    crc |= data[N+2] << 8;
    crc |= data[N+3];
    g_printf("CRC: %u\n", crc);

}
/**
 * called when new section arrives.
 */
static gboolean
_iface_mdw_demux_data_handler_new_data (
    MdwDemuxDataHandler *self, 
    const guint16 pid, 
    const gsize size, 
    const guint8* data) 
{
    g_assert( (CONVERT_8_TO_16(data[1], data[2]) & 0x0fff) == (size - 3));


    /* only one fuction per time (thread safe)*/
    static GStaticMutex mutex = G_STATIC_MUTEX_INIT;
    g_static_mutex_lock (&mutex);

    #define TABLE_ID data[0]
    g_printf("********NEW DUMP********\n");
    g_printf("PID: %u 0x%04X\n", pid, pid);
    g_printf("PACKET SIZE: %u\n", (guint) size);
    g_printf("\n");
    g_printf("table id: %u 0x%02X (%s)\n", TABLE_ID, TABLE_ID, table_id_to_name(TABLE_ID));
    g_printf("syntax indication: %u\n", (data[1] & 0x80)?1:0);
    g_printf("private indication: %u\n", (data[1] & 0x40)?1:0);
    g_printf("reserved: %u\n", (data[1] & 0x30) >> 4);
    g_printf("section lenght: %u\n", CONVERT_8_TO_16(data[1],data[2]) & 0x0fff);
   
    #define PAT_ID 0x00
    #define PMT_ID 0x02
    switch (TABLE_ID)  {
        case PAT_ID:
            dump_pat(data, size);
            break;
        case PMT_ID:
            dump_pmt(data, size);
            break;
        default:
            dump_generic(data, size);
            break;
    }

    g_printf("********END DUMP********\n");

    g_static_mutex_unlock (&mutex);
    return TRUE;
}

/**
 * initilize/override parent class methods, create signals, properties.
 */

static void
mdw_section_dumper_class_init (MdwSectionDumperClass *g_class) 
{
    //void
}

static void
mdw_section_dumper_init(MdwSectionDumper *self) 
{
    //void
}

/* Constructors */
MdwSectionDumper* 
mdw_section_dumper_new() {
    return (MdwSectionDumper*) g_object_new(MDW_TYPE_SECTION_DUMPER, NULL);
}

