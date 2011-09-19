/*@COPYRIGHT@*/
#include <stdlib.h>
#include <glib.h>
#include <check.h>
#include <string.h>
#include <unistd.h>

#include "config.h"


#include "mdw_demux.h"
#include "mdw_ts_file_source.h"
#include "mdw_ts_source.h"
#include "mdw_dvb_demux.h"
#include "mdw_dvb_audio_dummy.h"
#include "mdw_dvb_video_dummy.h"
#include "mdw_demux_data_handler_adapter.h"


#define NOT_NULL(x) OBLIGATORY((x) != NULL)

#define OBLIGATORY(x) \
    {   \
        if (!(x)) { \
            fail(#x); \
        } \
    } while (0)

void new_setup() {
    g_type_init();
}

void new_teardown() {
}

START_TEST( test_new_g_object ) {
   
    MdwDvbDemux *d;
    GValue val = {0, };
    g_value_init(&val, G_TYPE_STRING);

    NOT_NULL(d = g_object_new(MDW_TYPE_DVB_DEMUX, NULL));
    
    /* Default demux device */
    g_object_get_property(G_OBJECT(d), "demux-device", &val);

    NOT_NULL(g_value_get_string(&val));
    OBLIGATORY(strcmp(g_value_get_string(&val), DVB_BASE"/demux0") == 0);

    /* Default dvr device */
    g_object_get_property(G_OBJECT(d), "dvr-device", &val);

    NOT_NULL(g_value_get_string(&val));
    OBLIGATORY(strcmp(g_value_get_string(&val), DVB_BASE"/dvr0") == 0);

    g_value_unset(&val);
    g_object_unref(d);
    d = NULL;
    
}
END_TEST

START_TEST( test_new_g_object_arg ) {
    GValue val = {0, }, val2 = {0, };
    MdwDvbDemux *d;

    NOT_NULL(d = g_object_new(MDW_TYPE_DVB_DEMUX, 
        "demux-device", "/arhua", 
        "dvr-device", "/arhua2",
        NULL));

    /* get the value back*/
    g_value_init(&val, G_TYPE_STRING);
    g_object_get_property(G_OBJECT(d), "demux-device", &val);
    NOT_NULL(g_value_get_string(&val));
    OBLIGATORY(strcmp(g_value_get_string(&val), "/arhua") == 0);

    /* get the value back*/
    g_value_init(&val2, G_TYPE_STRING);
    g_object_get_property(G_OBJECT(d), "dvr-device", &val2);
    NOT_NULL(g_value_get_string(&val2));
    OBLIGATORY(strcmp(g_value_get_string(&val2), "/arhua2") == 0);

    g_value_unset(&val);
    g_value_unset(&val2);
    g_object_unref(d);
    d = NULL;
}
END_TEST

START_TEST( test_new_func ) {
   
    MdwDvbDemux *d;

    NOT_NULL(d = mdw_dvb_demux_new());

    g_object_unref(d);
    d = NULL;
    
}
END_TEST

/******************************************/
#define TS_FILE "sample.ts"
#define VALID_PID_1 2562
#define VALID_PID_2 2563
#define INVALID_PID 4000
#define SECTION_PID 39
#define PAT_PID 0
#define PMT_PID1 3328
#define PMT_PID2 3329
#define PMT_PID3 3330
#define PMT_PID4 3331


static MdwDvbDemux *demux = NULL;
static MdwDemuxDataHandlerAdapter *section_handler = NULL;
static int section_handler_called = 0;
static MdwTsSource  *source = NULL;

static int pid_1_handler_called = 0;
MdwDemuxDataHandlerAdapter *pid_1_handler = NULL;
static int pid_2_handler_called = 0;
MdwDemuxDataHandlerAdapter *pid_2_handler = NULL;
MdwDemuxDataHandlerAdapter *invalid_pid_handler = NULL;

static gboolean
new_section_data_cb (
    MdwDemuxDataHandler *self, 
    const guint16 pid, 
    const gsize size, 
    const guint8* data) 
{
    section_handler_called++;
    OBLIGATORY(self == MDW_DEMUX_DATA_HANDLER(section_handler));
    OBLIGATORY(pid == SECTION_PID);
    OBLIGATORY(size <= 4096 && size > 3);
    OBLIGATORY(size == ((data[1] & 0xf) << 8|data[2]) + 3);
    if (section_handler_called > 1) { 
        return FALSE;
    }   
    return TRUE; /* continue */
}

static gboolean 
new_pid_1_data_cb(
    MdwDemuxDataHandler *self, 
    const guint16 pid, 
    const gsize size, 
    const guint8* data) 
{
    pid_1_handler_called++;
    OBLIGATORY(size - 6 == ((data[4] << 8) | data[5]));

    guint32 prefix = ((data[0] << 16) | 
        (data[1] << 8) | 
        (data[2])
    );
    OBLIGATORY(prefix == 0x000001);

    OBLIGATORY(self == MDW_DEMUX_DATA_HANDLER(pid_1_handler));
    OBLIGATORY(pid == VALID_PID_1);
    return TRUE;
}

static gboolean 
new_pid_2_data_cb(
    MdwDemuxDataHandler *self, 
    const guint16 pid, 
    const gsize size, 
    const guint8* data) 
{
    pid_2_handler_called++;
    OBLIGATORY(size - 6 == ((data[4] << 8) | data[5]));

    guint32 prefix = ((data[0] << 16) | 
        (data[1] << 8) | 
        (data[2])
    );
    OBLIGATORY(prefix == 0x000001);

    OBLIGATORY(MDW_DEMUX_DATA_HANDLER_ADAPTER(self) == pid_2_handler);
    OBLIGATORY(pid == VALID_PID_2);
    return FALSE;
}

static gboolean 
invalid_pid_handler_cb(
    MdwDemuxDataHandler *self, 
    const guint16 pid, 
    const gsize size, 
    const guint8* data) 
{
    fail("invalid_pid_handler was called");
    return FALSE;
}


void use_setup() {
    g_type_init();

    demux = mdw_dvb_demux_new();

    section_handler = mdw_demux_data_handler_adapter_new();
    section_handler->new_data = new_section_data_cb;

    source = MDW_TS_SOURCE(mdw_ts_file_source_new_with_location(TS_FILE));
    //source = g_object_new(MDW_TYPE_TS_FILE_SOURCE, "location", TS_FILE, "continuous", TRUE, NULL);

    pid_1_handler = mdw_demux_data_handler_adapter_new();
    pid_2_handler = mdw_demux_data_handler_adapter_new();
    invalid_pid_handler = mdw_demux_data_handler_adapter_new();

    pid_1_handler->new_data = new_pid_1_data_cb;
    pid_2_handler->new_data = new_pid_2_data_cb;
    invalid_pid_handler->new_data = invalid_pid_handler_cb;

}

void use_teardown() {
    g_object_unref(source);
    g_object_unref(demux);
    g_object_unref(pid_1_handler);
    g_object_unref(pid_2_handler);
    g_object_unref(invalid_pid_handler);
    pid_1_handler = NULL;
    pid_2_handler = NULL;
    invalid_pid_handler = NULL;
    demux = NULL;
    demux = NULL;
}

gboolean loop_quit(gpointer data) {
    GMainLoop* loop = (GMainLoop*) data;
    g_main_loop_quit(loop);
    return FALSE;
}


START_TEST( test_section_callback ) {

    GMainLoop* loop = g_main_loop_new(NULL, FALSE);

    OBLIGATORY(mdw_demux_add_section_handler(MDW_DEMUX(demux),
        MDW_DEMUX_DATA_HANDLER(section_handler), SECTION_PID) == TRUE);
    OBLIGATORY(mdw_demux_set_ts_source(MDW_DEMUX(demux), source) == TRUE);

    OBLIGATORY(mdw_demux_start(MDW_DEMUX(demux)) == TRUE);
    
    g_timeout_add(2000, loop_quit, loop);
    g_main_loop_run(loop);

    OBLIGATORY(mdw_demux_stop(MDW_DEMUX(demux)) == TRUE);

    OBLIGATORY(section_handler_called == 2);

    /* REMOVING */
    /* already removed by itself */
    OBLIGATORY(mdw_demux_del_section_handler(MDW_DEMUX(demux), 
        MDW_DEMUX_DATA_HANDLER(section_handler), SECTION_PID) == FALSE);

    OBLIGATORY(mdw_demux_add_section_handler(MDW_DEMUX(demux),
        MDW_DEMUX_DATA_HANDLER(section_handler), SECTION_PID) == TRUE);

    OBLIGATORY(mdw_demux_del_section_handler(MDW_DEMUX(demux), 
        MDW_DEMUX_DATA_HANDLER(section_handler), SECTION_PID) == TRUE);

    OBLIGATORY(mdw_demux_del_section_handler(MDW_DEMUX(demux), 
        MDW_DEMUX_DATA_HANDLER(section_handler), SECTION_PID) == FALSE);

    /* wrong pid */
    OBLIGATORY(mdw_demux_add_section_handler(MDW_DEMUX(demux),
        MDW_DEMUX_DATA_HANDLER(section_handler), SECTION_PID) == TRUE);

    OBLIGATORY(mdw_demux_del_section_handler(MDW_DEMUX(demux), 
        MDW_DEMUX_DATA_HANDLER(section_handler), INVALID_PID) == FALSE);

    /* Two for same pid */
    OBLIGATORY(mdw_demux_add_section_handler(MDW_DEMUX(demux),
        MDW_DEMUX_DATA_HANDLER(section_handler), SECTION_PID) == TRUE);

    OBLIGATORY(mdw_demux_del_section_handler(MDW_DEMUX(demux), 
        MDW_DEMUX_DATA_HANDLER(section_handler), SECTION_PID) == TRUE);

    OBLIGATORY(mdw_demux_del_section_handler(MDW_DEMUX(demux), 
        MDW_DEMUX_DATA_HANDLER(section_handler), SECTION_PID) == TRUE);

    OBLIGATORY(mdw_demux_del_section_handler(MDW_DEMUX(demux), 
        MDW_DEMUX_DATA_HANDLER(section_handler), SECTION_PID) == FALSE);

}
END_TEST
gboolean test_pes_callback_aux0(gpointer nil) {
    OBLIGATORY(mdw_demux_set_ts_source(MDW_DEMUX(demux), source) == TRUE);
    
    OBLIGATORY(mdw_demux_add_pes_handler(MDW_DEMUX(demux),
        MDW_DEMUX_DATA_HANDLER(pid_1_handler), VALID_PID_1) == TRUE);

    OBLIGATORY(mdw_demux_add_pes_handler(MDW_DEMUX(demux),
        MDW_DEMUX_DATA_HANDLER(invalid_pid_handler), INVALID_PID) == TRUE);
   
    OBLIGATORY(mdw_demux_start(MDW_DEMUX(demux)) == TRUE);
   
    return FALSE;
}

gboolean test_pes_callback_aux1(gpointer nil) {
    
    OBLIGATORY(mdw_demux_add_pes_handler(MDW_DEMUX(demux),
        MDW_DEMUX_DATA_HANDLER(pid_2_handler), VALID_PID_2) == TRUE);

    return FALSE;
}
gboolean test_pes_callback_aux2(gpointer nil) {
    OBLIGATORY(mdw_demux_stop(MDW_DEMUX(demux)) == TRUE);
    OBLIGATORY(pid_1_handler_called > 1);
    OBLIGATORY(pid_2_handler_called == 1);

    return FALSE;
}

START_TEST( test_pes_callback )  {
    GMainLoop* loop = g_main_loop_new(NULL, FALSE);

    g_timeout_add(100, test_pes_callback_aux0, NULL);
    g_timeout_add(1000, test_pes_callback_aux1, NULL);
    g_timeout_add(2000, test_pes_callback_aux2, NULL);
    g_timeout_add(3000, loop_quit, loop);

    g_main_loop_run(loop);


}
END_TEST

START_TEST( test_2pes_callback ) {
    
    
    OBLIGATORY(mdw_demux_set_ts_source(MDW_DEMUX(demux), source) == TRUE);

    OBLIGATORY(mdw_demux_add_pes_handler(MDW_DEMUX(demux),
        MDW_DEMUX_DATA_HANDLER(pid_1_handler), VALID_PID_1) == TRUE);

    MdwDvbAudioDummy* audio_handler = mdw_dvb_audio_dummy_new();
    
    /* cannot add handlers of diferent types */
    OBLIGATORY(mdw_demux_add_pes_handler(MDW_DEMUX(demux),
        MDW_DEMUX_DATA_HANDLER(audio_handler), VALID_PID_1) == FALSE);

    OBLIGATORY(mdw_demux_add_pes_handler(MDW_DEMUX(demux),
        MDW_DEMUX_DATA_HANDLER(pid_1_handler), VALID_PID_1) == TRUE);

    MdwDvbVideoDummy* video_handler = mdw_dvb_video_dummy_new();

    /* cannot add handlers of diferent types */
    OBLIGATORY(mdw_demux_add_pes_handler(MDW_DEMUX(demux),
        MDW_DEMUX_DATA_HANDLER(video_handler), VALID_PID_1) == FALSE);
    
    OBLIGATORY(mdw_demux_add_pes_handler(MDW_DEMUX(demux),
        MDW_DEMUX_DATA_HANDLER(pid_1_handler), VALID_PID_1) == TRUE);

    OBLIGATORY(mdw_demux_add_pes_handler(MDW_DEMUX(demux),
        MDW_DEMUX_DATA_HANDLER(audio_handler), VALID_PID_2) == TRUE);

    OBLIGATORY(mdw_demux_add_pes_handler(MDW_DEMUX(demux),
        MDW_DEMUX_DATA_HANDLER(pid_2_handler), VALID_PID_2) == FALSE);
    

    /* removing from bottom to top*/
    OBLIGATORY(mdw_demux_del_pes_handler(MDW_DEMUX(demux),
        MDW_DEMUX_DATA_HANDLER(pid_2_handler), VALID_PID_2) == FALSE);

    OBLIGATORY(mdw_demux_del_pes_handler(MDW_DEMUX(demux),
        MDW_DEMUX_DATA_HANDLER(audio_handler), VALID_PID_2) == TRUE);

    OBLIGATORY(mdw_demux_del_pes_handler(MDW_DEMUX(demux),
        MDW_DEMUX_DATA_HANDLER(pid_1_handler), VALID_PID_1) == TRUE);

    OBLIGATORY(mdw_demux_del_pes_handler(MDW_DEMUX(demux),
        MDW_DEMUX_DATA_HANDLER(video_handler), VALID_PID_1) == FALSE);

    OBLIGATORY(mdw_demux_del_pes_handler(MDW_DEMUX(demux),
        MDW_DEMUX_DATA_HANDLER(pid_1_handler), VALID_PID_1) == TRUE);

    OBLIGATORY(mdw_demux_del_pes_handler(MDW_DEMUX(demux),
        MDW_DEMUX_DATA_HANDLER(audio_handler), VALID_PID_1) == FALSE);

    OBLIGATORY(mdw_demux_del_pes_handler(MDW_DEMUX(demux),
        MDW_DEMUX_DATA_HANDLER(pid_1_handler), VALID_PID_1) == TRUE);

    g_object_unref(video_handler);
    g_object_unref(audio_handler);

}
END_TEST


static Suite *dvb_demux_suite(void) 
{
    Suite *s = suite_create("class MdwDvbDemux ");

    /* tc_new */
    TCase *tc_new = tcase_create("Constructors");
    tcase_add_test(tc_new, test_new_g_object);
    tcase_add_test(tc_new, test_new_g_object_arg);
	tcase_add_test(tc_new, test_new_func);

    tcase_add_checked_fixture(tc_new, new_setup, new_teardown);
    suite_add_tcase(s, tc_new);

    /* tc_use */
    TCase *tc_use = tcase_create("Normal use of class");

    /* 30 executions for trying get a dead lock */
    /* tcase_add_loop_test(tc_use, test_section_callback, 0, 30); */
    
    tcase_add_test(tc_use, test_section_callback);
    tcase_add_test(tc_use, test_pes_callback);
    tcase_add_test(tc_use, test_2pes_callback);

    tcase_add_checked_fixture(tc_use, use_setup, use_teardown);
    tcase_set_timeout(tc_use, 30);
    suite_add_tcase(s, tc_use);
   

    return s;
}

int main(void) {
    int nf;
    Suite *s = dvb_demux_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_VERBOSE);
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;

}

