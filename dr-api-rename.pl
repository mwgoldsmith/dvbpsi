#!/usr/bin/perl

use strict;
use warnings;
use File::Find;
use Tie::File;
use Data::Dumper;
use File::Basename;

my @dr_headers;

sub wanted_dr_h {
  push @dr_headers, $File::Find::name if /^dr_[0-f]{2}\.h$/;
}

find(\&wanted_dr_h, 'src/descriptors');

for(@dr_headers) {
  my $hdrinfo = parse_header($_);
  if(not defined $hdrinfo) {
    if(/dr_a1\.h$/) {
      $hdrinfo = {
        old => { stru => 'dvbpsi_service_location_dr',
          gen => 'dvbpsi_GenServiceLocationDr', 
          decode => 'dvbpsi_DecodeServiceLocationDr'
        },
        new => { stru => 'dvbpsi_atsc_service_location_dr',
          gen => 'dvbpsi_gen_atsc_service_location_dr',
          decode => 'dvbpsi_decode_atsc_service_location_dr'
        }
      }
    }
    elsif(/dr_49\.h$/) {
      $hdrinfo = {
        old => { stru => 'dvbpsi_country_availability_dr',
          gen => 'dvbpsi_GenCountryAvailabilityDr', 
          decode => 'dvbpsi_DecodeCountryAvailability'
        },
        new => { stru => 'dvbpsi_dvb_country_availability_dr',
          gen => 'dvbpsi_gen_dvb_country_availability_dr',
          decode => 'dvbpsi_decode_dvb_country_availability_dr'
        }
      }
    }
    else {
      print STDERR "Failed to process $_\n";
      next;
    }
  }
  replace_names($hdrinfo);
}

exit 0;

sub parse_header {
  my $dr_h = shift;
  my $sname;
  my $fname;
  my @decode_decl;
  my @file;
  my $rv;
  tie @file, 'Tie::File', $dr_h or die "Can't open $dr_h";
  
  for(@file) {
    if(/^typedef struct dvbpsi_(.*?)_dr_(?:t|s)$/) {
      $sname = $1;
      last;
    }
  }
  goto end if not defined $sname;
  
  for(my $i = 0 ; $i < scalar(@file) ; ++$i) {
    if($file[$i] =~ /^dvbpsi_${sname}_dr_t *?\* *?dvbpsi_Decode(.*?)Dr/) {
      $fname = $1;
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
  goto end if not defined $fname;
  $dr_h =~ /dr_([0-9a-f]{2})\.h$/;
  my $drnum = hex($1);
  
  my $oldinfo = {
    stru => "dvbpsi_${sname}_dr",
    gen => "dvbpsi_Gen${fname}Dr",
    decode => "dvbpsi_Decode${fname}Dr"
  };
  $rv = { old => $oldinfo, new => get_new_hdrinfo($sname, get_dr_system($drnum)) };

end:
  untie @file;
  return $rv;
}

sub get_dr_system {
  my $drnum = shift;
  return "mpeg" if $drnum >= 2 && $drnum <= 63;
  return "dvb" if $drnum >= 64 && $drnum <= 127;
  return "atsc" if $drnum == 0xa0 || $drnum == 0x86 || $drnum == 0x81;
  return "eacem" if $drnum == 0x83;
  return "scte" if $drnum == 0x8a;
  return "custom" if $drnum >= 128 && $drnum <= 254;
  die "Forbidden descriptor $drnum";
}

sub get_new_hdrinfo {
  (my $sname, my $sys) = @_;
  return {
    stru => "dvbpsi_${sys}_${sname}_dr",
    gen => "dvbpsi_gen_${sys}_${sname}_dr",
    decode => "dvbpsi_decode_${sys}_${sname}_dr"
  }
}

my $replacer;

sub wanted_global_c_or_h {
  return unless /\.(c|h)$/;
  my @file;
  my $old = $replacer->{old};
  my $new = $replacer->{new};
  tie @file, 'Tie::File', $_ or die "Cannot open $_";
  for(@file) {
    s/$old->{stru}/$new->{stru}/g;
    s/$old->{gen}/$new->{gen}/g;
    s/$old->{decode}/$new->{decode}/g;
  }
  untie @file;
}

sub replace_names {
  $replacer = shift;
  find(\&wanted_global_c_or_h, '.');
}
