package SCSIRPC;

use strict;
use vars qw($VERSION @ISA @EXPORT);

require Exporter;
require DynaLoader;

@ISA = qw(Exporter DynaLoader);
# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.
@EXPORT = qw(
	
);
$VERSION = '0.01';

bootstrap SCSIRPC $VERSION;

# Preloaded methods go here.

# Autoload methods go after =cut, and are processed by the autosplit program.

1;
__END__
# Below is the stub of documentation for your module. You better edit it!

=head1 NAME

SCSIRPC - Perl extension for raw SCSI access

=head1 SYNOPSIS

  use SCSIRPC;
  $status = SCSIRPC::send_cdb(
			      $device,
			      $rw,
			      $cdb,
			      $dat, $datlen,
			      $stt, $sttlen,
			      $timeout,
			      );

=head1 DESCRIPTION



=head1 AUTHOR

Larry Fenske,
larry@emass.com,
larry@towanda.com

=head1 SEE ALSO

perl(1).

=cut
