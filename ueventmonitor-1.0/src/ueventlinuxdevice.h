#ifndef __UEVENT_LINUX_DEVICE_H
#define __UEVENT_LINUX_DEVICE_H

#include  <dbus/dbus-glib.h>
#include <gudev/gudev.h>

typedef struct _UeventLinuxDevice UeventLinuxDevice;
struct _UeventLinuxDevice
{
    /*< private >*/
    GObject parent_instance;
    /*< public >*/
    GUdevDevice *udev_device;
};

typedef struct _UeventLinuxDeviceClass UeventLinuxDeviceClass;
struct _UeventLinuxDeviceClass
{
    GObjectClass parent_class;
};

#define UEVENT_TYPE_LINUX_DEVICE  (uevent_linux_device_get_type ())
#define UEVENT_LINUX_DEVICE(o)    (G_TYPE_CHECK_INSTANCE_CAST ((o), UEVENT_TYPE_LINUX_DEVICE, UeventLinuxDevice))
#define UEVENT_IS_LINUX_DEVICE(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), UEVENT_TYPE_LINUX_DEVICE))

UeventLinuxDevice *
uevent_linux_device_new_sync (GUdevDevice *udev_device);
#endif