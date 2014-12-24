4x4x4 RGB LED Cube
========

A friend and I built a 4x4x4 RGB LED cube for a microcontroller class.  We designed our own PCB on which we mounted a UBW32 PIC board from [sparkfun](http://sparkfun.com), some MOSFETs, a few resistor packs and the LEDs.  With this design we can individually control each LED to display any of the basic 8 colors (Red/Green/Blue/Yellow/Magenta/Cyan/White/Off).  The hardware/control design is based loosely on [this instructables.com project](http://www.instructables.com/id/LED-Cube-4x4x4).  All the code is in C using the PIC32 libraries.

## Patterns
* Solid cube of each color
* Random column in random color for 20 seconds
* Rotate a green plane around the center axis
* Grey code bubble expand from center
* Random LED in random color for 30 seconds
* Rotate blue plan around the center axis
* Fireworks
* Swinging red plane
* Vertical slider of Red + Green + Blue planes
* Pulse (fill to color --> decay to all off --> fill to next color etc)
* Wireframe cube in each color
* Horizontal slider in Cyan + Magenta + Yellow
* Fill one LED at a time to white, then decay to off
* Diagonal slider from different top corners in Red + Blue + Green + Yellow
* Snake

## Comments/Ideas
If you have any comments or clever pattern ideas, fell free to open an issue!
