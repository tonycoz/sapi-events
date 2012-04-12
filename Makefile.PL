use strict;
use ExtUtils::MakeMaker;

my %opts = 
  (
   NAME => 'SAPI::Events',
   VERSION_FROM => 'Events.pm',
   OBJECT => 'Events.o sapidummy.o',
  );
my $eu_mmv = eval $ExtUtils::MakeMaker::VERSION;
if ($eu_mmv > 6.06) {
  $opts{AUTHOR} = 'Tony Cook <tonyc@cpan.org>';
  $opts{ABSTRACT} = 'Demo SAPI event delivery back to perl';
}
if ($eu_mmv > 6.30) {
  $opts{LICENSE} = 'perl';
}

WriteMakefile(%opts);
