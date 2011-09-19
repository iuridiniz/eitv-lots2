#include <glib.h>
#include <check.h>
#include <stdlib.h>

#include "sec_filter.h"

#define NOT_NULL(x) OBLIGATORY((x) != NULL)

#define OBLIGATORY(x) \
    {   \
        if (!(x)) { \
            fail(#x); \
        } \
    } while (0)

SecFilter* filter = NULL;

void 
use_setup()
{
    filter = sec_filter_new();

}

void 
use_teardown() 
{
    sec_filter_unref(filter);
    filter = NULL;
}

START_TEST(test_new_destroy_sec_filter) 
{
    SecFilter* f = sec_filter_new();
    NOT_NULL(f);
    sec_filter_unref(f);
}
END_TEST

#define TS_SEC_PID 0x002A
#define TS_TABLE_ID 0xB5
#define TS_TABLE_ID_EXT 0xD8D8

static void 
null_callback(const GByteArray* a, void* user_data) 
{
    // void 
}

START_TEST(test_add_remove_filter) 
{
    guint id = 0;
    OBLIGATORY( FALSE == sec_filter_remove(filter, id) ); /* id == 0 is a invalid id */

    id = sec_filter_add_filter(
        filter, 
        TS_SEC_PID, TS_TABLE_ID, TS_TABLE_ID_EXT, 
        null_callback,
        NULL
    );

    OBLIGATORY(id > 0);

    OBLIGATORY( TRUE == sec_filter_remove(filter, id) );

    /* already removed */
    OBLIGATORY( FALSE == sec_filter_remove(filter, id) );
}
END_TEST

/* valid */
#define TS_SEC_PID_01 0x002A
#define TS_TABLE_ID_01 60
#define TS_TABLE_ID_EXT_01 0

static void
sec_callback_01 (const GByteArray *byteArray,void *user_data)
{
    guint *var= (guint*)user_data;
    (*var)++;    
    /* verify if the GByteArray has the correct information about table_id and table_id_ext */
    guint8 *data = byteArray->data;
    
    guint8 table_id = data[0];
    guint16 table_id_ext = (data[3] << 8) & data[4];

    OBLIGATORY(table_id == TS_TABLE_ID_01);
    OBLIGATORY(table_id_ext == TS_TABLE_ID_EXT_01);
}
/* valid */
#define TS_SEC_PID_02 0x1200
#define TS_TABLE_ID_02 61
#define TS_TABLE_ID_EXT_02 0

static void
sec_callback_02 (const GByteArray *byteArray,void *user_data)
{
    guint *var= (guint*)user_data;
    (*var)++;    
    /* verify if the GByteArray has the correct information about table_id and table_id_ext */
    guint8 *data = byteArray->data;
    
    guint8 table_id = data[0];
    guint16 table_id_ext = (data[3] << 8) & data[4];

    OBLIGATORY(table_id == TS_TABLE_ID_02);
    OBLIGATORY(table_id_ext == TS_TABLE_ID_EXT_02);
}

/* valid with any table_id */
#define TS_SEC_PID_03 0x002A

static void
sec_callback_03 (const GByteArray *byteArray,void *user_data)
{
    guint *var= (guint*)user_data;
    (*var)++;    

}


/* invalid */
#define TS_SEC_PID_INVALID 0x002A
#define TS_TABLE_ID_INVALID 60
#define TS_TABLE_ID_EXT_INVALID 99

static void
sec_callback_invalid (const GByteArray *byteArray,void *user_data)
{
    guint *var= (guint*)user_data;
    (*var)++;    
}


static gboolean 
main_loop_quit(GMainLoop* loop)
{
    g_main_loop_quit(loop);
    return FALSE;
}

START_TEST(test_callback_filter) {

    guint id_01 = 0;
    guint id_02 = 0;
    guint id_03 = 0;
    guint id_invalid = 0;

    guint var_01 = 1;
    guint var_02 = 1;
    guint var_03 = 1;
    guint var_invalid = 1;
    
    id_01 = sec_filter_add_filter(
        filter, 
        TS_SEC_PID_01, TS_TABLE_ID_01, TS_TABLE_ID_EXT_01, 
        sec_callback_01,
        &var_01
    );

    id_02 = sec_filter_add_filter(
        filter, 
        TS_SEC_PID_02, TS_TABLE_ID_02, TS_TABLE_ID_EXT_02, 
        sec_callback_02,
        &var_02
    );
    /* any */
    id_03 = sec_filter_add_filter(
        filter, 
        TS_SEC_PID_03, -1, -1, 
        sec_callback_03,
        &var_03
    );

    id_invalid = sec_filter_add_filter(
        filter, 
        TS_SEC_PID_INVALID, TS_TABLE_ID_INVALID, TS_TABLE_ID_EXT_INVALID, 
        sec_callback_invalid,
        &var_invalid
    );

    GMainLoop *loop = NULL;

    loop = g_main_loop_new(NULL, FALSE);

    g_timeout_add(5000, (GSourceFunc) main_loop_quit, loop);
    g_main_loop_run(loop);

    sec_filter_remove(filter, id_01);
    sec_filter_remove(filter, id_02);
    sec_filter_remove(filter, id_03);
    sec_filter_remove(filter, id_invalid);

    OBLIGATORY( var_01 > 1 );
    OBLIGATORY( var_02 > 1 );
    OBLIGATORY( var_03 > 1 );
    OBLIGATORY( var_invalid == 1 );

}
END_TEST

Suite* 
sec_filter_suite() 
{

    Suite *s = suite_create("Sec Filter testing");

    /* tc_new */
    TCase *tc_new = tcase_create("Sec Filter New");
    tcase_add_test(tc_new, test_new_destroy_sec_filter);

    //tcase_add_checked_fixture(tc_new, new_setup, new_teardown);
    //tcase_set_timeout(tc_new, 10);

    suite_add_tcase(s, tc_new);

    /* tc_use */
    TCase *tc_use = tcase_create("Sec Filter API");
    tcase_add_test(tc_use, test_add_remove_filter);
    tcase_add_test(tc_use, test_callback_filter);

    tcase_add_checked_fixture(tc_use, use_setup, use_teardown);
    tcase_set_timeout(tc_use, 20);
    
    suite_add_tcase(s, tc_use);

    return s;

}

int 
main(int argc, char** argv)
{
    int nf;
    Suite *s = sec_filter_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_VERBOSE);
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
 

}
