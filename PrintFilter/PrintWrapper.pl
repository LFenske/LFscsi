#!/usr/bin/perl

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


# main

$file = $ARGV[0];
$func = $ARGV[1];

$data = "";
while (sysread(STDIN, $data, 65536, length($data))) {
}

require $file;
&$func($data);

