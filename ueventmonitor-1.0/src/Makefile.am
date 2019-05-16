AM_CPPFLAGS = \
	$(DBUS_GLIB_CFLAGS) \
	$(UDISKS2_CFLAGS) \
	$(GUDEV_CFLAGS) \
	-DPACKAGE_LIBEXEC_DIR=\""$(libexecdir)"\"

LIBS = \
	$(DBUS_GLIB_LIBS) \
	$(UDISKS2_LIBS) \
	$(GUDEV_LIBS)

ueventmonitorprivdir = $(libexecdir)/ueventmonitor
ueventmonitorpriv_PROGRAMS = ueventmonitord
ueventmonitord_SOURCES = main.c ueventmonitor.c ueventlinuxdevice.c

BUILT_SOURCES = ueventmonitor-glue.h

$(BUILT_SOURCES) : org.freedesktop.UeventMonitor.xml
	$(LIBTOOL) --mode=execute dbus-binding-tool --prefix=ueventmonitor --mode=glib-server \
                        --output=ueventmonitor-glue.h $(srcdir)/org.freedesktop.UeventMonitor.xml

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = ueventmonitor.xml

