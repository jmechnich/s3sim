#!/bin/sh

# converts binary m68k ROM file to elf and disassemles it

if [ -z "$1" ]; then
    echo "Usage: `basename $0` filename"
    exit 1
fi

OBJCOPY=m68k-linux-gnu-objcopy
OBJDUMP=m68k-linux-gnu-objdump
ROMSTART=0x0
if [ ! -z $2 ]; then
    ROMSTART="$2"
fi

SYMNAME=_binary_`echo "$1" | tr './ ' '_'`_
OUTBASE=`basename "$1" | rev | cut -d. -f 2- | rev`

$OBJCOPY \
    --change-addresses=$ROMSTART \
    --redefine-sym ${SYMNAME}start=rom \
    --strip-symbol ${SYMNAME}size \
    --strip-symbol ${SYMNAME}end \
    -I binary -O elf32-m68k -B m68k "$1" "$OUTBASE.elf"

$OBJDUMP -D "$OUTBASE.elf" > "$OUTBASE.dis"
