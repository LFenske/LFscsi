#!/usr/bin/perl

sub printinquiry {
    return if (!defined($_[0]));
    &myread_init($_[0]);
    local($i, $q);

    &myread(STDIN, *q, 4) || exit(1);
    local($periphtype, $rmb, $ver, $format) = unpack("CCCC", $q);
    if ($ver == 0) {
	# this must be vital product data
	local($page) = $rmb;
	local($len) = $format;
	printf "Inquiry page 0x%.2x: ", $page;
	&myread(STDIN, *q, $len) || exit(1);
	if      ($page == 0x00) {
	    print  "Supported Vital Product Data Page\n";
	    for ($i=0; $i<$len; $i++) {
		local($suppage) = unpack("C", substr($q, $i, 1));
		printf "  Supported VPD Page: 0x%.2x\n", $suppage;
	    }
	} elsif (0x01 <= $page && $page <=0x7f) {
	    printf "ASCII Information Page 0x%.2x\n", $page;
	    local($thislen) = unpack("C", $q);
	    print  "  ASCII information:\n";
	    printf "    %s\n", substr($q, 1, $thislen);
	    print  "  Vendor-specific information:\n";
	    printf "    %s\n", substr($q, 1+$thislen);
	} elsif ($page == 0x80) {
	    print  "VPD Unit Serial Number Page\n";
	    print  "  Product Serial Number: $q\n";
	} elsif ($page == 0x81) {
	    print  "Implemented Operating Definition Page\n";
	    local($curopdef, $defopdef) = unpack("CC", $q);
	    printf "  Current Operating Definition: %d\n", $curopdef;
	    printf "  Default Operating Definition: %d, SavImp: %d\n", $defopdef & 0x7f, $defopdef >> 7;
	    for ($i=2; $i<length($q); $i++) {
		local($thisop) = unpack("C", substr($q, $i));
		printf "  Support Operating Definition: %d, SavImp: %d\n", $thisop & 0x7f, $thisop >> 7;
	    }
	} elsif ($page == 0x82) {
	    print  "ASCII Implemented Operating Definition Page\n";
	    local($thislen) = unpack("C", $q);
	    print  "  ASCII operating definition description data:\n";
	    printf "    %s\n", substr($q, 1, $thislen);
	    print  "  Vendor-specific description data:\n";
	    printf "    %s\n", substr($q, 1+$thislen);
	} elsif ($page == 0xc0) {
	    print  "(Vendor-specific) Firmware Revision Page\n";
	    local($revision, $builddate, $checksum) = unpack("a22a19a14", $q);
	    print  "  Revision:    $revision\n";
	    print  "  Build Date:  $builddate\n";
	    print  "  Checksum:    $checksum\n";
#	     print  "(Vendor-specific) Device Firmware Page\n";
#	     local($controller, $DSP, $res, $firmware, $SN) = unpack("a4a4Ca3a4", $q);
#	     print  "  Controller Code Revision:  $controller\n";
#	     print  "  DSP        Code Revision:  $DSP\n";
#	     print  "  Firmware Revision of Head: $firmware\n";
#	     print  "  Serial Number     of Head: $SN\n";
	} elsif ($page == 0xe0) {
	    print  "Implemented SCSI-2 Commands Page\n";
	    for ($i=0; $i<$len; $i++) {
		local($cmd) = unpack("C", substr($q, $i, 1));
		local($name) = $cmdtable{$cmd};
		$name = "unknown"
		    if (!defined $name);
		printf "  Command Code: 0x%.2x (%s)\n", $cmd, $name;
	    }
	} elsif ($page == 0xe1) {
	    print  "Implemented Vendor-Specific Commands Page\n";
	    for ($i=0; $i<$len; $i++) {
		local($cmd) = unpack("C", substr($q, $i, 1));
		local($name) = $cmdtable{$cmd};
		$name = "unknown"
		    if (!defined $name);
		printf "  Command Code: 0x%.2x (%s)\n", $cmd, $name;
	    }
	} else {
	    printf "This is a page not supported by this program: 0x%.2x\n", $page;
	}
    } else {
	# this must not be vital product data
	print  "Inquiry\n";
	printf "  Peripheral Device Type: %s", $periphtype{$periphtype&0x1f};
	printf ", Peripheral Qualifier: %d\n", $periphtype>>5;
	printf "  %sRemovable Media\n", ($rmb & 0x80) ? "" : "Non-";
	printf "  Device-type modifier: %d\n", $rmb & 0x7f;
	printf "  ISO version:  %d\n", ($ver>>6)&3;
	printf "  ECMA version: %d\n", ($ver>>3)&7;
	printf "  ANSI version: %d\n", ($ver>>0)&7;
	print  "  ";
	printf "AENC: %d, ", $format >> 7 if ($periphtype == 3);
	printf "TrmIOP: %d, Response data format: %d\n", ($format>>6)&1, $format&7;

	&myread(STDIN, *q, 4) || exit(1);
	local($len, $res, $res, $bits) = unpack("CCCC", $q);
	$len -= 3;
	print  "  RelAdr WBus32 WBus16  Sync  Linked CmdQue SftRe \n";
	printf "    %s     %s     %s     %s     %s     %s     %s  \n",
	    &biti($bits,7), &biti($bits,6), &biti($bits,5), &biti($bits,4),
	    &biti($bits,3),                 &biti($bits,1), &biti($bits,0);

	&myread(STDIN, *q,    8) || exit(1); $len -=    8;
	print  "  Vendor  ID: $q\n";

	&myread(STDIN, *q,   16) || exit(1); $len -=   16;
	print  "  Product ID: $q\n";

	&myread(STDIN, *q,    4) || exit(1); $len -=    4;
	print  "  Rev level:  $q\n";

	&myread(STDIN, *q, $len) || exit(1); $len -= $len;
	print  "  Other:      $q\n";
    }
}

sub biti {
    local($flags, $bitnum) = @_;
    return (($flags >> $bitnum) & 1) ? "1 " : "0 ";
}

%periphtype = (
	       0x00, "Direct Access",
	       0x01, "Sequential Access",
	       0x02, "Printer",
	       0x03, "Processor",
	       0x04, "Write-once",
	       0x05, "CD-ROM",
	       0x06, "Scanner",
	       0x07, "Optical Memory",
	       0x08, "Autochanger",
	       0x09, "Communications",
	       0x0a, "Graphic Arts Pre-press",
	       0x0b, "Graphic Arts Pre-press",
	       0x1f, "Unknown",
	       );

%cmdtable = (
	     0x00, "Test Unit Ready",
	     0x01, "Rezero Unit//Rewind",
	     0x03, "Request Sense",
	     0x04, "Format//Format Unit",
	     0x05, "Read Block Limits",
	     0x07, "Initialize Element Status//Reassign BLocks",
	     0x08, "Read (6)//Get Message (6)//Receive",
	     0x0a, "Write (6)//Send Message (6)//Send (6)//Print",
	     0x0b, "Seek (6)//Slew and Print",
	     0x0f, "Read Reverse",
	     0x10, "Synchronize Buffer//Write Filemarks",
	     0x11, "Space",
	     0x12, "Inquiry",
	     0x13, "Verify (6)",
	     0x14, "Recover Buffered Data",
	     0x15, "Mode Select (6)",
	     0x16, "Reserve//Reserve Unit",
	     0x17, "Release//Release Unit",
	     0x1a, "Mode Sense (6)",
	     0x1b, "Load/Unload//Stop/Start Unit//Stop Print//Scan",
	     0x1c, "Receive Diagnostic Results",
	     0x1d, "Send Diagnostic",
	     0x1e, "Prevent/Allow Medium Removal",
	     0x24, "Set Window",
	     0x25, "Read Capacity//Read CD-ROM Capacity//Get Window",
	     0x28, "Read (10)//Get Message (10)",
	     0x29, "Read Generation",
	     0x2a, "Write (10)//Send Message (10)//Send (10)",
	     0x2b, "Position to Element//Locate//Seek (10)",
	     0x2c, "Erase",
	     0x2d, "Read Updated Block",
	     0x2e, "Write and Verify (10)",
	     0x2f, "Verify",
	     0x30, "Search Data High (10)",
	     0x31, "Search Data Equal (10)//Object Position",
	     0x32, "Search Data Low (10)",
	     0x33, "Set Limits (10)",
	     0x34, "Read Position//Pre-Fetch//Get Data Buffer Status",
	     0x35, "Synchronize Cache",
	     0x36, "Lock/Unlock Cache",
	     0x37, "Read Defect Data (10)",
	     0x38, "Medium Scan",
	     0x39, "Compare",
	     0x3a, "Copy and Verify",
	     0x3b, "Write Buffer",
	     0x3c, "Read Buffer",
	     0x3d, "Upate Block",
	     0x3e, "Read Long",
	     0x3f, "Write Long",
	     0x40, "Change Definition",
	     0x41, "Write Same",
	     0x42, "Read Sub-Channel",
	     0x43, "Read TOC",
	     0x44, "Read Header",
	     0x45, "Play Audio (10)",
	     0x47, "Play Audio MSF",
	     0x48, "Play Audio Track Index",
	     0x49, "Play Track Relative (10)",
	     0x4b, "Pause/Resume",
	     0x4c, "Log Select",
	     0x4d, "Log Sense",
	     0x55, "Mode Select (10)",
	     0x5a, "Mode Sense (10)",
	     0xa5, "Move Medium//Play Audio (12)",
	     0xa6, "Exchange Medium",
	     0xa8, "Get Message (12)",
	     0xa9, "Play Track Relative (12)",
	     0xaa, "Write (12)//Send Message (12)",
	     0xac, "Erase (12)",
	     0xae, "Write and Verify (12)",
	     0xaf, "Verify (12)",
	     0xb0, "Search Data High (12)",
	     0xb1, "Search Data Equal (12)",
	     0xb2, "Search Data Low (12)",
	     0xb3, "Set Limits (12)",
	     0xb5, "Request Volume Element Address",
	     0xb6, "Send Volume Tag",
	     0xb7, "Read Defect Data (12)",
	     0xb8, "Read Element Status",
	     0xd8, "Read CDDA",
	     0xe7, "Initialize Element Status With Range",
	     );

1;
