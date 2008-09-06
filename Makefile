all clean clobber scratch depend deliver::
	for dir in ScsiCmd ScsiServer ScsiTransport; do $(MAKE) -C $$dir $@; done

deliver::
	mkdir -p deliverables
	cp -p ScsiCmd/libscsicmd.a deliverables/libscsicmd.a
	$(AR) r deliverables/libscsicmd.a ScsiTransport/scsitransport.o

clobber::
	rm -rf deliverables

