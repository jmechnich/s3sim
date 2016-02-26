#!/usr/bin/env python

# sets the first two bits of big-endian encoded 16-bit data to zero

import sys, os

def pad14(fobj):
    buf = fobj.read(2)
    while len(buf):
        sys.stdout.write(chr(ord(buf[0]) & 0x3f))
        sys.stdout.write(buf[1])
        buf = fobj.read(2)

for fn in sys.argv[1:]:
    with open(fn, 'rb') as f:
        pad14(f)
