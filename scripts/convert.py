#!/usr/bin/env python

# convert binary 16-bit sample to C header file

import sys, os

filename = sys.argv[1]
name     = os.path.splitext(os.path.basename(filename))[0]

sys.stdout.write("const uint16_t %s[] = {\n" % name)
with open(filename, 'rb') as f:
    buf = f.read(2)
    while len(buf):
        sys.stdout.write("    %s,\n" % hex((ord(buf[1]) << 8) | ord(buf[0])))
        buf = f.read(2)
f.close()
sys.stdout.write("\n};\n")
