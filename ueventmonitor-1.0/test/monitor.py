#! /usr/bin/python

import dbus
import gobject
import dbus.mainloop.glib

def UeventAdded(serial, vendor, model, uuid):
    print("UeventAdded serial:%s vendor:%s model:%s uuid:%s" % (serial, vendor, model, uuid))

def UeventRemoved(serial, vendor, model, uuid):
    print("UeventRemoved serial:%s vendor:%s model:%s uuid:%s" % (serial, vendor, model, uuid))

dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
bus = dbus.SystemBus()
obj = bus.get_object("org.freedesktop.UeventMonitor", "/org/freedesktop/UeventMonitor")
interface = dbus.Interface(obj, "org.freedesktop.UeventMonitor.Base")
interface.connect_to_signal("UeventAdded", UeventAdded)
interface.connect_to_signal("UeventRemoved", UeventRemoved)
gobject.MainLoop().run()
