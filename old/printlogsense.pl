#!/usr/bin/perl

%page_names = (
	       0x00 => "Supported",
	       0x30 => "System Statistics",
	       0x31 => "State",
	       0x32 => "History of Events",
	       0x33 => "Element Statistics",
	       0x34 => "Cartridge Scan Retries",
	       0x35 => "Element Position",
	       0x3e => "Supported Media",
	       );

%logsense_titles = (
		    "30,0" => "Total Number of Moves",
		    "30,1" => "Total Number of Pick Retries",
		    "30,2" => "Total Number of Put Retries",
		    "30,3" => "Total Number of Scans",
		    "30,4" => "Total Number of Scan Retries",
		    "30,5" => "Reserved",
		    "30,6" => "Total Number of I/E Station Insert Cycles",
		    "30,7" => "Reserved",
		    "31,0" => "Door Open",
		    "31,1" => "Reserved",
		    "31,2" => "Retract Complete",
		    "31,3" => "Cartridge Present",
		    "31,4" => "Reserved",
		    "31,5" => "Horizontal Axis Home",
		    "31,6" => "Reserved",
		    "31,7" => "Reserved",
		    "31,8" => "Vertical Axis Home",
		    "31,9" => "Insert/Eject Station Locked",
		    "31,10"=> "Insert/Eject Station Open",
		    "33,0" => "Total Puts",
		    "33,1" => "Total Put Retries",
		    "33,2" => "Total Pick Retries",
		    "35,0" => "Vertical Axis Position",
		    "35,1" => "Picker Position",
		    "35,2" => "Horizontal Axis Position",
		    );

sub printlogsense {
    local($dat) = @_;
    local($datp) = 0;
    local($pagecode, $res, $totlen) = unpack("x${datp}CCn", $dat);
    exit(1) if ($datp += 4) > length($dat);
    printf "Log Sense Page 0x%.2x", $pagecode;
    print ": $page_names{$pagecode} Log Page"
	if defined $page_names{$pagecode}; 
    print "\n";
    if ($pagecode == 0) {
	exit(1) if $datp+$totlen > length($dat);
	print  "  supported pages:\n";
	foreach $i (unpack("x${datp}C*", $dat)) {
	    printf "    %.2x", $i;
	    print ": $page_names{$i} Log Page"
		if defined $page_names{$i}; 
	    print "\n";
	}
    } else {
	while ($totlen) {
	    local($p0, $p1, $p2);
	    local($uform, $format);
	    local($parmcode, $bits, $parmlen) = unpack("x${datp}nCC", $dat);
	    exit(1) if ($datp += 4) > length($dat); $totlen -= 4;
	    local($page) = unpack("x${datp}a${parmlen}", $dat);
	    exit(1) if ($datp += $parmlen) > length($dat); $totlen -= $parmlen;
	    if ($bits & 0x01) {
	    } else {
		if    ($parmlen == 1) { $uform = "C"  ; $format = "%.2x"; }
		elsif ($parmlen == 2) { $uform = "n"  ; $format = "%.4x"; }
		elsif ($parmlen == 4) { $uform = "N"  ; $format = "%.8x"; }
		elsif ($parmlen == 6) { $uform = "nnn"; $format = "%.4x %.4x %.4x"; }
		elsif ($parmlen == 8) { $uform = "Nnn"; $format = "%.8x %.4x %.4x"; }
		else { printf "change printlogsense.pl to add \$parmlen == $parmlen, \$pagecode == 0x%.2x\n", $pagecode; }
		($p0, $p1, $p2) = unpack($uform, $page);
	    }
	    if ($pagecode == 0x30 || $pagecode == 0x31) {
		$format = "%5d" if ($pagecode == 0x30);
		$format = "%1d" if ($pagecode == 0x31);
		printf "  %.4x: $format", $parmcode, $p0;
		local($title) = $logsense_titles{sprintf("%.2x,%d", $pagecode, $parmcode)};
		print "  $title" if defined $title;
		print "\n";
	    } elsif ($pagecode == 0x33) {
		printf "  %.4x: %5d %2d %2d\n", $parmcode, $p0, $p1, $p2;
	    } elsif ($pagecode == 0x35) {
		printf "  %.4x: %5d %5d %5d\n", $parmcode, $p0, $p1, $p2;
	    } elsif ($pagecode == 0x3e) {
		require "scsimisc.pl";
		local($dom, $etyp) = ($parmcode >> 8, $parmcode & 0xff);
		printf "  %.4x: ", $parmcode;
		local($edom) = $edom{$dom};
		print $edom if defined $edom;
		local($ename) = $edomtyp{"$dom,$etyp"};
		print "-$ename" if defined $ename;
		print "\n";
		for $mtyp (split('', $page)) {
		    print "    '$mtyp':  ";
		    local($mdom) = $mdom{$dom};
		    print $mdom if defined $mdom;
		    local($mname) = $mdomtyp{"$dom,$mtyp"};
		    print "-$mname" if defined $mname;
		    print "\n";
		}
	    } else {
		if ($bits & 0x01) {
		    printf "  %.4x:", $parmcode;
		    print length($page) > 72 ? "\n" : " ";
		    print $page;
		    print "\n";
		} else {
		    printf "  %.4x: ", $parmcode;
		    printf $format, $p0, $p1, $p2
			if defined $format;
		    print "\n";
		}
	    }
	}
    }
    return 1;
}

1;
