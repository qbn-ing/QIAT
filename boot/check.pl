#!/usr/bin/perl

open(SIG, $ARGV[0]) || die "open $ARGV[0]: $!";

$n = sysread(SIG, $buf, 1000);
print STDERR "boot block : $n bytes (max 446)\n";
if($n > 446){
  print STDERR "boot block too large: $n bytes (max 446)\n";
  exit 1;
}
close SIG;
