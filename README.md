# MSP430Player
MSP430 Wave Player


Program that play wavs files from an microSD card using an MSP430g2553 micro.
This little project is used to give a Spock bobble head more samples to play, a lot more!!

Description:

    The code uses PWM to output a wav sample to a speaker (or head phones). Using the Petit FAT file system
    the program will a play wave file stored on an SD card via SPI mode.
    
Limitations:

    - The SD card is formated as FAT32.
    - The code only scans for .wav files at the root directory. 
    - The files must a mono 11025KHz wav file.

Parts:

    MSP430g2553 (obivouisly)
    MicroSD card holder
    MicroSD card (any size I guess)
    Resistors, capacitors (see the Schematic diagram in the folder.

CREDIT:

 - Most of this code (if not all) was heavily inspired by the giants at 43oh.com
   http://forum.43oh.com/topic/1427-petitefs-fatfs-sdcard-usci-code/
   Also, the great 43oh.com code vault was in invaluable tool that got this project going.

 - elm-chan's Petit FAT file system: http://elm-chan.org/fsw/ff/00index_p.html




