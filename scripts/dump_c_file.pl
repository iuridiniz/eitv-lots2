#!/usr/bin/perl

use strict;
use warnings;

open FILE, "<", $ARGV[0] or die "unnable to open $ARGV[0]";

my $byte;
my @buffer;
my $first_time = 1;

while(read(FILE, $byte, 1)) {

    if (@buffer == 10) {
        if ($first_time) { 
            print "const guint8 VAR = {\n";
            $first_time = 0;
        } else {
            print ",\n";
        }
        dump_buffer(\@buffer);
        @buffer = ();
    }
    push(@buffer, ord($byte));

}

if (@buffer) {
    if ($first_time) { 
        print "const guint8 VAR = {\n";
    }  else {
        print ",\n";
    }
    dump_buffer(\@buffer);
    print "\n};\n";
}

sub dump_buffer {
    my @buffer = @{shift()};
    my @hexas = map { sprintf( "0x%02X", $_) } @buffer;
    print "    ", join(", ", @hexas) ;
}
