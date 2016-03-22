# s3sim
CPU/System Simulator of the GEM S2/S3 Turbo Synthesizer based on Musashi.

## Background
The S2/S3 'Music Processor' is a synthesizer workstation which was build by the Italian company General Music (GEM) and released in 1992. It uses a Motorola 68302 CPU with 2MB RAM and its 'MIOS' operating system on EPROM chips. The original revision had a total of 1 MB of ROM code. A so called 'Turbo' kit was released some time later with a 2 MB ROM upgrade containing additional sound presets and a program extension for importing samples from MIDI and floppy disk. It is possible to load user programs from floppy disk to add functionality.

## Features
* Memory map implemented to the point that execution seems to start running properly

## To do
* Model periphery (i.e. talking to DSPs, floppy controller, display, etc)
* External interrupt handling
* ...

## Files

### Documentation
* [Checksums of ROM images](https://github.com/jmechnich/s3sim/blob/master/doc/rom_checksums.txt)

### [Helper scripts](https://github.com/jmechnich/s3sim/tree/master/scripts)
* **combine.py**     combines even and odd ROM images
* **convert.py**     creates a header C header file from an u16be raw sample
* **disassemble.sh** uses gcc-m68k to convert ROM to ELF and dissambles it
* **mybin.py**       pretty prints integers in binary format
* **openROM.m**      load sample ROM images in GNU Octave
* **pad14.py**       sets bit 15 and 16 to zero for each word (big endian)
* **playAudios.m**   playAudiou.m: playback vector with signed/unsigned data in GNU Octave
* **player.m**       play sample ROM image from the commandline
