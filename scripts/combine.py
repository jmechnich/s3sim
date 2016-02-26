#!/usr/bin/env python

# usage: combine.py file1 file2
#
# combines even/odd (high/low) ROM files to one

import sys

with open(sys.argv[1], "rb") as f1:
    with open(sys.argv[2], "rb") as f2:
        b1 = f1.read(1)
        b2 = f2.read(1)
        while b1 != "" and b2 != "":
            sys.stdout.write(b1)
            sys.stdout.write(b2)
            b1 = f1.read(1)
            b2 = f2.read(1)
