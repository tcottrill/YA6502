I'm calling this "Yet Another 6502 CPU core in C++" YA6502 for short. 

This is the code and a demo program for my 6502 CPU emulator. I needed something that was compatible with the older code by Neil Bradly, specifically for emulating arcade game, but I also wanted it to be able to be compiled into modern 64 bit code. 
This code had been tested fairly rigorously and will run multiple 6502 CPU cores in the Major Havoc arcade game with no issue. 
This isn't the fastest CPU core available, since all stack and zero page accesses are required to go through the memory handlers, but that was required for maximum compatibility (especially Major Havoc)!

A very bare bones demo of asteroids is bundled with the cpu core so you can see how it is used. It requires the roms from the latest MAME (TM) "asteroid" romset to run. (Not included).
Visual Studio 2019 or higher is required to compile and run. 

If this is used with an NES Emulator, the ADC and SBC code will need to be modified. 

Acknowledgements:

Timing code taken from Fake6502
http://rubbermallet.org/fake6502.c 

SBC and ADC code taken from M.A.M.E (TM)
https://www.mamedev.org/

The rest based on the original C version of CPU code by Neil Bradley.
https://www.zophar.net/6502/m6502.html

 

