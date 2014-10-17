#!/usr/bin/perl
use strict;
use warnings;
use diagnostics;

use Data::Dumper;


my @color_intensities=();

<>;<>;

my $width = (split(/ /, <>))[0];
my $max = <>;

my $l=0;
my ($r, $g, $b);
while(<>) {
	chomp;
	if($l==0) {
		$r=$g=$b=-1;
	}
	$r=$_ if $l==0;
	$g=$_ if $l==1;
	$b=$_ if $l==2;
	#print "L=$l R=$r G=$g B=$b\n";


	if($l==2) {
		#if ($r==$max) {
		#	$b=$g=0;
		#}

		push @color_intensities, [$r, $g, $b];
	}

	$l++;
	$l%=3;
}


for my $c (@color_intensities) {
	my $value = (($c->[0])<<16) + (($c->[1])<<8) + $c->[2];
	print sprintf("%x\n", $value);
}
