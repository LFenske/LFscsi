all clean clobber scratch depend:
	for dir in ScsiCmd ScsiServer ScsiTransport; do $(MAKE) -C $$dir $@; done

