#!/usr/bin/perl
# Copyright (C) 2008  Larry Fenske
# 
# This file is part of LFscsi.
# 
# LFscsi is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
# 
# LFscsi is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public
# License along with LFscsi.  If not, see <http://www.gnu.org/licenses/>.

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

