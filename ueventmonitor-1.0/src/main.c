#include "ueventmonitor.h"
#include <stdio.h>
#include "ueventmonitor-glue.h"

int main(int argc, char **argv)
{
    GMainLoop *mainLoop;
    DBusGConnection *bus;
    GError *error = NULL;
    DBusGProxy *bus_proxy;
    UeventMonitor *uevent_monitor;
    guint request_name_result;

    dbus_g_object_type_install_info(UEVENT_MONITOR_TYPE, &dbus_glib_ueventmonitor_object_info);
    mainLoop = g_main_loop_new(NULL, FALSE);
    bus = dbus_g_bus_get(DBUS_BUS_SYSTEM, &error);
    if (!bus) {
        printf("Cannot get system bus %s\n.", error->message);
        return 1;
    }
    bus_proxy = dbus_g_proxy_new_for_name(bus, "org.freedesktop.DBus", 
                                                                "/org/freedesktop/DBus", "org.freedesktop.DBus");
    if (!dbus_g_proxy_call(bus_proxy, "RequestName", 
                                    &error, G_TYPE_STRING, 
                                    "org.freedesktop.UeventMonitor", G_TYPE_UINT, 0,
			                        G_TYPE_INVALID, G_TYPE_UINT, &request_name_result,
			                        G_TYPE_INVALID)) {
        printf("Failed to acquire org.freedesktop.UeventMonitor %s.\n", error->message);
        return 1;
    }
    // new object
    uevent_monitor = uevent_monitor_new();
    dbus_g_connection_register_g_object(bus, "/org/freedesktop/UeventMonitor", G_OBJECT(uevent_monitor));
    printf ("EventMonitor server is running\n");
    g_main_loop_run(mainLoop);
    g_object_unref(bus_proxy);
    return 0;
}