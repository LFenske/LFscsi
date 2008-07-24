# local(%mdom, %mtyp, %edom, %etyp, %mdomtyp, %edomtyp);
%mdom = (
	 0x00 => "1/2",
	 0x01 => "DLT",
	 0x02 => "8mm",
	 0x80 => "all",
	 0xff => "Unknown",
	 );
%edom = %mdom;
%mtyp = (
	 0x00 => "Unused",
	 0xff => "Unknown",
	 );
%etyp = %mtyp;
%mdomtyp = (
	    "0,1" => "3480 cartridge",
	    "0,E" => "3490E cartridge",
	    "0,J" => "3590 cartridge",
	    "0,M" => "NCTP cartridge",
	    "0,N" => "NCTP cleaning",
	    "1,C" => "CompacTape III",
	    "1,D" => "CompacTape IV",
	    "1,E" => "CompacTape IIIXT",
	    "2,A" => "AIT",
	    "2,M" => "Mammoth",
	    );
%edomtyp = (
	    "0,1" => "8490",
	    "0,2" => "NCTP",
	    "0,3" => "3590",
	    "1,1" => "2000",
	    "1,2" => "2000XT",
	    "1,3" => "4000",
	    "1,4" => "7000",
	    "2,1" => "AIT",
	    "2,2" => "Mammoth",
	    );
