#perl -e 'use lib "./blib/arch"; use SCSIRPC; print STDERR "result = ", SCSIRPC::send_cdb("turkey:/dev/sga", "r", pack("C6",0x1a,0x08,0x3f,0,32,0), $dat,32, $stt,16, 5), "\n"; print $dat,$stt;' >/tmp/q; od -Ax -txC /tmp/q
perl -e 'use SCSIRPC; print STDERR "result = ", SCSIRPC::send_cdb("turkey:/dev/sga", "r", pack("C6",0x1a,0x08,0x3f,0,32,0), $dat,32, $stt,16, 5), "\n"; print $dat,$stt;' >/tmp/q; od -Ax -txC /tmp/q
