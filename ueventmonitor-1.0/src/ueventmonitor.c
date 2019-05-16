#include "ueventmonitor.h"
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

G_LOCK_DEFINE_STATIC (monitor_lock);

G_DEFINE_TYPE (UeventMonitor, uevent_monitor, G_TYPE_OBJECT)

static guint signals[LAST_SIGNAL] = { 0 };

UeventMonitor *
uevent_monitor_new (void)
{
    return UEVENT_MONITOR (g_object_new (UEVENT_MONITOR_TYPE, NULL));
}

static void
uevent_monitor_finalize (GObject *object)
{
    UeventMonitor *monitor = UEVENT_MONITOR (object);

    /* stop the request thread and wait for it */
    g_async_queue_push (monitor->probe_request_queue, (gpointer) 0xdeadbeef);
    g_thread_join (monitor->probe_request_thread);
    g_async_queue_unref (monitor->probe_request_queue);

    if (G_OBJECT_CLASS (uevent_monitor_parent_class)->finalize != NULL)
        G_OBJECT_CLASS (uevent_monitor_parent_class)->finalize (object);
}

gchar *
uevent_decode_udev_string (const gchar *str)
{
    GString *s;
    gchar *ret;
    const gchar *end_valid;
    guint n;

    if (str == NULL)
    {
        ret = NULL;
        goto out;
    }

    s = g_string_new (NULL);
    for (n = 0; str[n] != '\0'; n++)
    {
        if (str[n] == '\\')
        {
            gint val;

            if (str[n + 1] != 'x' || str[n + 2] == '\0' || str[n + 3] == '\0')
            {
                printf ("**** NOTE: malformed encoded string `%s'", str);
                break;
            }

            val = (g_ascii_xdigit_value (str[n + 2]) << 4) | g_ascii_xdigit_value (str[n + 3]);

            g_string_append_c (s, val);

            n += 3;
        }
        else
        {
            g_string_append_c (s, str[n]);
        }
    }

    if (!g_utf8_validate (s->str, -1, &end_valid))
    {
        printf ("The string `%s' is not valid UTF-8. Invalid characters begins at `%s'", s->str, end_valid);
        ret = g_strndup (s->str, end_valid - s->str);
        g_string_free (s, TRUE);
    }
    else
    {
        ret = g_string_free (s, FALSE);
    }

out:
    return ret;
}

/* called with lock held */
static void
handle_block_uevent (UeventMonitor *monitor,
                     const gchar         *action,
                     UeventLinuxDevice   *device)
{
    const gchar *uuid;
    const gchar *serial;
    const gchar *vendor;
    const gchar *model;
    
    if (g_strcmp0 (action, "remove") == 0)
    {
        uuid = uevent_decode_udev_string (g_udev_device_get_property (device->udev_device, "ID_FS_UUID_ENC"));
        serial = g_udev_device_get_property (device->udev_device, "ID_SERIAL_SHORT");
        vendor = g_udev_device_get_property (device->udev_device, "ID_VENDOR");
        model = g_udev_device_get_property (device->udev_device, "ID_MODEL");
        printf("remove uuid:%s serial:%s vendor:%s model:%s\n", uuid, serial, vendor, model);
        if (uuid && serial && vendor && model) {
            g_signal_emit(monitor, signals[UEVENT_REMOVED_SIGNAL], 0, serial, vendor, model, uuid);
        }
    }
    else
    {
        uuid = uevent_decode_udev_string (g_udev_device_get_property (device->udev_device, "ID_FS_UUID_ENC"));
        serial = g_udev_device_get_property (device->udev_device, "ID_SERIAL_SHORT");
        vendor = g_udev_device_get_property (device->udev_device, "ID_VENDOR");
        model = g_udev_device_get_property (device->udev_device, "ID_MODEL");
        printf("add uuid:%s serial:%s vendor:%s model:%s\n", uuid, serial, vendor, model);
        if (uuid && serial && vendor && model) {
            g_signal_emit(monitor, signals[UEVENT_ADDED_SIGNAL], 0, serial, vendor, model, uuid);
        }
    }
}
/* called without lock held */
static void
uevent_monitor_handle_uevent (UeventMonitor *monitor,
                                     const gchar         *action,
                                     UeventLinuxDevice   *device)
{
    const gchar *subsystem;

    G_LOCK (monitor_lock);

#if 0
    printf ("uevent %s %s",
                action,
                g_udev_device_get_sysfs_path (device->udev_device));
#endif

    subsystem = g_udev_device_get_subsystem (device->udev_device);
    if (g_strcmp0 (subsystem, "block") == 0)
    {
        handle_block_uevent (monitor, action, device);
    }

    G_UNLOCK (monitor_lock);
}

static void
probe_request_free (ProbeRequest *request)
{
    g_clear_object (&request->monitor);
    g_clear_object (&request->udev_device);
    g_clear_object (&request->uevent_device);
    g_slice_free (ProbeRequest, request);
}

/* called in main thread with a processed ProbeRequest struct - see probe_request_thread_func() */
static gboolean
on_idle_with_probed_uevent (gpointer user_data)
{
    ProbeRequest *request = user_data;
    uevent_monitor_handle_uevent (request->monitor,
                                        g_udev_device_get_action (request->udev_device),
                                        request->uevent_device);
    probe_request_free (request);
    return FALSE; /* remove source */
}

gpointer
probe_request_thread_func (gpointer user_data)
{
  UeventMonitor *provider = UEVENT_MONITOR (user_data);
  ProbeRequest *request;

  do
    {
      request = g_async_queue_pop (provider->probe_request_queue);

      /* used by _finalize() above to stop this thread - if received, we can
       * no longer use @provider
       */
      if (request == (gpointer) 0xdeadbeef)
        goto out;

      /* probe the device - this may take a while */
      request->uevent_device = uevent_linux_device_new_sync (request->udev_device);

      /* now that we've probed the device, post the request back to the main thread */
      g_idle_add (on_idle_with_probed_uevent, request);
    }
  while (TRUE);

 out:
  return NULL;
}

static void
on_uevent (GUdevClient  *client,
           const gchar  *action,
           GUdevDevice  *device,
           gpointer      user_data)
{
    UeventMonitor *monitor = UEVENT_MONITOR (user_data);
    ProbeRequest *request;

    request = g_slice_new0 (ProbeRequest);
    request->monitor = g_object_ref (monitor);
    request->udev_device = g_object_ref (device);

    /* process uevent in "probing-thread" */
    g_async_queue_push (monitor->probe_request_queue, request);
}

static void
uevent_monitor_init (UeventMonitor *monitor)
{
    const gchar *subsystems[] = {"block", "iscsi_connection", "scsi", NULL};
    GFile *file;
    GError *error = NULL;

    /* get ourselves an udev client */
    monitor->gudev_client = g_udev_client_new (subsystems);
    g_signal_connect (monitor->gudev_client,
                    "uevent",
                    G_CALLBACK (on_uevent),
                    monitor);
    monitor->probe_request_queue = g_async_queue_new ();
    monitor->probe_request_thread = g_thread_new ("probing-thread",
                                                 probe_request_thread_func,
                                                 monitor);

}

static void
uevent_monitor_class_init (UeventMonitorClass *klass)
{
    GObjectClass *gobject_class = (GObjectClass *) klass;

    gobject_class->finalize    = uevent_monitor_finalize;

    signals[UEVENT_ADDED_SIGNAL] = g_signal_new ("uevent-added",
                                                G_OBJECT_CLASS_TYPE (klass),
                                                G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                                                0,
                                                NULL,
                                                NULL,
                                                g_cclosure_marshal_VOID__STRING,
                                                G_TYPE_NONE,
                                                4,
                                                G_TYPE_STRING, G_TYPE_STRING,
                                                G_TYPE_STRING, G_TYPE_STRING);

    signals[UEVENT_REMOVED_SIGNAL] = g_signal_new ("uevent-removed",
                                                G_OBJECT_CLASS_TYPE (klass),
                                                G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                                                0,
                                                NULL,
                                                NULL,
                                                g_cclosure_marshal_VOID__STRING,
                                                G_TYPE_NONE,
                                                4,
                                                G_TYPE_STRING, G_TYPE_STRING,
                                                G_TYPE_STRING, G_TYPE_STRING);
}