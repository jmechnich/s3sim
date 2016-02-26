#!/usr/bin/env octave

## plays a big-endian 16-bit sound file

arg_list = argv ();
for i = 1:nargin
  printf ("Playing %s", arg_list{i});
  datas = openROM(arg_list{i});
  playAudios(datas);
endfor
