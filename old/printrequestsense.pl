sub printrequestsense {
    local($stt) = @_;
    local($code,	# 0
	  $seg,		# 1
	  $keyf,	# 2
	  $info,	# 3,4,5,6
	  $len,		# 7
	  $csinfo,	# 8, 9, 10, 11
	  $asc,		# 12
	  $ascq,	# 13
	  $fru,		# 14
	  $sksv,	# 15
	  $fieldp,	# 16, 17
	  ) = unpack("CCCNCNCCCCn", $stt);
    print  "Request Sense:\n";
    local($sttlen) = (length($stt));
    if ($sttlen==0) {
	print "  no data\n";
	return 0;
    }
    $len = $sttlen-8 if ($sttlen<8);
    if ($len+8 > $sttlen) { $len = $sttlen-8; }
    local($key, $keyascq);
    if ($sttlen>=3) {
	$key = $keyf & 0xf;
	$keyascq = $keyname[$key];
    }
    if ($sttlen>=2) {
	printf "  code: %2x, seg: %d\n", $code&0x7f, $seg;
    } elsif ($sttlen>=1) {
	printf "  code: %2x\n", $code&0x7f;
    }
    if ($len>=6) {
	local($ascqname) = $asc_q{ ($asc<<8) | $ascq };
	if ($asc == 0x40 && $ascq >= 0x80) {
	    $ascqname = $asc_q{ ($asc<<8) | 0xff };
	}
	$keyascq .= ": $ascqname" if (defined($ascqname));
	printf "  key/ASC/Q: %x/%.2x/%.2x (%s)\n", $key, $asc, $ascq, $keyascq;
    } elsif ($len>=5) {
	printf "  key/ASC: %x/%.2x (%s)\n", $key, $asc, $keyascq;
    } elsif ($sttlen>=3) {
	printf "  key: %x (%s)\n", $key, $keyascq;
    }
    printf "  FM=%s, EOM=%s, ILI=%s\n", ($keyf&0x80)?"T":"F", ($keyf&0x40)?"T":"F", ($keyf&0x20)?"T":"F" if ($sttlen>=3);
    printf "  info:      0x%.8x (%d)\n",   $info,   $info if ($code&0x80 && $sttlen>=7);
    printf "  more info: 0x%.8x (%d)\n", $csinfo, $csinfo if ($len>=4);
    printf "  FRU: 0x%.2x (%d)\n", $fru, $fru if ($len>=7);
    if ($len>=8 && $sksv&0x80) {
	print  "  SKSV:\n";
	if ($key == 5) {
	    # Illegal Request
	    printf "    %s ", $sksv&0x40 ? "Command" : "Data";
	    printf "bit %d ", $sksv&0x7 if ($sksv&0x08);
	    printf "byte %d (0x%.2x)\n", $fieldp, $fieldp;
	}
	if ($key == 1 || $key == 3 || $key == 4) {
	    # Recovered Error, Medium Error, Hardware Error
	    print  "    Actual Retry Count: $fieldp\n";
	}
	if ($key == 2) {
	    # Not Ready
	    print  "    Progress Indication: $fieldp\n";
	}
    }
    if ($len>10) {
	local($num, $byte);
	print  "  more bytes:\n";
	print  "    #0012:      ";
	for ($num=18; $num<$len+8; $num++) {
	    ($byte) = unpack("C", substr($stt, $num));
	    if ($num%8 == 0) {
		printf "    #%.4x:", $num;
		}
	    printf " %.2x", $byte;
	    if (($num+1)%8 == 0) {
		print  "\n";
	    }
	}
	if ($num%8 != 0) {
	    print  "\n";
	}
    }
}

@keyname = (
	    "NO SENSE",		# 0
	    "RECOVERED ERROR",	# 1
	    "NOT READY",	# 2
	    "MEDIUM ERROR",	# 3
	    "HARDWARE ERROR",	# 4
	    "ILLEGAL REQUEST",	# 5
	    "UNIT ATTENTION",	# 6
	    "DATA PROTECT",	# 7
	    "BLANK CHECK",	# 8
	    "VENDOR-SPECIFIC",	# 9
	    "COPY ABORTED",	# A
	    "ABORTED COMMAND",	# B
	    "EQUAL",		# C
	    "VOLUME OVERFLOW",	# D
	    "MISCOMPARE",	# E
	    "RESERVED",		# F
	    );

%asc_q = (
	  0x0000, "NO ADDITIONAL SENSE INFORMATION",
	  0x0001, "FILEMARK DETECTED",
	  0x0002, "END-OF-PARTITION/MEDIUM DETECTED",
	  0x0003, "SETMARK DETECTED",
	  0x0004, "BEGINNING-OF-PARTITION/MEDIUM DETECTED",
	  0x0005, "END-OF-DATA DETECTED",
	  0x0006, "I/O PROCESS TERMINATED",
	  0x0011, "AUDIO PLAY OPERATION IN PROGRESS",
	  0x0012, "AUDIO PLAY OPERATION PAUSED",
	  0x0013, "AUDIO PLAY OPERATION SUCCESSFULLY COMPLETED",
	  0x0014, "AUDIO PLAY OPERATION STOPPED DUE TO ERROR",
	  0x0015, "NO CURRENT AUDIO STATUS TO RETURN",
	  0x0100, "NO INDEX/SECTOR SIGNAL",
	  0x0200, "NO SEEK COMPLETE",
	  0x0300, "PERIPHERAL DEVICE WRITE FAULT",
	  0x0301, "NO WRITE CURRENT",
	  0x0302, "EXCESSIVE WRITE ERRORS",
	  0x0400, "LOGICAL UNIT NOT READY, CAUSE NOT REPORTABLE",
	  0x0401, "LOGICAL UNIT IS IN PROCESS OF BECOMING READY",
	  0x0402, "LOGICAL UNIT NOT READY, INITIALIZING COMMAND REQUIRED",
	  0x0403, "LOGICAL UNIT NOT READY, MANUAL INTERVENTION REQUIRED",
	  0x0404, "LOGICAL UNIT NOT READY, FORMAT IN PROGRESS",
	  0x0500, "LOGICAL UNIT DOES NOT RESPOND TO SELECTION",
	  0x0600, "NO REFERENCE POSITION FOUND",
	  0x0700, "MULTIPLE PERIPHERAL DEVICES SELECTED",
	  0x0800, "LOGICAL UNIT COMMUNICATION FAILURE",
	  0x0801, "LOGICAL UNIT COMMUNICATION TIME-OUT",
	  0x0802, "LOGICAL UNIT COMMUNICATION PARITY ERROR",
	  0x0900, "TRACK FOLLOWING ERROR",
	  0x0901, "TRACKING SERVO FAILURE",
	  0x0902, "FOCUS SERVO FAILURE",
	  0x0903, "SPINDLE SERVO FAILURE",
	  0x0A00, "ERROR LOG OVERFLOW",
	  0x0C00, "WRITE ERROR",
	  0x0C01, "WRITE ERROR RECOVERED WITH AUTO REALLOCATION",
	  0x0C02, "WRITE ERROR - AUTO REALLOCATION FAILED",
	  0x1000, "ID CRC OR ECC ERROR",
	  0x1100, "UNRECOVERED READ ERROR",
	  0x1101, "READ RETRIES EXHAUSTED",
	  0x1102, "ERROR TOO LONG TO CORRECT",
	  0x1103, "MULTIPLE READ ERRORS",
	  0x1104, "UNRECOVERED READ ERROR - AUTO REALLOCATE FAILED",
	  0x1105, "L-EC UNCORRECTABLE ERROR",
	  0x1106, "CIRC UNRECOVERED ERROR",
	  0x1107, "DATA RESYNCHRONIZATION ERROR",
	  0x1108, "INCOMPLETE BLOCK READ",
	  0x1109, "NO GAP FOUND",
	  0x110A, "MISCORRECTED ERROR",
	  0x110B, "UNRECOVERED READ ERROR - RECOMMEND REASSIGNMENT",
	  0x110C, "UNRECOVERED READ ERROR - RECOMMEND REWRITE THE DATA",
	  0x1200, "ADDRESS MARK NOT FOUND FOR ID FIELD",
	  0x1300, "ADDRESS MARK NOT FOUND FOR DATA FIELD",
	  0x1400, "RECORDED ENTITY NOT FOUND",
	  0x1401, "RECORD NOT FOUND",
	  0x1402, "FILEMARK OR SETMARK NOT FOUND",
	  0x1403, "END-OF-DATA NOT FOUND",
	  0x1404, "BLOCK SEQUENCE ERROR",
	  0x1500, "RANDOM POSITIONING ERROR",
	  0x1501, "MECHANICAL POSITIONING ERROR",
	  0x1502, "POSITIONING ERROR DETECTED BY READ OF MEDIUM",
	  0x1600, "DATA SYNCHRONIZATION MARK ERROR",
	  0x1700, "RECOVERED DATA WITH NO ERROR CORRECTION APPLIED",
	  0x1701, "RECOVERED DATA WITH RETRIES",
	  0x1702, "RECOVERED DATA WITH POSITIVE HEAD OFFSET",
	  0x1703, "RECOVERED DATA WITH NEGATIVE HEAD OFFSET",
	  0x1704, "RECOVERED DATA WITH RETRIES AND/OR CIRC APPLIED",
	  0x1705, "RECOVERED DATA USING PREVIOUS SECTOR ID",
	  0x1706, "RECOVERED DATA WITHOUT ECC - DATA AUTO-REALLOCATED",
	  0x1707, "RECOVERED DATA WITHOUT ECC - RECOMMEND REASSIGNMENT",
	  0x1708, "RECOVERED DATA WITHOUT ECC - RECOMMEND REWRITE",
	  0x1800, "RECOVERED DATA WITH ERROR CORRECTION APPLIED",
	  0x1801, "RECOVERED DATA WITH ERROR CORRECTION & RETRIES APPLIED",
	  0x1802, "RECOVERED DATA - DATA AUTO-REALLOCATED",
	  0x1803, "RECOVERED DATA WITH CIRC",
	  0x1804, "RECOVERED DATA WITH L-EC",
	  0x1805, "RECOVERED DATA - RECOMMEND REASSIGNMENT",
	  0x1806, "RECOVERED DATA - RECOMMEND REWRITE",
	  0x1900, "DEFECT LIST ERROR",
	  0x1901, "DEFECT LIST NOT AVAILABLE",
	  0x1902, "DEFECT LIST ERROR IN PRIMARY LIST",
	  0x1903, "DEFECT LIST ERROR IN GROWN LIST",
	  0x1A00, "PARAMETER LIST LENGTH ERROR",
	  0x1B00, "SYNCHRONOUS DATA TRANSFER ERROR",
	  0x1C00, "DEFECT LIST NOT FOUND",
	  0x1C01, "PRIMARY DEFECT LIST NOT FOUND",
	  0x1C02, "GROWN DEFECT LIST NOT FOUND",
	  0x1D00, "MISCOMPARE DURING VERIFY OPERATION",
	  0x1E00, "RECOVERED ID WITH ECC CORRECTION",
	  0x2000, "INVALID COMMAND OPERATION CODE",
	  0x2100, "LOGICAL BLOCK ADDRESS OUT OF RANGE",
	  0x2101, "INVALID ELEMENT ADDRESS",
	  0x2200, "ILLEGAL FUNCTION (SHOULD USE 20 00, 24 00, OR 26 00)",
	  0x2400, "INVALID FIELD IN CDB",
	  0x2500, "LOGICAL UNIT NOT SUPPORTED",
	  0x2600, "INVALID FIELD IN PARAMETER LIST",
	  0x2601, "PARAMETER NOT SUPPORTED",
	  0x2602, "PARAMETER VALUE INVALID",
	  0x2603, "THRESHOLD PARAMETERS NOT SUPPORTED",
	  0x2700, "WRITE PROTECTED",
	  0x2800, "NOT READY TO READY TRANSITION, MEDIUM MAY HAVE CHANGED",
	  0x2801, "IMPORT OR EXPORT ELEMENT ACCESSED",
	  0x2900, "POWER ON, RESET, OR BUS DEVICE RESET OCCURRED",
	  0x2A00, "PARAMETERS CHANGED",
	  0x2A01, "MODE PARAMETERS CHANGED",
	  0x2A02, "LOG PARAMETERS CHANGED",
	  0x2B00, "COPY CANNOT EXECUTE SINCE HOST CANNOT DISCONNECT",
	  0x2C00, "COMMAND SEQUENCE ERROR",
	  0x2C01, "TOO MANY WINDOWS SPECIFIED",
	  0x2C02, "INVALID COMBINATION OF WINDOWS SPECIFIED",
	  0x2D00, "OVERWRITE ERROR ON UPDATE IN PLACE",
	  0x2F00, "COMMANDS CLEARED BY ANOTHER INITIATOR",
	  0x3000, "INCOMPATIBLE MEDIUM INSTALLED",
	  0x3001, "CANNOT READ MEDIUM - UNKNOWN FORMAT",
	  0x3002, "CANNOT READ MEDIUM - INCOMPATIBLE FORMAT",
	  0x3003, "CLEANING CARTRIDGE INSTALLED",
	  0x3100, "MEDIUM FORMAT CORRUPTED",
	  0x3101, "FORMAT COMMAND FAILED",
	  0x3200, "NO DEFECT SPARE LOCATION AVAILABLE",
	  0x3201, "DEFECT LIST UPDATE FAILURE",
	  0x3300, "TAPE LENGTH ERROR",
	  0x3600, "RIBBON, INK, OR TONER FAILURE",
	  0x3700, "ROUNDED PARAMETER",
	  0x3900, "SAVING PARAMETERS NOT SUPPORTED",
	  0x3A00, "MEDIUM NOT PRESENT",
	  0x3B00, "SEQUENTIAL POSITIONING ERROR",
	  0x3B01, "TAPE POSITION ERROR AT BEGINNING-OF-MEDIUM",
	  0x3B02, "TAPE POSITION ERROR AT END-OF-MEDIUM",
	  0x3B03, "TAPE OR ELECTRONIC VERTICAL FORMS UNIT NOT READY",
	  0x3B04, "SLEW FAILURE",
	  0x3B05, "PAPER JAM",
	  0x3B06, "FAILED TO SENSE TOP-OF-FORM",
	  0x3B07, "FAILED TO SENSE BOTTOM-OF-FORM",
	  0x3B08, "REPOSITION ERROR",
	  0x3B09, "READ PAST END OF MEDIUM",
	  0x3B0A, "READ PAST BEGINNING OF MEDIUM",
	  0x3B0B, "POSITION PAST END OF MEDIUM",
	  0x3B0C, "POSITION PAST BEGINNING OF MEDIUM",
	  0x3B0D, "MEDIUM DESTINATION ELEMENT FULL",
	  0x3B0E, "MEDIUM SOURCE ELEMENT EMPTY",
	  0x3D00, "INVALID BITS IN IDENTIFY MESSAGE",
	  0x3E00, "LOGICAL UNIT HAS NOT SELF-CONFIGURED YET",
	  0x3F00, "TARGET OPERATING CONDITIONS HAVE CHANGED",
	  0x3F01, "MICROCODE HAS BEEN CHANGED",
	  0x3F02, "CHANGED OPERATING DEFINITION",
	  0x3F03, "INQUIRY DATA HAS CHANGED",
	  0x4000, "RAM FAILURE (SHOULD USE 40 NN)",
#	  0x40NN, "DIAGNOSTIC FAILURE ON COMPONENT NN (80H-FFH)",
	  0x40ff, "DIAGNOSTIC FAILURE ON COMPONENT NN (80H-FFH)",
	  0x4100, "DATA PATH FAILURE (SHOULD USE 40 NN)",
	  0x4200, "POWER-ON OR SELF-TEST FAILURE (SHOULD USE 40 NN)",
	  0x4300, "MESSAGE ERROR",
	  0x4400, "INTERNAL TARGET FAILURE",
	  0x4500, "SELECT OR RESELECT FAILURE",
	  0x4600, "UNSUCCESSFUL SOFT RESET",
	  0x4700, "SCSI PARITY ERROR",
	  0x4800, "INITIATOR DETECTED ERROR MESSAGE RECEIVED",
	  0x4900, "INVALID MESSAGE ERROR",
	  0x4A00, "COMMAND PHASE ERROR",
	  0x4B00, "DATA PHASE ERROR",
	  0x4C00, "LOGICAL UNIT FAILED SELF-CONFIGURATION",
	  0x4E00, "OVERLAPPED COMMANDS ATTEMPTED",
	  0x5000, "WRITE APPEND ERROR",
	  0x5001, "WRITE APPEND POSITION ERROR",
	  0x5002, "POSITION ERROR RELATED TO TIMING",
	  0x5100, "ERASE FAILURE",
	  0x5200, "CARTRIDGE FAULT",
	  0x5300, "MEDIA LOAD OR EJECT FAILED",
	  0x5301, "UNLOAD TAPE FAILURE",
	  0x5302, "MEDIUM REMOVAL PREVENTED",
	  0x5400, "SCSI TO HOST SYSTEM INTERFACE FAILURE",
	  0x5500, "SYSTEM RESOURCE FAILURE",
	  0x5700, "UNABLE TO RECOVER TABLE-OF-CONTENTS",
	  0x5800, "GENERATION DOES NOT EXIST",
	  0x5900, "UPDATED BLOCK READ",
	  0x5A00, "OPERATOR REQUEST OR STATE CHANGE INPUT (UNSPECIFIED)",
	  0x5A01, "OPERATOR MEDIUM REMOVAL REQUEST",
	  0x5A02, "OPERATOR SELECTED WRITE PROTECT",
	  0x5A03, "OPERATOR SELECTED WRITE PERMIT",
	  0x5B00, "LOG EXCEPTION",
	  0x5B01, "THRESHOLD CONDITION MET",
	  0x5B02, "LOG COUNTER AT MAXIMUM",
	  0x5B03, "LOG LIST CODES EXHAUSTED",
	  0x5C00, "RPL STATUS CHANGE",
	  0x5C01, "SPINDLES SYNCHRONIZED",
	  0x5C02, "SPINDLES NOT SYNCHRONIZED",
	  0x6000, "LAMP FAILURE",
	  0x6100, "VIDEO ACQUISITION ERROR",
	  0x6101, "UNABLE TO ACQUIRE VIDEO",
	  0x6102, "OUT OF FOCUS",
	  0x6200, "SCAN HEAD POSITIONING ERROR",
	  0x6300, "END OF USER AREA ENCOUNTERED ON THIS TRACK",
	  0x6400, "ILLEGAL MODE FOR THIS TRACK",

	  0x0483, "Not ready because a front door is open",
	  0x0489, "Not ready because running from serial port",
	  0x048D, "Not ready because running offline",
	  0x0490, "Not ready because not taught",
	  0x1580, "Accessor dropped a cartridge",
	  0x1581, "Accessor could not pick a cartridge",
	  0x1583, "Accessor could not put a cartridge",
	  0x1585, "Gripper could not open",         
	  0x2480, "Attempt to write a read-only buffer",
	  0x2680, "Checksum of Parameter data failure",
	  0x2980, "Reset for Perm error recovery",
	  0x2981, "Reset into degraded mode",
	  0x3B85, "Destination element is medium changer",
	  0x3B86, "Source element is the medium changer",
	  0x3B87, "Cartridge cannot be removed from drive",
	  0x3B90, "Cartridge loaded in drive cannot be moved", 
	  0x3BA0, "Media will not go into destination",
	  0x3F80, "Failure to erase FLASH EEPROMS",
	  0x3F84, "Failure to program FLASH EEPROMS",
	  0x4001, "Gripper holds cartridge unexpectedly",
	  0x4088, "Door open or solenoid defective",
	  0x4091, "Generic Gripper error",
	  0x40A0, "Accessor could not move on Y axis",
	  0x40A1, "Accessor could not home on Y axis",
	  0x40A3, "Y axis controller could not reset",
	  0x40B0, "Accessor could not move on X axis",
	  0x40B1, "Accessor could not home on X axis",
	  0x40B3, "X axis controller could not reset",
	  0x40C0, "Accessor cannot move in X or Y",
	  0x40E0, "Accessor has no power to move at all",
	  0x5380, "Accessor has problems with tape in IE",
	  0x5381, "The IE station is open",
	  0x5382, "Cannot lock the IE station",
	  0x5383, "Cannot unlock the IE station",
	  0x8300, "Bar Code label is questionable",
	  0x8301, "Cannot get scanner ready to read labels",
	  0x8302, "Cartridge magazine not present",
	  0x8303, "Bar code and full status questionable",
	  0x8304, "Drive not installed",
	  0x8307, "Drive configuration not valid", 
	  0x8308, "Bar Code label is upside down",
	  0x8309, "No Bar Code Label installed",
	  0x830A, "Cannot calibrate offsets, teach",
	  0x8400, "Firmware seems to have a problem",

	  );

1;
