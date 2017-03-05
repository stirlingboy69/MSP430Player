# MSP430Player
MSP430 Wave Player


Program that play wavs files from an microSD card using an MSP430g2553 micro.
This little project is used to give a Spock bobble head more samples to play, a lot more!!

Description program:

    The code uses PWM to output a wave sample to a speaker (or head phones). Using the Petit FAT file system,
    the program scans the sd card via SPI mode and plays a random wave file. The "random" number is generated
    by using a Linear-feedback shift register code (see wiki ref. at bottom). After the wave is played it stores
    the lfsr number on SegC of the micro and goes into low power mode. When the reed switch is toggle (i.e. Spocks
    head bobble's?) the next wave is played.

Compiling the code:

    I am using Code Composser Studio Version: 5.5.0.00077 (although it may compile with a later version). 
    Just import the code and compile for debug. Compiling in release didn't work. The optimization was causing
    the timer interrupt to not fire, couldn't figure out why so I left as a debug build.
    
    Getting the code to compile using msp430-gcc was also problematic. Even after I converted the timer interrupt
    to use gcc's syntax, it still didn't work. If anyone else can get it to work msp430-gcc I like know about it.

Limitations of program:

    - The SD card is formated as FAT32.
    - The code only scans for .wav files at the root directory. 
    - The files must a mono 11025KHz wav file.

Building the circuit:

    I am using a breadboard to wire it all up. It's fairly straight forward (see schematic). I would like to 
    make a PCB for this circuit, but I'll settle for a simple perf board for now. Maybe I can get my friend
    to help me out with the Eagle software as I just still learning it.
    

Parts:

    MSP430g2553 micro (obviously)
    MicroSD card holder
    MicroSD card (Tested on some old 256Meg an 1G cards. Any size should work.)
    Resistors
    Capacitors
    Reed switch, push button (see the Schematic circuit layout).

CREDIT:

   - Most of this code (if not all) was heavily inspired by the giants at 43oh.com
 
    ( see http://forum.43oh.com/topic/1427-petitefs-fatfs-sdcard-usci-code )
   
   - Also, the great 43oh.com code vault was in invaluable tool that got this project going.

   - elm-chan's Petit FAT file system: http://elm-chan.org/fsw/ff/00index_p.html

   - https://en.wikipedia.org/wiki/Linear-feedback_shift_register


