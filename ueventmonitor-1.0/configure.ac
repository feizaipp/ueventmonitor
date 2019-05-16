#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT(mountmonitor, 1.0)
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS(config.h)

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
PKG_CHECK_MODULES(DBUS_GLIB, dbus-glib-1, have_dbus_glib=yes, have_dbus_glib=no)
if test x$have_dbus_glib = xno ; then
    AC_MSG_ERROR([DBus-GLib development libraries not found])
fi

AM_CONDITIONAL(HAVE_DBUS_GLIB, test x$have_dbus_glib = xyes)

AC_SUBST(DBUS_GLIB_CFLAGS)
AC_SUBST(DBUS_GLIB_LIBS)

PKG_CHECK_MODULES(UDISKS2, udisks2, have_udisks2=yes, have_udisks2=no)
if test x$have_udisks2 = xno ; then
    AC_MSG_ERROR([have_udisks2 development libraries not found])
fi

AM_CONDITIONAL(HAVE_UDISKS2, test x$have_udisks2 = xyes)

AC_SUBST(UDISKS2_CFLAGS)
AC_SUBST(UDISKS2_LIBS)

PKG_CHECK_MODULES(GUDEV, gudev-1.0, have_gudev=yes, have_gudev=no)
if test x$have_gudev = xno ; then
    AC_MSG_ERROR([have_gudev development libraries not found])
fi

AM_CONDITIONAL(HAVE_GUDEV, test x$have_gudev = xyes)

AC_SUBST(GUDEV_CFLAGS)
AC_SUBST(GUDEV_LIBS)

# systemd
AC_ARG_WITH([systemdsystemunitdir],
            AS_HELP_STRING([--with-systemdsystemunitdir=DIR], [Directory for systemd service files]),
            [],
            [with_systemdsystemunitdir=$($PKG_CONFIG --variable=systemdsystemunitdir systemd)])
if test "x$with_systemdsystemunitdir" != "xno"; then
  AC_SUBST([systemdsystemunitdir], [$with_systemdsystemunitdir])
fi
AM_CONDITIONAL(HAVE_SYSTEMD, [test -n "$systemdsystemunitdir"])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_OUTPUT([Makefile
src/Makefile
data/Makefile])

echo "
               ueventmonitor $VERSION
             ================

        prefix:                     ${prefix}
        libdir:                     ${libdir}
        libexecdir:                 ${libexecdir}
        exec_prefix:             $(exec_prefix)
        bindir:                     ${bindir}
        sbindir:                    ${sbindir}
        datadir:                    ${datadir}
        sysconfdir:                 ${sysconfdir}
        localstatedir:              ${localstatedir}
        docdir:                     ${docdir}
        introspection:              ${found_introspection}

        udevdir:                    ${udevdir}
        systemdsystemunitdir:       ${systemdsystemunitdir}
        using libsystemd-login:     ${have_libsystemd_login}
        using libelogind:           ${have_libelogind}
        use /media for mounting:    ${fhs_media}
        acl support:                ${have_acl}
        libblockdev_part support:   ${have_libblockdev_part}

        compiler:                   ${CC}
        cflags:                     ${CFLAGS}
        cppflags:                   ${CPPFLAGS}

        Maintainer mode:            ${USE_MAINTAINER_MODE}
        Building api docs:          ${enable_gtk_doc}
        Building man pages:         ${enable_man}

        Enable modules:             ${enable_modules_info}

        BTRFS module:               ${have_btrfs}
        iSCSI module:               ${have_iscsi}${have_libiscsi_session_info_msg}
        LVM2  module:               ${have_lvm2}
        LVMCache:                   ${have_lvmcache}
        Zram module:                ${have_zram}
        LibStorageMgmt module:      ${have_lsm}
        Bcache module:              ${have_bcache}
"