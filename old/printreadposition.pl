#!/usr/bin/perl

sub TF {
    return $_[0] ? "true" : "false";
}

sub val {
    return $_[0] ? " (invalid)" : "";
}

sub printreadposition {
    return if (!defined($_[0]));
    my($flags, $partnum, $rsvd2, $blockfirst, $blocklast, $numblocks, $numbytes) = unpack("CCnNNNN", $_[0]);
    my($BOP, $EOP, $BCU, $BYCU, $BPU, $PERR);
    $BOP  = ($flags >> 7) & 1;
    $EOP  = ($flags >> 6) & 1;
    $BCU  = ($flags >> 5) & 1;
    $BYCU = ($flags >> 4) & 1;
    $BPU  = ($flags >> 2) & 1;
    $PERR = ($flags >> 1) & 1;
    print  "Read Position:\n";
    printf "  BOP : %s\n", TF($BOP );
    printf "  EOP : %s\n", TF($EOP );
    printf "  BCU : %s\n", TF($BCU );
    printf "  BYCU: %s\n", TF($BYCU);
    printf "  BPU : %s\n", TF($BPU );
    printf "  PERR: %s\n", TF($PERR);
    print  "  First Block Location      : $blockfirst\n" , val($BPU );
    print  "  Last  Block Location      : $blocklast\n"  , val($BPU );
    printf "  Number of blocks in buffer: $numblocks%s\n", val($BCU );
    printf "  Number of bytes  in buffer: $numbytes%s\n" , val($BYCU);
}

1;
