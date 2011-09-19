#include <glib.h>
#include <check.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#define GETTEXT_PACKAGE "lots"
#include <glib/gi18n-lib.h>

#include <glib.h>
#include "mdw_manager.h"

GMainLoop* main_loop = NULL;
void handler(int a) {

    g_main_loop_quit(main_loop);

}
int main(int argc, char** argv) {

    g_type_init();
    bindtextdomain (GETTEXT_PACKAGE, "/usr/share/locale");
    textdomain (GETTEXT_PACKAGE);

    MdwManager* manager = NULL;

    manager = mdw_manager_new();
    gboolean ret = mdw_manager_dbus_register(manager, NULL);
    if (ret == FALSE) {
        return 1;
    }

    main_loop = g_main_loop_new(NULL, FALSE);

    g_debug(_("DAEMON INITED"));

    signal(SIGTERM, handler);
    signal(SIGHUP, handler);
    signal(SIGQUIT, handler);
    signal(SIGINT, handler);

    g_main_loop_run(main_loop);

    g_debug(_("DAEMON DOWN"));

    g_object_unref(manager);
    g_main_loop_unref(main_loop);
    exit(EXIT_SUCCESS);
}
