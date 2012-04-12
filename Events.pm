package SAPI::Events;
use strict;

BEGIN {
  our $VERSION = '0.006';

  require XSLoader;
  XSLoader::load('SAPI::Events' => $VERSION);
}

1;
