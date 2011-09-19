#!/usr/bin/perl
#
#  create a new gobject class files stubs
#


use strict;
use warnings;

use FindBin;

sub parser {
    my $tmpl = shift();
    my %subst = %{ shift() };
    
    foreach my $key (keys(%subst)) {
        $tmpl =~ s/$key/$subst{$key}/g;
    }

    return $tmpl;
}
my $copyright_template   = $FindBin::Bin . "/templates/copyright.tmpl";
my $gobject_class_header_template; 
my $gobject_class_source_template;

if (@ARGV == 2) {
    $gobject_class_header_template = $FindBin::Bin . "/templates/gobject_class.h.tmpl";
    $gobject_class_source_template = $FindBin::Bin . "/templates/gobject_class.c.tmpl";
} elsif (@ARGV == 3) {
    $gobject_class_header_template = $FindBin::Bin . "/templates/gobject_$ARGV[0].h.tmpl";
    $gobject_class_source_template = $FindBin::Bin . "/templates/gobject_$ARGV[0].c.tmpl";
    if ( ! -f $gobject_class_header_template ) {
        die "'$gobject_class_header_template' does not exists";
    } 
    if ( ! -f $gobject_class_source_template ) {
        die "'$gobject_class_source_template' does not exists";
    }
    shift(@ARGV);
} else {
    die "invalid paramters" ;
}

my $namespace = lc($ARGV[0]);
# class in lower_case separeted by _ 
my $class = lc($ARGV[1]);

##################################################################

# the name of the new type, in Camel case
my $TN =  join("", map { ucfirst } split("_", $class));

# the name of new type, in lowercase, with words separated by _
my $t_n = join("_", $class);

# the name of new tupe, in uppercase, with words separetd by _
my $T_N = uc($t_n);

##################

my $n_s = $namespace;
my $N_S = uc($n_s);
my $NS = ucfirst($n_s);


# verify if the outfiles already exists 
my $header_filename = join(".", join("_", $n_s, $t_n), "h");

if( -e join("/", ".", $header_filename)) {
    die "file '$header_filename' already exists";
}

my $source_filename = join(".", join("_", $n_s, $t_n), "c");

if( -e join("/", ".", $source_filename)) {
    die "file '$source_filename' already exists";
}


# Contruct the copyright file
open (ARQ, "<", $copyright_template) or die "cannot open '$copyright_template'";
my $copyright_content;
{ 
    local $/; # slurp mode
    $copyright_content = <ARQ>;
}
close (ARQ);
my $copyright_subst = { 
    '@BASENAME@' => $NS . $TN,
    '@DATE@' => scalar(localtime()),
    '@YEAR@' => (localtime())[5] + 1900,
    '@NAME@' => (getpwnam(getlogin()))[6], 
    '@EMAIL@' => getlogin() . '@email.com.br',
    '@COMPANY@' => (getpwnam(getlogin()))[6],
};

$copyright_content = parser($copyright_content, $copyright_subst);

# Construct the header file 
open (ARQ, "<", $gobject_class_header_template) or die "cannot open '$gobject_class_header_template'";
my $header_content;
{ 
    local $/; # slurp mode
    $header_content = <ARQ>;
}
close (ARQ);

my $header_subst = { 
    '@T_N@' => $T_N,
    '@t_n@' => $t_n,
    '@TN@'  => $TN, 

    '@N_S@'  => $N_S,
    '@n_s@'   => $n_s,
    '@NS@'  => $NS,

    '@COPYRIGHT@' => $copyright_content,
};

$header_content = parser($header_content, $header_subst);

# write to file 
open (ARQ, ">", join("/", ".", $header_filename))
    or die "Cannot open '$header_filename' for writing";

print ARQ $header_content;

close(ARQ);

# Construct the source file
open (ARQ, "<", $gobject_class_source_template) or die "cannot open '$gobject_class_source_template'";
my $source_content;
{ 
    local $/; # slurp mode
    $source_content = <ARQ>;
}
close (ARQ);

my $source_subst = $header_subst;

$source_content = parser($source_content, $source_subst);

# writing to file
open (ARQ, ">", join("/", ".", $source_filename))
    or die "Cannot open '$source_filename' for writing";

print ARQ $source_content;

close(ARQ);
