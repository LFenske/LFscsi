#!/usr/bin/perl

sub printmodesense {
    local($density, $rawtype, $len, $ps, $type, $bytenum);
    &myread_init($_[0]);
    &myread(STDIN, *q, 4) || exit(1);
    local($totlen, $medtype, $WPcache, $BDL) = unpack("CCCC", $q);
    $totlen -= 3;
    if ($BDL) {
	&myread(STDIN, *q, $BDL) || exit(1);
	local($numblocks, $blocklen) = unpack("NN", $q);
	$totlen -= $BDL;
	$density = $numblocks >> 24;
	$numblocks &= 0xffffff;
	print  "Block Descriptor\n";
	print  "  Density: " . (
			$density == 0x03 ? "Optical Erasable 650 MB\n" :
			$density == 0x06 ? "Optical Write-Once 650 MB\n" :
			$density == 0x0a ? "Optical 1.3 GB\n" :
				           "unknown\n")
	    if ($density);
	printf "  %7d Blocks of\n", $numblocks;
	printf "  %7d bytes each\n", $blocklen;
    }
    while ($totlen) {
	&myread(STDIN, *q, 2) || exit(1);
	($rawtype, $len) = unpack("CC", $q);
	&myread(STDIN, *page, $len) || exit(1);
	$totlen -= $len+2;
	$ps = $rawtype >> 7 ? "savable" : "not savable";	# top bit
	$type = $rawtype & 0x3f;	# bottom six bits
	
	if      ($type == 0x1d) {
	    local( $MTad, $MTnm, $STad, $STnm, $IEad, $IEnm, $DTad, $DTnm, $junk ) =
		unpack("n8", $page);
	    printf "page 0x1d: Element Address Assignment (%s)\n", $ps;
	    print  "      start num\n";
	    print  "     ------ ---\n";
	    printf "  MT 0x%.4x %d\n", $MTad, $MTnm;
	    printf "  ST 0x%.4x %d\n", $STad, $STnm;
	    printf "  IE 0x%.4x %d\n", $IEad, $IEnm;
	    printf "  DT 0x%.4x %d\n", $DTad, $DTnm;
	} elsif ($type == 0x1e) {
	    printf "page 0x1e: Transport Geometry (%s)\n", $ps;
	    for ($bytenum=0; $bytenum < length($page); $bytenum += 2) {
		( $rotate, $transport ) = unpack("CC", substr($page, $bytenum, 2));
		printf "  Transport %d is %s\n", $transport, $rotate&1 ? "rotatable" : "non-rotatable";
	    }
	} elsif ($type == 0x1f) {
	    printf "page 0x1f: Device Capabilities (%s)\n", $ps;
	    local( $stor, $res, $frMT, $frST, $frIE, $frDT, $res, $exMT, $exST, $exIE, $exDT ) = unpack("CCCCCCNCCCC", $page);
	    printf "  op & source  DT IE ST MT\n";
	    printf "  ------------ -- -- -- --\n";
	    printf "  store       %s\n", &caps($stor);
	    printf "  move from MT%s\n", &caps($frMT);
	    printf "  move from ST%s\n", &caps($frST);
	    printf "  move from IE%s\n", &caps($frIE);
	    printf "  move from DT%s\n", &caps($frDT);
	    printf "  exch MT with%s\n", &caps($exMT);
	    printf "  exch ST with%s\n", &caps($exST);
	    printf "  exch IE with%s\n", &caps($exIE);
	    printf "  exch DT with%s\n", &caps($exDT);
	} elsif ($type == 0x20) {
	    printf "page 0x20: SCSI Configuration (%s)\n", $ps;
	    local( $parid, $dpinit, $vtchk, $res ) = unpack("CCCC", $page);
	    printf "  parity is %s\n", ($parid  & 0x10) ? "ON" : "OFF";
	    printf "  SCSI ID = %d\n", ($parid  & 0x0f);
	    printf "  DInit  is %s\n", ($dpinit & 0x02) ? "ON" : "OFF";
	    printf "  PInit  is %s\n", ($dpinit & 0x01) ? "ON" : "OFF";	
	} elsif ($type == 0x21) {
	    printf "page 0x21: Midrange Unit Configuration (%s)\n", $ps;
	} elsif ($type == 0x22) {
	    printf "page 0x22: LCD Mode (%s)\n", $ps;
	    local( $bits, $res, @line ) = unpack("CCa20a20a20a20", $page);
	    printf "  LCD security:  %s\n", ($bits&0x40) ? "ON" : "OFF";
	    $mask = 0x08; $linenum = 1;
	    foreach $i (@line) {
		if (($pos = index($i, "\0")) >= 0) {
		    $i = substr($i,0,$pos);
		}
		printf "  line %d %s '%s'\n", $linenum, ($bits&$mask)?"(user):":"(lib): ", $i;
		$mask >>= 1;
		$linenum++;
	    }
	} elsif ($type == 0x00) {
	    printf "page 0x00: Parity (%s)\n", $ps;
	    local( $parity, $retries ) = unpack("CC", $page);
	    printf "  Parity:  %s\n", ($parity&0x20) ? "Enabled" : "Disabled";
	    printf "  Retries: %d\n", $retries;
	} elsif ($type == 0x2a) {
	    printf "page 0x2a: Mixed Media (%s)\n", $ps;
	    local( $bits, $res ) = unpack("CC", $page);
	    print  "  Enabled:      ", $bits&1 ? "TRUE" : "FALSE", "\n";
	    if ($bits&1) {
		print  "  Extended RES: ", &ED($bits&2), "\n";
		print  "  Vendor ASCQ:  ", &ED($bits&4), "\n";
		print  "  Volser:       ", &ED($bits&8), "\n";
	    }
	} elsif ($type == 0x2b) {
	    printf "page 0x2b: Library Geometry (%x)\n", $ps;
	    local( $res, @b0, @b1, @b2, @b3 );
	    ($res, $res,
	     #$b0[0], $b0[1], $b0[2], $b0[3],
	     #$b1[0], $b1[1], $b1[2], $b1[3],
	     #$b2[0], $b2[1], $b2[2], $b2[3],
	     #$b3[0], $b3[1], $b3[2], $b3[3],
	     $b0[0], $b1[0], $b2[0], $b3[0],
	     $b0[1], $b1[1], $b2[1], $b3[1],
	     $b0[2], $b1[2], $b2[2], $b3[2],
	     $b0[3], $b1[3], $b2[3], $b3[3],
	     ) = unpack("CC a5a5a5a5 a5a5a5a5 a5a5a5a5 a5a5a5a5", $page);
	    for ($i=0; $i<4; $i++) {
		print "  $b0[$i] $b1[$i] $b2[$i] $b3[$i]\n";
	    }
	} else {
	    printf "page 0x%x: (unimplemented) (%s)\n", $type, $ps;
	    printf "%.3x:  %.2x %.2x", 0, $rawtype, $len;
	    for ($bytenum=2; $bytenum-2 < length($page); $bytenum++) {
		if ($bytenum % 16 == 0) {
		    printf "%.3x:", $bytenum;
		}
		if ($bytenum %  4 == 0) {
		    printf " ";
		}
		( $byte ) = unpack("C", substr($page, $bytenum-2, 1));
		printf " %.2x", $byte;
		if ($bytenum % 16 == 15) {
		    print "\n";
		}
	    }
	    if ($bytenum % 16 != 0) {
		print "\n";
	    }
	}
	print "\n";
    }
}

sub caps {
    local($bits) = @_;
    local($res);
    $res = "";
    $res .= "  " . ($bits & 8 ? "X" : " ");
    $res .= "  " . ($bits & 4 ? "X" : " ");
    $res .= "  " . ($bits & 2 ? "X" : " ");
    $res .= "  " . ($bits & 1 ? "X" : " ");
}

sub ED {
    return $_[0] ? "ENABLED" : "DISABLED";
}

1;
