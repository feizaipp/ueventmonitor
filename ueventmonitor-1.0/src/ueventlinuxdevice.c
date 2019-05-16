#include "ueventlinuxdevice.h"

G_DEFINE_TYPE (UeventLinuxDevice, uevent_linux_device, G_TYPE_OBJECT);


UeventLinuxDevice *
uevent_linux_device_new_sync (GUdevDevice *udev_device)
{
    UeventLinuxDevice *device;

    g_return_val_if_fail (G_UDEV_IS_DEVICE (udev_device), NULL);

    device = g_object_new (UEVENT_TYPE_LINUX_DEVICE, NULL);
    device->udev_device = g_object_ref (udev_device);

    return device;
}

static void
uevent_linux_device_finalize (GObject *object)
{
    UeventLinuxDevice *device = UEVENT_LINUX_DEVICE (object);

    g_clear_object (&device->udev_device);

    G_OBJECT_CLASS (uevent_linux_device_parent_class)->finalize (object);
}

static void
uevent_linux_device_init (UeventLinuxDevice *device)
{
}

static void
uevent_linux_device_class_init (UeventLinuxDeviceClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->finalize     = uevent_linux_device_finalize;
}

