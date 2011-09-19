/*@COPYRIGHT@*/
#include <stdlib.h>
#include <glib.h>
#include <check.h>
#include <string.h>

#define TS_FILE_01 "sample_100pkts.ts"
#define TS_FILE_02 "sample_1000pkts.ts"

#include "ts_file_01.h"
#include "ts_file_02.h"

#include "mdw_ts_file_source.h"


#define NOT_NULL(x) OBLIGATORY((x) != NULL)

#define OBLIGATORY(x) \
    {   \
        if (!(x)) { \
            fail(#x); \
        } \
    } while (0);


MdwTsFileSource *s = NULL;
void new_setup(void) {
    g_type_init();
}


void new_teardown(void) {
}

void use_setup(void) {
    g_type_init();
    s = g_object_new(MDW_TYPE_TS_FILE_SOURCE, NULL);
}


void use_teardown(void) {
    g_object_unref(s);
}



/* Test the Constructors */
START_TEST( test_new_g_object ) {
    NOT_NULL(s = g_object_new(MDW_TYPE_TS_FILE_SOURCE, NULL));
    g_object_unref(s);
    s = NULL;
}
END_TEST

START_TEST( test_new_g_object_arg ) {
    NOT_NULL(s = g_object_new(MDW_TYPE_TS_FILE_SOURCE, "location", TS_FILE_01, NULL));
    
    GValue val = {0, };
    g_value_init (&val, G_TYPE_STRING);

    g_object_get_property(G_OBJECT(s), "location", &val);

    NOT_NULL(g_value_get_string(&val));

    OBLIGATORY(strcmp(g_value_get_string(&val), TS_FILE_01)==0);

    g_value_unset(&val);
    g_object_unref(s);
    s = NULL;
}
END_TEST

START_TEST( test_new_func ) {
    NOT_NULL(s = mdw_ts_file_source_new());
    g_object_unref(s);
    s = NULL;
}
END_TEST

START_TEST( test_new_func_arg ) {
    NOT_NULL(s = mdw_ts_file_source_new_with_location(TS_FILE_01));

    GValue val = {0, };
    g_value_init (&val, G_TYPE_STRING);

    g_object_get_property(G_OBJECT(s), "location", &val);

    NOT_NULL(g_value_get_string(&val));

    OBLIGATORY(strcmp(g_value_get_string(&val), TS_FILE_01)==0);

    g_value_unset(&val);

    g_object_unref(s);
    s = NULL;
}
END_TEST
/*************/


/*** USE TESTS ***/
START_TEST( test_location_and_read )  {
    guint8 buffer[256];
    GValue val = {0, };
    GValue val2 = {0, };
    /* there's no location setted */
    OBLIGATORY(mdw_ts_source_read(MDW_TS_SOURCE(s), buffer, 256) == -1);

    /* set location */
    g_value_init(&val, G_TYPE_STRING);

    g_value_set_string(&val, TS_FILE_01);
    g_object_set_property(G_OBJECT(s), "location", &val);
    g_value_unset(&val);

    /* Was location setted correctly? */
    g_value_init(&val2, G_TYPE_STRING);

    g_object_get_property(G_OBJECT(s), "location", &val2);
    NOT_NULL(g_value_get_string(&val2));
    OBLIGATORY(strcmp(g_value_get_string(&val2), TS_FILE_01)==0);
    g_value_unset(&val2);

    /* there's a location now, so read must be ok */
    OBLIGATORY(mdw_ts_source_read(MDW_TS_SOURCE(s), buffer, 256) == 256);

    /* is the buffer wrong? */
    /*FIXME: use memcmp */
    for (int i = 0; i < 256; i++) {
        OBLIGATORY(buffer[i] == ts_file_01[i]);
    }

    /* now change the location */
    /* set location */
    g_value_init(&val, G_TYPE_STRING);

    g_value_set_string(&val, TS_FILE_02);
    g_object_set_property(G_OBJECT(s), "location", &val);
    g_value_unset(&val);

    /* Was location setted correctly? */
    g_value_init(&val2, G_TYPE_STRING);

    g_object_get_property(G_OBJECT(s), "location", &val2);
    NOT_NULL(g_value_get_string(&val2));
    OBLIGATORY(strcmp(g_value_get_string(&val2), TS_FILE_02)==0);
    g_value_unset(&val2);

    /* there's a new location now, so read must be ok */
    OBLIGATORY(mdw_ts_source_read(MDW_TS_SOURCE(s), buffer, 256) == 256);

    /* is the buffer wrong? */
    /*FIXME: use memcmp */
    for (int i = 0; i < 256; i++) {
        OBLIGATORY(buffer[i] == ts_file_02[i]);
    }

    /* uninitialize is done by teardown*/
    //g_object_unref(G_OBJECT(s));

}
END_TEST

START_TEST( test_continuous_and_read ) {
    GValue val = {0, };
    GValue val2 = {0, };
    guint8 buffer[2 * sizeof(ts_file_01) + 20];

    /* set location */
    g_value_init(&val, G_TYPE_STRING);

    g_value_set_string(&val, TS_FILE_01);
    g_object_set_property(G_OBJECT(s), "location", &val);
    g_value_unset(&val);
    //return;
    /* read the full file */
    OBLIGATORY(mdw_ts_source_read(MDW_TS_SOURCE(s), buffer, sizeof(ts_file_01)) == sizeof(ts_file_01));
    
    /* Any subsequence call must be 0 */
    OBLIGATORY(mdw_ts_source_read(MDW_TS_SOURCE(s), buffer, sizeof(ts_file_01)) == 0);

    /* set to rewind automaticaly */
    g_value_init(&val, G_TYPE_BOOLEAN);
    g_value_set_boolean(&val, TRUE);
    g_object_set_property(G_OBJECT(s), "continuous", &val);
    g_value_unset(&val);

    /* Was the continuous property setted correctly? */
    g_value_init(&val2, G_TYPE_BOOLEAN);
    g_object_get_property(G_OBJECT(s), "continuous", &val2);
    OBLIGATORY(g_value_get_boolean(&val2) == TRUE);
    g_value_unset(&val2);

    /* All calls must be return all bytes */

    OBLIGATORY(mdw_ts_source_read(MDW_TS_SOURCE(s), buffer, sizeof(ts_file_01)) == sizeof(ts_file_01));

    OBLIGATORY(mdw_ts_source_read(MDW_TS_SOURCE(s), buffer, sizeof(ts_file_01)) == sizeof(ts_file_01));

    OBLIGATORY(mdw_ts_source_read(MDW_TS_SOURCE(s), buffer, 2 * sizeof(ts_file_01)+20) == 2 * sizeof(ts_file_01) + 20);

    /* unset continuous property */
    g_value_init(&val, G_TYPE_BOOLEAN);
    g_value_set_boolean(&val, FALSE);
    g_object_set_property(G_OBJECT(s), "continuous", &val);
    g_value_unset(&val);

    /* Was the continuous property setted correctly? */
    g_value_init(&val2, G_TYPE_BOOLEAN);
    g_object_get_property(G_OBJECT(s), "continuous", &val2);
    OBLIGATORY(g_value_get_boolean(&val2) == FALSE);
    g_value_unset(&val2);

    /* try to read again */
    OBLIGATORY(mdw_ts_source_read(MDW_TS_SOURCE(s), buffer, sizeof(ts_file_01)) == sizeof(ts_file_01) - 20);


}
END_TEST


/*****************/



static Suite *ts_file_source_suite(void) {
    Suite *s = suite_create("class MdwTsFileSouce ");

    /* tc_new */
    TCase *tc_new = tcase_create("Constructors");
    tcase_add_test(tc_new, test_new_g_object);
    tcase_add_test(tc_new, test_new_g_object_arg);
    tcase_add_test(tc_new, test_new_func);
    tcase_add_test(tc_new, test_new_func_arg);

    tcase_add_checked_fixture(tc_new, new_setup, new_teardown);

    suite_add_tcase(s, tc_new);

    /* tc_use */
    TCase *tc_use = tcase_create("Normal use of class");
    
    tcase_add_test(tc_use, test_location_and_read);
    tcase_add_test(tc_use, test_continuous_and_read);

    tcase_add_checked_fixture(tc_use, use_setup, use_teardown);

    suite_add_tcase(s, tc_use);
    
    return s;
}

int main(void) {
    int nf;
    Suite *s = ts_file_source_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_VERBOSE);
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;

}

