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
## @deftypefn {Function File} {@var{retval} =} playAudiou (@var{input1}, @var{input2})
##
## interprets a row vector as unsigned audio data and plays it back
##
## @seealso{}
## @end deftypefn

## Author: Joerg Mechnich <mechnich@darkstar>
## Created: 2016-02-16

function playAudiou (vec,samplerate=44100)
  scale = max(vec);
  vec = (vec/scale)-0.5;
  data = [vec;vec];
  player = audioplayer([vec;vec],samplerate,16);
  playblocking(player);
endfunction
