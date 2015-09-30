#!/usr/bin/perl

use strict;
use warnings;
use File::Find;
use Tie::File;
use Data::Dumper;

my @dr_headers;

sub wanted_dr_h {
  push @dr_headers, $File::Find::name if /^dr_[0-f]{2}\.h$/;
}

find(\&wanted_dr_h, 'src/descriptors');

for(@dr_headers) {
  my $hdrinfo = parse_header($_);
  next if not defined $hdrinfo;
}

exit 0;

sub parse_header {
  my $dr_h = shift;
  my $sname;
  my $decode_start;
  my $fname;
  my @decode_decl;
  my @file;
  tie @file, 'Tie::File', $dr_h or die "Can't open $dr_h";
  print $dr_h . "\n";
  
  for(@file) {
    if(/^typedef struct dvbpsi_(.*?)_dr_(?:t|s)$/) {
      $sname = $1;
      last;
    }
  }
  return undef if not defined $sname;
  
  for(my $i = 0 ; $i < scalar(@file) ; ++$i) {
    if($file[$i] =~ /^dvbpsi_${sname}_dr_t *?\* *?dvbpsi_Decode(.*?)Dr/) {
      $fname = $1;
      $decode_start = $i;
      push @decode_decl, $file[$i];
      last if($file[$i] =~ /.*;.*/);
      $i++;
      while(1) {
        push @decode_decl, $file[$i];
        if($file[$i] =~ /.*;.*/) {
          last;
        }
        else {
          $i++;
        }
      }
    }
  }
  return undef if not defined $fname;
  
  
  
  untie @file;
  return 0;
}
