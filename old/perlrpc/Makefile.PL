use ExtUtils::MakeMaker;
# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.
WriteMakefile(
    'NAME'	=> 'SCSIRPC',
    'VERSION_FROM' => 'SCSIRPC.pm', # finds $VERSION
    'LIBS'	=> [''],   # e.g., '-lm' 
    'DEFINE'	=> '',     # e.g., '-DHAVE_SOMETHING' 
    'INC'	=> '',     # e.g., '-I/usr/include/other'
    'OBJECT'    => "SCSIRPC.o scsi.o scsirpc/scsi_clnt.o scsirpc/scsi_xdr.o",
);
