#!/usr/bin/perl

sub printelstat {
    local(@type, $q);
    &myread_init($_[0]);

    @type = ( "", "MT", "ST", "IE", "DT", );

    printf "        I E       F    A  B                   \n";
    printf "        n x A E   u A  S  u   L I             \n";
    printf "        E E c x I l S  C  s I U n             \n";
    printf "ty ElAd n n c c E l C  Q  ? D N v Sorc Vol Tag Domain&Type: Element Storage\n";
    printf "-- ---- - - - - - - -- -- ----- - ---- ------- -------------------- -------\n";
    &myread(STDIN, *q, 8) || &printelstat_short(1) && return 1;
    local($firstelem, $numelems, $totlen) = unpack("nnN", $q);
    $totlen &= 0xffffff;
    while ($totlen) {
	&myread(STDIN, *q, 8) || &printelstat_short(2) && return 1;
	$totlen -= 8;
	local($eltype, $pvol, $leneach, $len) = unpack("CCnN", $q);
	while ($len) {
	    &myread(STDIN, *page, $leneach) || &printelstat_short(3) && return 1;
	    $len -= $leneach;
	    $totlen -= $leneach;
	    local($voltaglen, $volreslen, $alttaglen, $altreslen);
	    $voltaglen = ($pvol & 0x80) ? 32 : 0;
	    $volreslen = ($pvol & 0x80) ?  4 : 0;
	    $alttaglen = ($pvol & 0x40) ? 32 : 0;
	    $altreslen = ($pvol & 0x40) ?  4 : 0;
	    local(
		  $eladdr,
		  $f,
		  $res,
		  $asc,
		  $ascq,
		  $valids,
		  $addr,
		  $res,
		  $sval,
		  $ssea,
		  $voltag, $res,
		  $alttag, $res,
		  $dvcidhead,
		  $dvciddata,
		  $vendor
		  );
	    $dvcidlen = 0;
	    for $i ("firstpass", "secondpass") {
		(
		 $eladdr,
		 $f,
		 $res,
		 $asc,
		 $ascq,
		 $valids,
		 $addr,
		 $res,
		 $sval,
		 $ssea,
		 $voltag, $res,
		 $alttag, $res,
		 $dvcidhead,
		 $dvciddata,
		 $vendor
		 ) = unpack("nCCCCCCCCn".
			    "A$voltaglen a$volreslen".
			    "A$alttaglen a$altreslen".
			    "a4 a$dvcidlen a*",
			    $page);
		$dvcidlen = (unpack("CCCC", $dvcidhead))[3];
	    }
	    printf "%s%5d ", $type[$eltype], $eladdr;
	    printf("%s %s %s %s %s %s ",
		   &bit($f,5),
		   &bit($f,4),
		   &bit($f,3),
		   &bit($f,2),
		   &bit($f,1),
		   &bit($f,0),
		   );
	    printf "%.2x %.2x ", $asc, $ascq;
	    printf("%s%2s%2s ",
		   $eltype == 4 ?
		   ($valids&0x80 ? "-" : "+") : " ",
		   $valids&0x20 ? sprintf("%.2d", $addr) : "  ",
		   $valids&0x10 ? sprintf(":%.1d", $LUN&0x07) : "  ");
	    printf "%s", &bit($sval?$sval:0,6);
	    printf(($sval ? "%5d" : "     "), $ssea);
	    printf(" ");
	    printf(  "%-7s ", $voltag) if ($pvol & 0x80);
	    printf("\\%-7s ", $alttag) if ($pvol & 0x40);
	    print $dvciddata, " "
		if $dvcidlen != 0;
	    if (length($vendor) == 4) {
		local($mdom, $mtyp, $edom, $etyp) = unpack("CCCC", $vendor);
		local($mtypc) = pack("C", $mtyp);
		local($mtypfmt, $etypfmt);
		require "scsimisc.pl";
		if (!defined $mtypfmt) { $mtypfmt = $mtyp{$mtyp}; }
		if (!defined $etypfmt) { $etypfmt = $etyp{$etyp}; }
		if (!defined $mtypfmt) { $mtypfmt = $mdomtyp{"$mdom,$mtypc"}; }
		if (!defined $etypfmt) { $etypfmt = $edomtyp{"$edom,$etyp" }; }
		if (!defined $mtypfmt) { $mtypfmt = sprintf("0x%.2x", $mtyp); }
		if (!defined $etypfmt) { $etypfmt = sprintf("0x%.2x", $etyp); }
		local($mpart) = (($f&5)==1) ? "$mdom{$mdom}-$mtypfmt;" : "";
		$mpart .= " " x (21-length($mpart));
		print "$mpart$edom{$edom}";
		print "-$etypfmt" if $etyp;
	    } else {
		for $i (split('', $vendor)) {
		    printf " 0x%.2x", unpack("C", $i);
		    print " ($i)"
			if (' ' le $i && $i le '~');
		}
	    }
	    print "\n";
	}
    }
    if ($myreadp != length($myread)) {
	print length($myread) - $myreadp, " extra bytes\n";
    }
    return 0;
}

sub printelstat_short {
    local($level) = @_;
    print "not enough bytes, level $level\n";
}

sub bit {
    local($flags, $bitnum) = @_;
    return (($flags >> $bitnum) & 1) ? "X" : " ";
}

1;
