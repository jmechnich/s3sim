## Copyright (C) 2016 Joerg Mechnich                        -*- mode: octave -*-
## 
## This program is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or
## (at your option) any later version.
## 
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.

## -*- texinfo -*- 
## @deftypefn {Function File} {@var{retval} =} openROM (@var{input1}, @var{input2})
## opens a big-endian binary file with 16-bit audio data and returns it as
## signed and unsigned data vectors
## @seealso{}
## @end deftypefn

## Author: Joerg Mechnich <mechnich@darkstar>
## Created: 2016-02-16

function [datas16, datau16] = openROM (filename)
  f = fopen(filename,"rb","ieee-be");
  datas16 = fread(f,Inf,"int16")';
  frewind(f);
  datau16 = fread(f,Inf,"uint16")';
  datau16 = bitand(datau16,0x3fff);
  fclose(f);
endfunction
