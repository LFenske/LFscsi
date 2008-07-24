#!/usr/bin/perl -w
# -*- mode: perl -*-
# $Header$

# uses $device, &printrequestsense_stub, $nflag, $verbose, $noRS, $LUN, $size, $timeout

$timeout_default=30;

$mtbase = 0x0350; $mtcount =   1;
$stbase = 0x0000; $stcount = 118;
$iebase = 0x0314; $iecount =  12;
$dtbase = 0x0320; $dtcount =   4;
$bases_fresh = 0;	# 0 for generality, 1 to always use values set above

$nosize = -1;

if (!defined($device)) {
    if (($device=$ENV{"DEVICE"}) eq "") {
	die "env variable 'DEVICE' must be set.\naborting\n";
    }
}

%callbacks = ();


#  8.2.01 - void change_definition($dat, $definition_parameter) $size, $save
#           void eject()
#           void erase($long)
# 17.2.01 - void exchange_medium($source_address, $first_destination_address, $second_destination_address) $trans, $inv, $inv2
#           void format()
# 17.2.02 - void initialize_element_status()
#         - void initialize_element_status_with_range($range, $element_address, $number_of_elements)
#  8.2.05 - $dat inquiry($evpd, $page_code) $size
#           void load($loadflag) $immed
## 8.2.06 -      log_select($dat, $pcr) $save, $page_code
#  8.2.07 - $dat log_sense($ppc, $page_code) $size, $pc, $pp
## 8.2.08 -      mode_select($dat, $pf) $size, $save
## 8.2.09 -      mode_select_10($dat, $pf) $size, $save
#- 8.2.10 - $dat mode_sense($page_code) $size, $pc [$dbd]
## 8.2.11 - $dat mode_sense_10($page_code) $size, $pc, $dbd
# 17.2.03 - void move_medium($source_address, $destination_address) $trans, $inv
# 17.2.04 - void position_to_element($destination_element_address) $trans, $inv
#  9.2.04 - void prevent_medium_removal($prevent)
#           $dat read($block, $length)
#           $dat read_block_limits()
#  8.2.12 - $dat read_buffer() $size, $pp, $mode, $bufid
#           $dat read_capacity()
#           $dat read_cdda($blk, $len) $size
# 17.2.05 - $dat read_element_status($starting_element_address, $number_of_elements) $size, $voltag, $eltype, $dvcid
#  8.2.13 - $dat receive_diagnostic_results() $size
# 17.2.06 - void release($3rdPty, $third_party_device_id, $element, $reservation_identification)
#  8.2.14 - $dat request_sense() $size
# 17.2.07 - $dat request_volume_element_address($element_address, $number_of_elements) $size, $voltag, $eltype
# 17.2.08 - void reserve($dat, $3rdPty, $third_party_device_id, $element, $reservation_identification) $size
#  9.2.13 - void rezero_unit()
#  8.2.15 - void send_diagnostic($dat, $PF, $SelfTest, $devOfL, $UnitOfL)
# 17.2.09 - void send_volume_tag($dat, $eladdr, $send_action_code) $size, $eltype
#           void set_capacity($numerator)
#           void space6($code, $count)
#           void start_stop_unit($start)
#  8.2.16 - void test_unit_ready()
#           void verify($fix, $len) $immed
#           void unload($loadflag) $immed
#           void write($dat, $blk, $len)
#  8.2.17 - void write_buffer($dat) $size, $pp, $mode, $bufid
# 10.2.15 - void write_filemarks($num) $immed

#           void locate($block_address) $immed
#           void erase($long) $immed


sub unimplemented {
    print "function is currently unimplemented\n";
}

sub todec {
    $_ = $_[0];
    $_ = oct if /^0/;
    return $_;
}

sub xlate {
    $_ = $_[0];
    if (/^st/i) { &get_bases; return ($stbase + &todec(substr($_[0],2))); }
    if (/^dt/i) { &get_bases; return ($dtbase + &todec(substr($_[0],2))); }
    if (/^ie/i) { &get_bases; return ($iebase + &todec(substr($_[0],2))); }
    if (/^mt/i) { &get_bases; return ($mtbase + &todec(substr($_[0],2))); }
    return &todec($_[0]);
}

sub checkargs {
    local($i);
    for ($i=1; ; $i++) {
	if (!defined($_[$i])) {
	    last;
	}
    }
    die "expected $_[0] arguments\n" if ($i-1 != $_[0]);
}

sub get_bases {
    local($dat, $datp, $q, $save_size);
    if ($bases_fresh) {
	return;
    }
    $save_size = $size;
    $size = $nosize;
    $dat = &mode_sense_smart(0x1d);
    $size = $save_size;
    $datp = 0;
    local($totlen, $medtype, $WPcache, $BDL) =
	unpack("CCCC", substr($dat,$datp));
    $datp += 4;
    $datp += $BDL;
#   local($rawtype, $len) = unpack("CC", substr($dat,$datp));
    $datp += 2;
    ($mtbase, $mtcount,
     $stbase, $stcount,
     $iebase, $iecount,
     $dtbase, $dtcount) =
	unpack("n8", substr($dat,$datp));
    $bases_fresh = 1;
}


sub myread_init {	# for print functions
    $myread = $_[0];
    $myreadp = 0;
}

sub myread {		# for print functions
    local($fd, *des, $len) = @_;
    $des = substr($myread, $myreadp, $len);
    $myreadp += $len;
    return (length($des) == $len);
}

sub lcd {
    local($line, $text) = @_;
    local($savesize) = $size;
    local($dat);
    $size = 88;
    $dat = &mode_sense(0x22);
    local($q,$b0,$b1,$b2,$b3,@l) = unpack("LCCCCA20A20A20A20", $dat);
    $b0 &= 0x3f;
    if ($line == 0) {
	$b2 |= 0x80;
	if (defined $text) {
	    $b2 |=  0x40;
	} else {
	    $b2 &= ~0x40;
	}
    } else {
	if (defined $text) {
	    $b2 |=   1 << (4-$line) ;
	    $l[$line-1] = $text;
	} else {
	    $b2 &= ~(1 << (4-$line));
	}
    }
    $dat = pack("LCCCCA20A20A20A20", 0,$b0,$b1,$b2,$b3,@l);
    &mode_select($dat, 1);
}


sub clean_wait {
    &scsi_rs_register_callback("main'clean_wait_retry",  2, 0x30, 0x03);
    &scsi_rs_register_callback("main'clean_wait_ignore", 2, 0x04, 0x03);
    &scsi_rs_register_callback("main'clean_wait_ignore", 2, 0x3a, 0x00);
    &test_unit_ready();
    &scsi_rs_unregister_callback(                        2, 0x3a, 0x00);
    &scsi_rs_unregister_callback(                        2, 0x04, 0x03);
    &scsi_rs_unregister_callback(                        2, 0x30, 0x03);
}

sub clean_wait_retry {
    local($key,$asc,$ascq) = @_;
    sleep 1;
    return -1;	# redo the command
}

sub clean_wait_ignore {
    local($key,$asc,$ascq) = @_;
    return 1;	# all done
}


sub cb_format {
    local($device, @keys) = @_;
    # assumes well-behaved input
    if    (@keys == 1) { return sprintf("%s:%x",           $device, @keys); }
    elsif (@keys == 2) { return sprintf("%s:%.2x/%.2x",    $device, @keys); }
    elsif (@keys == 3) { return sprintf("%s:%x/%.2x/%.2x", $device, @keys); }
}


sub scsi_rs_register_callback {
    local($cb, @keys) = @_;
    local($format) = &cb_format($device, @keys);
    $callbacks{$format} = join(' ', $cb, split(' ', $callbacks{$format}));
}


sub scsi_rs_unregister_callback {
    local(@keys) = @_;
    local($format) = &cb_format($device, @keys);
    local(@cbs) = grep($_ != $format, split(' ', $callbacks{$format}));
    if (@cbs) {
	$callbacks{$format} = join(' ', @cbs);
    } else {
	undef $callbacks{$format};
    }
}


# sets $sense, $sense_key, $sense_code, $sense_codeq, $status
# uses $noRS, $verbose, $timeout_default, $LUN, $nflag, &printrequestsense_stub, &SCSImod::send_cdb, &SCSIRPC::send_cdb, GNU od
# -datain  $dat  (modifies $dat)
# -datalen $len  (maximum length of incoming $dat)
# -dataout $dat
# -timeout $time
# -cdb     $cdb  (also set without "-cdb")
# returns return value from SCSImod::send_cdb or SCSIRPC::send_cdb, should be SCSI status from command
# prints request sense data if any comes back from SCSImod::send_cdb/SCSIRPC::send_cdb and undefs $dat if it's datain
sub send_cdb {
    my($cdb    ) = undef;
    my($rw     ) = "r";
    my($datref ) = undef;
    my($datlen ) = 0;
    my($localTO) = $timeout_default;
    for ($i=0; $i < @_; $i++) {
	for ($_[$i]) {
	    /^-datain$/  && do {
		$rw = "r";
		$datref = ++$i;
		$_[$datref] = "";
		last;
	    };
	    /^-datalen$/ && do {
		$datlen = $_[++$i];
		last;
	    };
	    /^-dataout$/ && do {
		$rw = "w";
		$datref = ++$i;
		$datlen = length($_[$datref]);
		last;
	    };
	    /^-timeout$/ && do {
		$localTO = $_[++$i];
		last;
	    };
	    /^-cdb$/ && do {
		$cdb = $_[++$i];
		last;
	    };
	    $cdb = $_;
	}
    }
    if (!defined $cdb) {
	die "No CDB supplied.  Aborting.\n";
    }

    $localTO = $timeout
	if (defined $timeout);
    my(@cdblist,$i);
    # insert LUN
    @cdblist = unpack("C*", $cdb);
    $cdblist[1] |= $LUN << 5;
    $cdb = pack("C*", @cdblist);
    #
    local($handled) = 1;	# everything's OK now before we start
    do {	# maybe repeat command if check condition and callback says so
	if ($nflag || $verbose >= 2) {
	    print "SCSI???::send_cdb($device, $rw, (";
	    foreach $i (unpack("C*", $cdb)) { printf "0x%.2x,", $i; }
	    print "), dat, $datlen, stt, 0, $localTO)\n";
	    if (defined($datref) && length($_[$datref]) && $rw eq "w") {
		print "data out:";
		open(OUT, "|od -txC");
		print OUT $_[$datref];
		close OUT;
	    }
	}
	if (!$nflag) {
	    $handled = 1;
	    $status = &local_or_remote_send_cdb(
		$device,
		$rw,
		$cdb,
		$_[$datref], $datlen,
		$noRS ? undef : $sense, $noRS ? 0 : 18,
		$localTO
		);
	    print "status: $status\n"       if ($verbose >= 2);
            if (($status == 2) && !$noRS && (length($sense) == 0)) {
                local($size) = $nosize;
                $sense = &request_sense();
            }
	    $sense_key   = 0;
	    $sense_code  = 0;
	    $sense_codeq = 0;
	    if (defined($sense) && length($sense)) {
		local($key  ) = ord(substr($sense,  2)) & 0x0f;
		local($code ) = ord(substr($sense, 12));
		local($codeq) = ord(substr($sense, 13));
		local($cb_list, $cb1);
		$handled = 0;	# something went wrong
		for $cb_list (
			      $callbacks{&cb_format($device, $key, $code, $codeq)},
			      $callbacks{&cb_format($device, $code, $codeq)},
			      $callbacks{&cb_format($device, $key)},
			      ) {
		    if (defined $cb_list) {
			for $cb1 (split(' ',$cb_list)) {
			    if (!$handled) {
				$handled = do $cb1($key, $code, $codeq);
				# 1: the callback resolved the problem, all done
				# 0: the callback did nothing, try more
				#-1: the callback resolved it, redo command
			    }
			}
		    }
		}
		if (!$handled) {
		    if ($verbose >= 1) {
			print "device = $device\n";
			&printrequestsense_stub($sense);
		    }
		    $sense_key   = $key;
		    $sense_code  = $code;
		    $sense_codeq = $codeq;
		    printf "key/ASC/ASCQ = %x/%.2x/%.2x\n", $key, $code, $codeq;
		    undef $_[$datref] if ($rw eq "r");
		    if ($interactive) {
			print "check condition status\n";
		    } else {
			#die "check condition status\n";
			print "check condition status\n";
		    }
		} else {
		    $status = 0;
		}
	    }
	}
    } while ($handled == -1);	# -1 means handled, but repeat
    if ($verbose >= 3 && defined($datref) && length($_[$datref]) && $rw eq "r") {
	print "data in:\n";
	open(OUT, "|od -txC");
	print OUT $dat;
	close OUT;
    }
    return $status;
}

sub local_or_remote_send_cdb {
    my($device, $rw, $cdb, $dat, $datlen, $stt, $sttlen, $timeout) = @_;
    my($datref, $sttref) = (3, 5);
    my(@devs) = split(':', $device);
    if (@devs == 1) { @devs = ("localhost", "local", $devs[0]); }
    if (@devs == 2) { @devs = ($devs[0], "socket", $devs[1]); }
    my($rem, $protocol, $dev) = @devs;
    if ($protocol eq "wasabi") {
        require "ScsiTarget.ph";
        open FH, '+<', $dev || die "cannot open device file '$dev': $!";
        my $val = "";
        # TDEVENT
        #   int td_instance
        $val .= pack("V", 0);
        #   DTLUNID td_lun
        #     u_int lun_number
        $val .= pack("V", 0);
        #   DTEVENT event
        #     DTEVENT_TYPE event_type
        $val .= pack("V", DTEVENT_COMMAND);
        #     DTHOSTCMDID host
        #       u32 host_id
        $val .= pack("V", 3);   # arbitrary
        #       u32 host_tag_valid
        $val .= pack("V", 0);
        #       u32 host_tag
        $val .= pack("V", 0);
        #     u32 cdb_len
        $val .= pack("V", length($cdb));
        #     u8 cdb[MAX_COMMAND_LENGTH]
        $val .= pack("a".&MAX_COMMAND_LENGTH, $cdb);
        #     iovec data
        #       void *iov_base
        $tmp  = ' ' x (0);
        $val .= pack("P", $tmp);
        #       size_t iov_len
        $val .= pack("V", length($tmp));
        #   iovec data
        #     void *iov_base
        $dat .= ' ' x ($datlen - length($dat));
        $val .= pack("P", $dat);
        #     size_t iov_len
        $val .= pack("V", $datlen);
        #   DTCMDSTATUS status
        #     DTHOSTCMDID host
        #       u32 host_id
        $val .= pack("V", 3);   # arbitrary
        #       u32 host_tag_valid
        $val .= pack("V", 0);
        #       u32 host_tag
        $val .= pack("V", 0);
        #     DSTATBUF status_buffer
        #       u32 cmd_status
        $val .= pack("V", 0);
        #       u32 write_buffer_valid
        $val .= pack("V", 0);
        #       iovec next_write_buffer
        #         void *iov_base
        $stt = ' ' x $sttlen;
        $val .= pack("P", $stt);
        #         size_t iov_len
        $val .= pack("V", $sttlen);

        open OD, "|od -txC" || die "cannot open 'od' program: $!";
        print OD $val;
        close OD;
        my $result = ioctl(FH, &TDINJECTEVENT | (length($val) << 16), $val) || -1;
        print "ioctl result = $result\n";
        #die "stub";
    } elsif ($protocol eq "socket") {
	local($sock) = $socks{$rem};
	if (!defined $sock) {
	    my($sockaddr) = "S n a4 x8";
	    my($AF_INET) = 2;
	    my($SOCK_STREAM) = 1;
	    my($port) = 39074;
	    my($name, $alias, $proto) = getprotobyname('tcp');
	    my($hostname) = `hostname`;
	    chop($hostname);
	    my($name, $alias, $type, $len, $thisaddr) = gethostbyname($hostname);
	    my($name, $alias, $type, $len, $thataddr) = gethostbyname($rem);
	    my($this) = pack($sockaddr, $AF_INET,     0, $thisaddr);
	    my($that) = pack($sockaddr, $AF_INET, $port, $thataddr);
	    $sock = $socks{$rem} = $rem;
	    $sock = $rem;
	    socket ($sock, $AF_INET, $SOCK_STREAM, $proto)
		|| die "socket: $!";
	    bind   ($sock, $this)
		|| die "bind: $!";
	    connect($sock, $that)
		|| die "connect: $!";
	    select((select($sock), $| = 1)[0]);
	}
	if ($rw ne "w") { $dat = ""; }
	print $sock pack("C a*", length($dev), $dev);
	print $sock substr($rw,0,1);
	print $sock pack("C a*", length($cdb), $cdb);
	print $sock pack("N a*", $datlen, $dat);
	print $sock pack("N", $sttlen);
	print $sock pack("N", $timeout);
	$_[$sttref] = "";
	&sockread($sock, $status, 1); $status = unpack("C", $status);
	&sockread($sock, $datlen, 4); $datlen = unpack("N", $datlen);
	&sockread($sock, $_[$datref], $datlen) if $datlen;
	&sockread($sock, $sttlen, 4); $sttlen = unpack("N", $sttlen);
	&sockread($sock, $_[$sttref], $sttlen) if $sttlen;
        shutdown $sock, 2;
        close $sock;
        delete $socks{$rem};
    } elsif ($protocol eq "local") {
	require SCSImod;
	$status = SCSImod::send_cdb(
				    $dev,
				    $rw,
				    $cdb,
				    $_[$datref], $datlen,
				    $_[$sttref], $sttlen,
				    $timeout
				    );
    } elsif ($protocol eq "rpc1") {
	require SCSIRPC;
	$status = SCSIRPC::send_cdb(
				    "$rem:$dev",
				    $rw,
				    $cdb,
				    $_[$datref], $datlen,
				    $_[$sttref], $sttlen,
				    $timeout
				    );
    }
    return $status;
}

sub sockread {
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


sub cdb {
    &checkargs(1,@_);
    &send_cdb($_[0]);
}

sub cdbr {
    &checkargs(1,@_);
    local($thissize, $dat);
    $thissize = $size!=$nosize ? $size : 0xff;
    &send_cdb(
	      -cdb     => $_[0],
	      -datain  => $dat,
	      -datalen => $thissize,
	      );
    return $dat;
}

sub cdbw {
    &checkargs(2,@_);
    &send_cdb(
	      -cdb     => $_[0],
	      -dataout => $_[1],
	      );
}

sub change_definition {
    &checkargs(2,@_);
    local($thissize, $dat);
    $dat = $_[0];
    $thissize = $size!=$nosize ? $size : length($dat);
    &send_cdb(
	      pack("C10",0x40,0,$save,$_[1],0,0,0,0,$thissize,0),
	      -dataout => $dat,
	      );
}

sub eject {
    &checkargs(0,@_);
    &send_cdb(pack("C6",0x1b,0,0,0,2,0));
}

sub erase {
    &checkargs(1,@_);
    local($long) = $_[0];
    &send_cdb(pack("C6",
                   0x19,
                   ($immed << 1) | ($long << 0),
                   0,0,0,0));
}

sub exchange_medium {
    &checkargs(3,@_);
    &send_cdb(
	      pack("C2n4C2",
		   0xa6,
		   0,
		   &todec($trans),
		   &xlate($_[0]),
		   &xlate($_[1]),
		   &xlate($_[2]),
		   (&todec($inv2)<<1)|&todec($inv),
		   0),
	      );
}

sub format {
    &checkargs(0,@_);
    &send_cdb(
	      pack("C6",0x04,0,0,0,0,0),
	      -timeout => 60*60,
	      );
}

sub initialize_element_status {
    &checkargs(0,@_);
    &send_cdb(
	      pack("C6", 0x07, 0,0,0,0, $voltag<<7),
	      -timeout => 15*60,
	      );
}

sub initialize_element_status_with_range {
    &checkargs(3,@_);
    &send_cdb(
	      pack("C2nC2nC2",
		   0xe7,
		   &todec($_[0]),
		   &xlate($_[1]),
		   0,0,
		   &todec($_[2]),
		   0,
		   $voltag<<7),
	      -timeout => 3*60,
	      );
}

sub inquiry {
    &checkargs(2,@_);
    local($thissize, $dat);
    $thissize = $size!=$nosize ? $size : 0xff;
    &send_cdb(
	      pack("C6",0x12, &todec($_[0]), &todec($_[1]), 0, $thissize, 0),
	      -datain  => $dat,
	      -datalen => $thissize,
	      -timeout => 5,
	      );
    return $dat;
}

sub inquiry_smart {
    &checkargs(2,@_);
    local($thissize, $dat);
    local($save_size) = ($size);
    local($retval);
    local($evpd) = (&todec($_[0]) & 1);
    if ($size!=$nosize) {
	$thissize = $size;
    } else {
	if ($evpd) {
	    $size = 4;
	    $dat = &inquiry($evpd, $_[1]);
	    return $dat if ($nflag);
	    ($q,$q,$q,$size) = unpack("CCCC", $dat);
	    $size += 4;
	} else {
	    $size = 8;
	    $dat = &inquiry($evpd, $_[1]);
	    return $dat if ($nflag);
	    return $dat if (length($dat) < 5);
	    ($q,$size) = unpack("NC", $dat);
	    $size += 5;
	}
    }
    $retval = &inquiry($evpd, $_[1]);
    $size = $save_size;
    return $retval;
}

sub load {
    &checkargs(1,@_);
    local($begtime, $endtime);
    $begtime = time;
    &send_cdb(
	      pack("C6",0x1b,$immed,0,0,$_[0],0),
	      -timeout => 10*60,
	      );
    $endtime = time;
    if ($_[0] == 0) {
	local($s,$m,$h,$D,$M,$Y) = localtime($begtime);
	open(L, ">>unload.log");
	printf L "19%.2d%.2d%.2d %.2d:%.2d:%.2d unload %s took %d seconds\n", $Y, $M+1, $D, $h, $m, $s, $device, $endtime-$begtime;
	close(L);
    }
}

sub locate {
    &checkargs(1,@_);
    &send_cdb(
              pack("C3NC3",
                   0x2b,
                   $immed,
                   0,
                   &todec($_[0]),
                   0,
                   0,
                   0),
              );
}

sub log_sense {
    &checkargs(2,@_);
    local($thissize, $dat);
    $thissize = $size!=$nosize ? $size : 0xff;
    &send_cdb(
	      pack("C3n3C",
		   0x4d,
		   (&todec($_[0])<<1)|$save,
		   ($pc<<6)|&todec($_[1]),
		   0,
		   $pp,
		   $thissize,
		   0),
	      -datain  => $dat,
	      -datalen => $thissize,
	      );
    return $dat;
}

sub log_sense_smart {
    &checkargs(2,@_);
    local($thissize, $dat);
    local($save_size) = ($size);
    local($retval);
    if ($size!=$nosize) {
	$thissize = $size;
    } else {
	$size = 8;
	$dat = &log_sense(@_);
	return $dat if ($nflag);
	local($pagecode, $res, $totlen) = unpack("CCn", $dat);
	$size = $totlen+4;
    }
    $retval = &log_sense(@_);
    $size = $save_size;
    return $retval;
}

sub mode_select {
    &checkargs(2,@_);
    local($thissize, $dat);
    $dat = $_[0];
    $thissize = $size!=$nosize ? $size : length($dat);
    &send_cdb(
	      pack("C6", 0x15, ($_[1]<<4)+$save, 0, 0, $thissize, 0),
	      -dataout => $dat,
	      );
}

sub mode_sense {
    &checkargs(1,@_);
    local($thissize, $dat);
    $thissize = $size!=$nosize ? $size : 0xff;
    &send_cdb(
	      pack("C6", 0x1a, 0x00, ($pc<<6)|&todec($_[0]), 0, $thissize, 0),
	      -datain  => $dat,
	      -datalen => $thissize,
	      );
    return $dat;
}

sub mode_sense_smart {
    &checkargs(1,@_);
    local($thissize, $dat);
    local($save_size) = ($size);
    local($retval);
    if ($size!=$nosize) {
	$thissize = $size;
    } else {
	$size = 8;
	$dat = &mode_sense(@_);
	return $dat if ($nflag);
	local($totlen, $medtype, $WPcache, $BDL) = unpack("CCCC", $dat);
	$size = $totlen+1
	    if ($size < 1000);
    }
    $retval = &mode_sense(@_);
    $size = $save_size;
    return $retval;
}

sub move_medium {
    &checkargs(2,@_);
    &send_cdb(
	      pack("C2n4C2",
		   0xa5,
		   0,
		   &xlate($trans),
		   &xlate($_[0]),
		   &xlate($_[1]),
		   0,
		   $inv,
		   0),
	      -timeout => 10*60,
	      );
}

sub position_to_element {
    &checkargs(1,@_);
    &send_cdb(
	      pack("C2n3C2",
		   0x2b,
		   0,
		   &xlate($trans),
		   &xlate($_[0]),
		   0,
		   $inv,
		   0),
	      );
}

sub prevent_medium_removal {
    &checkargs(1,@_);
    &send_cdb(pack("C6",0x1e,0,0,0,&todec($_[0]),0));
}

sub read6 {
    &checkargs(2,@_);
    local($thissize, $dat);
    $thissize = $size!=$nosize ? $size : $_[1];
    ($len3,$len2,$len1,$len0) = unpack("C4", pack("N", $_[1]));
    $dat = "";
    &send_cdb(
	      pack("C6",
		   0x08,
		   $_[0],
		   $len2,$len1, $len0,
		   0),
	      -datain  => $dat,
	      -datalen => $thissize,
	      );
    return $dat;
}

sub read {
    &checkargs(2,@_);
    local($thissize, $dat);
    $thissize = $size!=$nosize ? $size : 0x10000;
    ($blk3,$blk2,$blk1,$blk0) = unpack("C4", pack("N", $_[0]));
    (            $len1,$len0) = unpack("C2", pack("n", $_[1]));
    $dat = "";
    &send_cdb(
	      pack("C10",
		   0x28,
		   0,
		   $blk3, $blk2, $blk1, $blk0,
		   0,
		   $len1, $len0,
		   0),
	      -datain  => $dat,
	      -datalen => $thissize,
	      );
    return $dat;
}

sub read_block_limits {
    &checkargs(0,@_);
    &send_cdb(
	      pack("C6",0x05,0,0,0,0,0),
	      -datain  => $dat,
	      -datalen => 6,
	      );
    return $dat;
}

sub read_buffer {
    &checkargs(0,@_);
    local($thissize, $dat);
    $thissize = $size!=$nosize ? $size : 0x7fff;
    ( $pp3, $pp2, $pp1, $pp0) = unpack("C4", pack("N", $pp));
    ($len3,$len2,$len1,$len0) = unpack("C4", pack("N", $thissize));
    $dat = "";
    &send_cdb(
	      pack("C10",
		   0x3c,
		   $mode,
		   $bufid,
		   $pp2, $pp1, $pp0,
		   $len2, $len1, $len0,
		   0),
	      -datain  => $dat,
	      -datalen => $thissize,
	      -timeout => 1*60,
	      );
    return $dat;
}

sub read_capacity {
    &checkargs(0,@_);
    &send_cdb(
	      pack("C10",0x25,0,0,0,0,0,0,0,0,0),
	      -datain  => $dat,
	      -datalen => 8,
	      );
    return $dat;
}

sub read_cdda {
    &checkargs(2,@_);
    ($blk3,$blk2,$blk1,$blk0) = unpack("C4", pack("N", $_[0]));
    (            $len1,$len0) = unpack("C2", pack("n", $_[1]));
    $dat = "";
    &send_cdb(
	      pack("C12",
		   0xd8,
		   0,
		   $blk3, $blk2, $blk1, $blk0,
		   0, 0, $len1, $len0,
		   0, 0),
	      -datain  => $dat,
	      -datalen => $size!=$nosize ? $size : 0x7f00,
	      );
    return $dat;
}

sub read_element_status {
    &checkargs(2,@_);
    local($thissize, $dat, $smart);
    if ($size!=$nosize) {
	$smart = 0;
	$thissize = $size;
    } else {
	$smart = 1;
	&send_cdb(
		  pack("CCnnCCnn",
		       0xb8,
		       ($voltag<<4)|$eltype,
		       &xlate($_[0]),
		       &todec($_[1]),
		       $dvcid,
		       0, 8,
		       0),
		  -datain  => $dat,
		  -datalen => 8,
		  );
	return $dat if ($nflag);
	local($firstelem, $numelems, $totlen) = unpack("nnN", $dat);
	$thissize = $totlen+8;
	$thissize = 0x7fff if $thissize > 0x7fff;
    }
    if ($thissize>=100000) { # sanity check
	printf "read_element_status requested outrageous data size = %d (0x%.8x)\n", $thissize, $thissize;
	return "";
    }
    &send_cdb(
	      pack("CCnnCCnn",
		   0xb8,
		   ($voltag<<4)|$eltype,
		   &xlate($_[0]),
		   &todec($_[1]),
		   $dvcid,
		   ($thissize >> 16) & 0xff,
		   ($thissize >>  0) & 0xffff,
		   0),
	      -datain  => $dat,
	      -datalen => $thissize,
	      -timeout => 2*60,
	      );
    if ($smart) {
	# may need to read it in pieces and reconstruct
	while (unpack("x4 N", $dat) + 8 > length($dat)) {
	    local($datp, $leneach, $numdesc, $lastelem, $newdat);
	    # decide where to start next
	    $datp = 8;	# skip result header
	    for (;;) {
		local($len);
		last if ($datp + 8 + $leneach > length($dat));
		($leneach, $len) = unpack("x$datp x2 nN", $dat);
		last if ($datp + 8 + $len > length($dat));
		$datp += 8 + $len;	# skip this entire page
	    }
	    $datp += 8;	# skip page header
	    $numdesc = int((length($dat)-$datp) / $leneach);
	    if ($numdesc == 0) {
		$datp -= 8;	# backskip header
	    }
	    $datp += $leneach * ($numdesc - 1);	# point to last descriptor
	    $lastelem = unpack("x$datp n", $dat);	# so far
	    &send_cdb(
		      pack("CCnnCCnn",
			   0xb8,
			   ($voltag<<4)|$eltype,
			   $lastelem,
			   &todec($_[1]),
			   $dvcid,
			   ($thissize >> 16) & 0xff,
			   ($thissize >>  0) & 0xffff,
			   0),
		      -datain  => $newdat,
		      -datalen => $thissize,
		      -timeout => 2*60,
		      );
	    substr($dat, $datp) = substr($newdat, 16);	# skip new result header and page header
	}
	substr($dat, unpack("x4 N", $dat) + 8) = "";
    }
    return $dat;
}

sub read_position {
    &checkargs(0,@_);
    &send_cdb(
              pack("C10",
                   0x34,
                   0,
                   0,0,0,0,0,
                   0,0,
                   0),
              -datain => $dat,
              -datalen => 20,
              );
    return $dat;
}

sub receive_diagnostic_results {
    &checkargs(0,@_);
    local($thissize, $dat);
    if ($size!=$nosize) {
	$thissize = $size;
    } else {
	&send_cdb(
		  pack("C6",0x1c,0,0,0,4,0),
		  -datain  => $dat,
		  -datalen => 4,
		  );
	return $dat if ($nflag);
	($junk, $thissize) = unpack("nn", $dat);
	$thissize += 4;
    }
    if ($thissize>=100000) { # sanity check
	printf "receive_diagnostic_results requested outrageous data size = %d (0x%.8x)\n", $thissize, $thissize;
	return "";
    }
    &send_cdb(
	      pack("C6",0x1c,0,0,0,$thissize,0),
	      -datain  => $dat,
	      -datalen => $thissize,
	      );
    return $dat;
}

sub release {
    &checkargs(4,@_);
    &send_cdb(
	      pack("CCCnC",
		   0x17,
		   (&todec($_[0])<<4)|(&todec($_[1])<<1)|(&todec($_[2])),
		   &todec($_[3]),
		   0, 0),
	      );
}

sub request_sense {
    &checkargs(0,@_);
    local($thissize, $dat);
    $thissize = $size!=$nosize ? $size : 0x12;
    &send_cdb(
	      pack("C6", 0x03, 0,0,0,$thissize,0),
	      -datain  => $dat,
	      -datalen => $thissize,
	      -timeout => 5,
	      );
    return $dat;
}

sub request_volume_element_address {
    &checkargs(2,@_);
    local($thissize, $dat);
    if ($size!=$nosize) {
	$thissize = $size;
    } else {
# can't do this anymore; this clears the results
#	 &send_cdb(
#		   pack("C2n5",
#			0xb5,
#			($voltag<<4)|$eltype,
#			&todec($_[0]),
#			&todec($_[1]),
#			0,
#			8,
#			0),
#		   -datain  => $dat,
#		   -datalen => 8,
#		   );
#	 return $dat if ($nflag);
#	 local($firstelem, $numelems, $totlen) = unpack("nnN", $dat);
#	 $totlen &= 0xffffff;
#	 $thissize = $totlen+8;
	$thissize = 0x7fff;	# max needed = 8 + 3*8 + 0x351*0x38 = 0xb9d8
    }
    if ($thissize>=100000) { # sanity check
	printf "request_volume_element_address requested outrageous data size = %d (0x%.8x)\n", $thissize, $thissize;
	return "";
    }
    &send_cdb(
	      pack("C2n5",
		   0xb5,
		   ($voltag<<4)|$eltype,
		   &xlate($_[0]),
		   &todec($_[1]),
		   ($thissize >> 16) & 0xff,	# reserved byte and first alloc
		   ($thissize      ) & 0xffff,	# bottom 16 bits of alloc len
		   0),
	      -datain  => $dat,
	      -datalen => $thissize,
	      );
    return $dat;
}

sub rewind {
    &checkargs(0,@_);
    &send_cdb(
	      pack("C6",0x01,0,0,0,0,0),
	      -timeout => 60,
	      );
}

sub reserve {
    &checkargs(5,@_);
    local($thissize, $dat);
    $dat = $_[0];
    $thissize = $size!=$nosize ? $size : length($dat);
    &send_cdb(
	      pack("CCCnC",
		   0x16,
		   (&todec($_[1])<<4)|(&todec($_[2])<<1)|(&todec($_[3])),
		   &todec($_[4]),
		   $thissize,
		   0),
	      -dataout => $dat,
	      );
}

sub rezero_unit {
    &checkargs(0,@_);
    &send_cdb(pack("C6",0x01,0,0,0,0,0));
}

sub send_diagnostic {
    &checkargs(5,@_);
    local($thissize, $dat);
    $dat = $_[0];
    $thissize = $size!=$nosize ? $size : length($dat);
    &send_cdb(
	      pack("CCCnC",
		   0x1d,
		   (&todec($_[1])<<4)|
		   (&todec($_[2])<<2)|
		   (&todec($_[3])<<1)|
		   (&todec($_[4])<<0),
		   0,
		   $thissize,
		   0),
	      -dataout => $dat,
	      -timeout => 10*60,
	      );
}

sub send_volume_tag {
    &checkargs(3,@_);
    local($thissize, $dat);
    $dat = $_[0];
    $thissize = $size!=$nosize ? $size : length($dat);
    &send_cdb(
	      pack("CCnCCnnCC",
		   0xb6,
		   $eltype,
		   &xlate($_[1]),
		   0,
		   &todec($_[2]),
		   0,
		   $thissize,
		   0, 0),
	      -dataout => $dat,
	      );
}

sub set_capacity {
    &checkargs(1,@_);
    &send_cdb(pack("C3nC", 0x0b, $immed, 0, $_[0], 0));
}

sub space6 {
    &checkargs(2,@_);
    ($cnt3,$cnt2,$cnt1,$cnt0) = unpack("C4", pack("N", $_[1]));
    &send_cdb(pack("C6", 0x11, $_[0], $cnt2, $cnt1, $cnt0, 0));
}

sub start_stop_unit {
    &checkargs(1,@_);
    &send_cdb(pack("C6", 0x1b, 0, 0, 0, $_[0], 0));
}

sub test_unit_ready {
    &checkargs(0,@_);
    &send_cdb(
	      pack("C6", 0,0,0,0,0,0),
	      #-timeout => 15,
	      );
}

sub unload {
    &load;
}

sub verify {
    &checkargs(2,@_);
    $fix = $_[0];
    ($len3,$len2,$len1,$len0) = unpack("C4", pack("N", $_[1]));
    &send_cdb(
              pack("C6",
                   0x13,
                   ($immed << 2) | ($fix << 0),
                   $len2, $len1, $len0,
                   0),
              );
}

sub write6 {
    &checkargs(3,@_);
    ($len3,$len2,$len1,$len0) = unpack("C4", pack("N", $_[2]));
    $dat = $_[0];
    &send_cdb(
	      pack("C6",
		   0x0a,
		   $_[1],
		   $len2, $len1, $len0,
		   0),
	      -dataout => $dat,
	      -timeout => 120,
	      );
}

sub write {
    &checkargs(3,@_);
    ($blk3,$blk2,$blk1,$blk0) = unpack("C4", pack("N", $_[1]));
    (            $len1,$len0) = unpack("C2", pack("n", $_[2]));
    $dat = $_[0];
    &send_cdb(
	      pack("C10",
		   0x2a,
		   0,
		   $blk3, $blk2, $blk1, $blk0,
		   0,
		   $len1, $len0,
		   0),
	      -dataout => $dat,
	      );
}

sub write_buffer {
    &checkargs(1,@_);
    local($thissize, $dat);
    $dat = $_[0];
    $thissize = $size!=$nosize ? $size : length($dat);
    ( $pp3, $pp2, $pp1, $pp0) = unpack("C4", pack("N", $pp));
    ($len3,$len2,$len1,$len0) = unpack("C4", pack("N", $thissize));
    &send_cdb(
	      pack("C10",
		   0x3b,
		   $mode,
		   $bufid,
		   $pp2, $pp1, $pp0,
		   $len2, $len1, $len0,
		   0),
	      -dataout => $dat,
	      -timeout => 3*60,
	      );
}

sub write_filemarks {
    &checkargs(1,@_);
    ($len3,$len2,$len1,$len0) = unpack("C4", pack("N", $_[0]));
    &send_cdb(
              pack("C6",
                   0x10,
                   $immed,
                   $len2, $len1, $len0,
                   0),
              );
}

1;

#__END__

# mode_select ()
# {
#   cat > $tmp
#   len=`wc -c < $tmp | tr -d " "`
#   ( readnum -x 15 00 0 0 -d $len 0; cat $tmp; ) | scsi 6w$len
#   rm $tmp
# }
# 
# send_volume_tag ()
# {
#   action=`echo $1 | tr [A-Z] [a-z]`
#   case $action in
#     translate|t*) action=0x5;;
#     assert   |a*) action=0x8;;
#     replace  |r*) action=0xa;;
#     undefined|u*) action=0xc;;
#   esac
#   if [ $# -lt 3 ]
#   then
#     len=0
#   else
#     len=40
#     volid=`expr substr "$3                                " 1 32`
#     if [ $# -lt 4 ]
#     then minvolseq=0x0000
#     else minvolseq=$4
#     fi
#     if [ $# -lt 5 ]
#     then maxvolseq=0xffff
#     else maxvolseq=$5
#     fi
#   fi
#   (
#     readnum -x b6 -0 $eltype -W `xlate $2` $action 0 $len 0
#     if [ $len -gt 0 ]
#     then readnum -S "$volid" -W 0 $minvolseq 0 $maxvolseq
#     fi
#   ) | scsi 12w$len
# }
# 
