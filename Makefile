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

