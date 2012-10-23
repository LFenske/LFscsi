PKGNAME = LFscsi
VERSION = 0.9.0

all clean clobber scratch depend deliver::
	for dir in ScsiCmd ScsiServer ScsiTransport; do $(MAKE) -C $$dir $@; done

deliver::
	mkdir -p deliverables/bin
	mkdir -p deliverables/lib
	mkdir -p deliverables/include
	$(MAKE) -C ScsiCmd libscsicmd.a
	$(MAKE) -C ScsiTransport scsitransport.o
	cp -p ScsiCmd/libscsicmd.a deliverables/lib/
	$(AR) r deliverables/lib/libscsicmd.a ScsiTransport/scsitransport.o

clobber::
	rm -rf deliverables LFscsi.spec

RPMDIR = /nishome/lfenske/rpmbuild
TMPDIR = /tmp
PKGREV = $(PKGNAME)-$(VERSION)
DES    = $(TMPDIR)/$(PKGREV)
rpm:	$(PKGNAME).spec
	rm -rf "$(DES)"
	cp -a . "$(DES)"
	cd "$(DES)"; make clobber; find . -name CVS | xargs rm -rf
	tar czvf "$(RPMDIR)"/SOURCES/"$(PKGREV)".tar.gz -C "$(TMPDIR)" "$(PKGREV)" && rm -rf "$(DES)"
	rpmbuild -ba --rmsource --clean $(PKGNAME).spec

$(PKGNAME).spec:	Makefile clobber deliver
	{	\
	echo Name: $(PKGNAME);	\
	echo Version: $(VERSION);	\
	echo Release: 1;	\
	echo Summary: low-level SCSI tools;	\
	echo %description;	\
	echo Low-level SCSI tools to send CDBs to SCSI devices.;	\
	echo ;	\
	echo Copyright: GPL;	\
	echo Vendor: Larry Fenske;	\
	echo Group: Applications/System;	\
	echo Packager: 'Larry Fenske <LFscsi@towanda.com>';	\
	echo ExclusiveOS: Linux cygwin
	echo Prefix: /usr/local;	\
	echo Source: $(PKGNAME)-$(VERSION).tar.gz	echo Buildroot: /tmp/$(PKGNAME);	\
	echo ;	\
	echo %prep;	\
	echo %setup -q;	\
	echo ;	\
	echo %build;	\
	echo make deliver;	\
	echo ;	\
	echo %install;	\
	echo 'rm -rf   $$RPM_BUILD_ROOT';	\
	echo 'mkdir -p $$RPM_BUILD_ROOT/usr/local/';	\
	echo 'cp -pa deliverables/* $$RPM_BUILD_ROOT/usr/local/';	\
	echo ;	\
	echo %clean;	\
	echo 'rm -rf $$RPM_BUILD_ROOT';	\
	echo ;	\
	echo %files;	\
	echo %defattr\(-,root,root\);	\
	cd deliverables; find . -type f | sed s@^.@/usr/local@;	\
	echo;	\
	} > $@

