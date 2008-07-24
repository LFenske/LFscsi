#!/usr/bin/perl -w
# -*- mode: perl -*-
# $Header$

#push(@INC, "/tmp");
#push(@INC, "/usr/lib/perl");
#push(@INC, "/usr/local/lib/perl");
#push(@INC, "/usr/ih/bin");
#push(@INC, "/usr/local/bin");
#unshift(@INC, "/manuf/manuf/bin");

use SCSImod;

$use_children = 1;
$sockaddr = "S n a4 x8";
$AF_INET = 2;
$SOCK_STREAM = 2;
$port = 39074;

# to make -w happy
$name = "";
$alias = "";
$addr = "";

($name, $alias, $proto) = getprotobyname('tcp');
$this = pack($sockaddr, $AF_INET, $port, "\0\0\0\0");

socket(S, $AF_INET, $SOCK_STREAM, $proto) || die "socket: $!";
bind  (S, $this) || die "bind: $!";
listen(S,50) || die "listen: $!";

select((select(S), $| = 1)[0]);

$fd=$port;
for (;;) {
    $fd++;
    ($addr = accept($fd, S)) || die $!;
    if ($use_children) {
	if ($pid = fork()) {
	    # parent
	    close($fd);
	} elsif (defined $pid) {
	    # child
	    &one_connection($fd);
	    exit(0);
	} else {
	    die "Can't fork: $!\n";
	}
    } else {
	&one_connection($fd);
    }
}


sub one_connection {
    my($fd) = @_;
    my($devlen, $dev, $rw, $cdblen, $cdb, $datlen, $dat, $sttlen, $timout) =
	("","","","","","","","","");
    select((select($fd), $| = 1)[0]);
    for (;;) {
	&myread($fd, $devlen, 1)   || last; $devlen = unpack("C", $devlen);
	&myread($fd, $dev, $devlen)|| last;
	&myread($fd, $rw, 1)       || last;
	&myread($fd, $cdblen, 1)   || last; $cdblen = unpack("C", $cdblen);
	&myread($fd, $cdb, $cdblen)|| last;
	&myread($fd, $datlen, 4)   || last; $datlen = unpack("N", $datlen);
	if ($rw eq "w") {
	    &myread($fd, $dat, $datlen)|| last;
	}
	&myread($fd, $sttlen, 4)   || last; $sttlen = unpack("N", $sttlen);
	&myread($fd, $timout, 4)   || last; $timout = unpack("N", $timout);
	$status = SCSImod::send_cdb(
				    $dev,
				    $rw,
				    $cdb,
				    $dat, $datlen,
				    $sttlen ? $stt : undef, $sttlen,
				    $timout,
				    );
	$sttlen = length($stt);
	if ($rw ne "r") { $dat = ""; }
	$datlen = length($dat);
	print $fd
	    pack("C", $status),
	    pack("N", $datlen), $datlen ? $dat : "",
	    pack("N", $sttlen), $stt;
    }
    close($fd);
}


sub myread {
    # this "my" is translated into "local" for perl4 on suns
    # this illustrates the problem with "local": I can't use "datlen" here
    # because I'm writing into it as $_[$datref]
    my($fd, $mydat, $mydatlen) = @_;
    my($datref) = (1);
    my($retval);
    $_[$datref] = $mydat = "";
    while (length($_[$datref]) < $mydatlen) {
	$retval = read($fd, $_[$datref], $mydatlen-length($_[$datref]), length($_[$datref]));
	last if (!defined($retval) || $retval == 0);
    }
    return defined $retval ? length($_[$datref]) : $retval;
}

