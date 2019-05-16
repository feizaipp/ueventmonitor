Name:		ueventmonitor
Version:	1.0
Release:	1%{?dist}
Summary:	listen usb block device hot plug

License:      GPL-2.0
Source0:     %{name}-%{version}.tar.gz

BuildRequires: dbus-glib-devel
BuildRequires: libudisks2-devel
BuildRequires: libgudev1-devel

Requires: udisks2
Requires: dbus-glib
Requires: libgudev1

%description
listen usb block device hot plug

%prep
%setup -q

%build
./autogen.sh
./configure --prefix=/usr --exec-prefix=/usr --bindir=/usr/bin \
--sbindir=/usr/sbin --sysconfdir=/etc --datadir=/usr/share \
--includedir=/usr/include --libdir=/usr/lib64 --libexecdir=/usr/libexec \
--localstatedir=/var --sharedstatedir=/var/lib --mandir=/usr/share/man \
--infodir=/usr/share/info
make

%install
[ "${RPM_BUILD_ROOT}" != "/" ] && [ -d ${RPM_BUILD_ROOT} ] && rm -rf ${RPM_BUILD_ROOT};
make install DESTDIR=$RPM_BUILD_ROOT

%clean
[ "${RPM_BUILD_ROOT}" != "/" ] && [ -d ${RPM_BUILD_ROOT} ] && rm -rf ${RPM_BUILD_ROOT};

%post
%systemd_post ueventmonitor.service
systemctl enable ueventmonitor.service >/dev/null 2>&1

%files
%dir %{_libexecdir}/ueventmonitor
%{_libexecdir}/ueventmonitor/ueventmonitord

%{_sysconfdir}/dbus-1/system.d/org.freedesktop.UeventMonitor.conf
%{_unitdir}/ueventmonitor.service
%{_datadir}/dbus-1/system-services/org.freedesktop.UeventMonitor.service

%changelog
* Mon Oct 29 2018 zpehome <zpehome@yeah.net> - 1.0-1
- Initial version of the package
