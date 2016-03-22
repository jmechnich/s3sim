# s3sim
CPU/System Simulator of the GEM S2/S3 Turbo Synthesizer

## Features
* Memory map implemented to the point that execution seems to start running properly

## To do
* Model periphery (i.e. talking to DSPs, floppy controller, display, etc)
* External interrupt handling
* ...

## [Helper scripts](https://github.com/jmechnich/s3sim/tree/master/scripts)
* **combine.py**     combines even and odd ROM images
* **convert.py**     creates a header C header file from an u16be raw sample
* **disassemble.sh** uses gcc-m68k to convert ROM to ELF and dissambles it
* **mybin.py**       pretty prints integers in binary format
* **openROM.m**      load sample ROM images in GNU Octave
* **pad14.py**       sets bit 15 and 16 to zero for each word (big endian)
* **playAudios.m**   playAudiou.m: playback vector with signed/unsigned data in GNU Octave
* **player.m**       play sample ROM image from the commandline
