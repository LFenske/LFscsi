VERSION=0.9.0

all clean clobber scratch depend deliver::
	for dir in ScsiCmd ScsiServer ScsiTransport; do $(MAKE) -C $$dir $@; done

deliver::
	mkdir -p deliverables
	$(MAKE) -C ScsiCmd libscsicmd.a
	$(MAKE) -C ScsiTransport scsitransport.o
	cp -p ScsiCmd/libscsicmd.a deliverables/libscsicmd.a
	$(AR) r deliverables/libscsicmd.a ScsiTransport/scsitransport.o

clobber::
	rm -rf deliverables

RPMDIR = /nishome/lfenske/rpmbuild
TMPDIR = /tmp
PKGREV = LFscsi-$(VERSION)
DES    = $(TMPDIR)/$(PKGREV)
rpm:
	rm -rf "$(DES)"
	cp -a . "$(DES)"
	cd "$(DES)"; make clobber; find . -name CVS | xargs rm -rf
	tar czvf "$(RPMDIR)"/SOURCES/"$(PKGREV)".tar.gz -C "$(TMPDIR)" "$(PKGREV)" && rm -rf "$(DES)"
	rpmbuild -ba --rmsource --clean LFscsi.spec

