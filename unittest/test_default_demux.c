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
#include "mdw_section_dumper.h"
#include "mdw_default_demux.h"
#include "mdw_demux_data_handler_adapter.h"


#define NOT_NULL(x) OBLIGATORY((x) != NULL)

#define OBLIGATORY(x) \
    {   \
        if (!(x)) { \
            fail(#x); \
        } \
    } while (0);

void new_setup() {
    g_type_init();
}

void new_teardown() {
}

START_TEST( test_new_g_object ) {
   
    MdwDefaultDemux *d;

    NOT_NULL(d = g_object_new(MDW_TYPE_DEFAULT_DEMUX, NULL));
    
    g_object_unref(d);
    d = NULL;
    
}
END_TEST

START_TEST( test_new_func ) {
   
    MdwDefaultDemux *d;

    NOT_NULL(d = mdw_default_demux_new());

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

static GMainLoop* main_loop = NULL;

static MdwDefaultDemux *demux = NULL;
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

    main_loop = g_main_loop_new(NULL, FALSE);
    demux = mdw_default_demux_new();

    section_handler = mdw_demux_data_handler_adapter_new();
    section_handler->new_data = new_section_data_cb;

    source = MDW_TS_SOURCE(mdw_ts_file_source_new_with_location(TS_FILE));

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
    g_object_unref(section_handler);
    g_object_unref(pid_1_handler);
    g_object_unref(pid_2_handler);
    g_object_unref(invalid_pid_handler);

    g_main_loop_unref(main_loop);

    pid_1_handler = NULL;
    pid_2_handler = NULL;
    invalid_pid_handler = NULL;
    demux = NULL;
    section_handler = NULL;
    demux = NULL;
}

gboolean main_loop_quit(gpointer data) {
    g_main_loop_quit(main_loop);
    return FALSE;
}


START_TEST( test_section_callback ) {

    OBLIGATORY(mdw_demux_add_section_handler(MDW_DEMUX(demux),
        MDW_DEMUX_DATA_HANDLER(section_handler), SECTION_PID) == TRUE);
    OBLIGATORY(mdw_demux_set_ts_source(MDW_DEMUX(demux), source) == TRUE);

    OBLIGATORY(mdw_demux_start(MDW_DEMUX(demux)) == TRUE);
    
    g_timeout_add(2000, main_loop_quit, NULL);
    g_main_loop_run(main_loop);

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

gboolean test_pes_callback_aux0(gpointer __nil) {
    OBLIGATORY(mdw_demux_set_ts_source(MDW_DEMUX(demux), source) == TRUE);
    
    OBLIGATORY(mdw_demux_add_pes_handler(MDW_DEMUX(demux),
        MDW_DEMUX_DATA_HANDLER(pid_1_handler), VALID_PID_1) == TRUE);

    OBLIGATORY(mdw_demux_add_pes_handler(MDW_DEMUX(demux),
        MDW_DEMUX_DATA_HANDLER(invalid_pid_handler), INVALID_PID) == TRUE);
   
    OBLIGATORY(mdw_demux_start(MDW_DEMUX(demux)) == TRUE);
    
    return FALSE;
}

gboolean test_pes_callback_aux1(gpointer __nil) {
    OBLIGATORY(mdw_demux_add_pes_handler(MDW_DEMUX(demux),
        MDW_DEMUX_DATA_HANDLER(pid_2_handler), VALID_PID_2) == TRUE);
    return FALSE;
}

gboolean test_pes_callback_aux2(gpointer __nil) {
    OBLIGATORY(mdw_demux_stop(MDW_DEMUX(demux)) == TRUE);

    OBLIGATORY(pid_1_handler_called > 1);
    OBLIGATORY(pid_2_handler_called == 1);

    OBLIGATORY(mdw_demux_del_pes_handler(MDW_DEMUX(demux), 
        MDW_DEMUX_DATA_HANDLER(pid_2_handler), VALID_PID_2) == FALSE);

    OBLIGATORY(mdw_demux_del_pes_handler(MDW_DEMUX(demux),
        MDW_DEMUX_DATA_HANDLER(pid_1_handler), VALID_PID_2) == FALSE);

    OBLIGATORY(mdw_demux_del_pes_handler(MDW_DEMUX(demux),
        MDW_DEMUX_DATA_HANDLER(pid_1_handler), VALID_PID_1) == TRUE);

    OBLIGATORY(mdw_demux_del_pes_handler(MDW_DEMUX(demux),
        MDW_DEMUX_DATA_HANDLER(pid_1_handler), VALID_PID_1) == FALSE);

    return FALSE;
}


START_TEST( test_pes_callback )  {

    g_timeout_add(100,  test_pes_callback_aux0, NULL);
    g_timeout_add(1000, test_pes_callback_aux1, NULL);
    g_timeout_add(2000, test_pes_callback_aux2, NULL);
    g_timeout_add(2100, main_loop_quit, NULL);
    g_main_loop_run(main_loop);

}
END_TEST



static Suite *default_demux_suite(void) 
{
    Suite *s = suite_create("class MdwDefaultDemux ");

    /* tc_new */
    TCase *tc_new = tcase_create("Constructors");
    tcase_add_test(tc_new, test_new_g_object);
    tcase_add_test(tc_new, test_new_func);

    tcase_add_checked_fixture(tc_new, new_setup, new_teardown);

    suite_add_tcase(s, tc_new);
    /* tc_use */
    TCase *tc_use = tcase_create("Normal use of class");
    
    tcase_add_test(tc_use, test_section_callback);
    tcase_add_test(tc_use, test_pes_callback);

    tcase_add_checked_fixture(tc_use, use_setup, use_teardown);
    tcase_set_timeout(tc_use, 30);

    suite_add_tcase(s, tc_use);
   

    return s;
}

int main(void) {
    int nf;
    Suite *s = default_demux_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_VERBOSE);
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;

}

