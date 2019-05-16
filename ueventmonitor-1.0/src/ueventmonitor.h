#ifndef __UEVENT_MONITOR_H__
#define __UEVENT_MONITOR_H__
#include <udisks/udisks.h>
#include <gudev/gudev.h>
#include "ueventlinuxdevice.h"

typedef struct _UeventMonitor UeventMonitor;
struct _UeventMonitor
{
    GObject parent_instance;

    GUdevClient *gudev_client;
    GAsyncQueue *probe_request_queue;
    GThread *probe_request_thread;
};

typedef struct _UeventMonitorClass UeventMonitorClass;

struct _UeventMonitorClass
{
    GObjectClass parent_class;
};

typedef struct
{
    UeventMonitor *monitor;
    GUdevDevice *udev_device;
    UeventLinuxDevice *uevent_device;
} ProbeRequest;

enum
{
    UEVENT_ADDED_SIGNAL,
    UEVENT_REMOVED_SIGNAL,
    LAST_SIGNAL,
};


#define UEVENT_MONITOR_TYPE         (uevent_monitor_get_type ())
#define UEVENT_MONITOR(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), UEVENT_MONITOR_TYPE, UeventMonitor))
#define IS_UEVENT_MONITOR(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), UEVENT_MONITOR_TYPE))


#endif