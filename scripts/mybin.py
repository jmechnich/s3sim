#!/usr/bin/env python

# pretty-print integers as binary

def mybin(intstr):
    base = 10
    if intstr.startswith('0x'): base = 16
    if intstr.startswith('0b'): base = 2
    s = bin(int(intstr,base))[2:]
    for i in xrange(len(s)):
        print str(len(s)-i-1).rjust(3),
    print
    for i in xrange(len(s)):
        print s[i].rjust(3),
    print

if __name__ == '__main__':
    import sys
    for integer in sys.argv[1:]:
        mybin(integer)
