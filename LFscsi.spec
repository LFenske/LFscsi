License: GPL
Summary: low-level SCSI tools
%define version 0.9.0
Name: LFscsi
Prefix: /usr
Provides: LFscsi
Release: 1
Source: LFscsi-%{version}.tar.gz
Version: %{version}
Buildroot: /tmp/LFSCSI
%description
Low-level SCSI tools can send CDBs to SCSI devices.

%prep
%setup -q

%build
make deliver

%install
rm -rf   $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/local/bin
mkdir -p $RPM_BUILD_ROOT/usr/local/lib
mkdir -p $RPM_BUILD_ROOT/usr/local/include
cp -p deliverables/scsi deliverables/Print* $RPM_BUILD_ROOT/usr/local/bin/
cp -p deliverables/libscsicmd.a             $RPM_BUILD_ROOT/usr/local/lib/
cp -p deliverables/*.h                      $RPM_BUILD_ROOT/usr/local/include/

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
/usr/local/bin/scsi
/usr/local/bin/PrintDefault
/usr/local/bin/PrintInquiry
/usr/local/bin/PrintModeSense
/usr/local/bin/PrintReadCapacity
/usr/local/bin/PrintReadPosition
/usr/local/bin/PrintRequestSense
/usr/local/bin/PrintReportTargetPortGroups
/usr/local/lib/libscsicmd.a
/usr/local/include/common.h
/usr/local/include/scsicmd.h
/usr/local/include/ScsiTransport.h
/usr/local/include/send_cdb.h

