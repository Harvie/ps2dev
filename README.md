# Arduino ps2dev library
This library allows arduino to emulate ps2 keyboard and/or mouse, so you can implement your own ps2 keyboard or whatever...

I want to improve this, provide better documentation and arduino library manager package for it...

Originaly from:
 * https://playground.arduino.cc/ComponentLib/Ps2mouse/
 * https://github.com/grappendorf/arduino-framework/tree/master/ps2dev
 * https://github.com/dpavlin/Arduino-projects/tree/master/libraries/ps2dev
 * Written by Chris J. Kiick, January 2008. https://github.com/ckiick
 * modified by Gene E. Scogin, August 2008.
 * modified by Tomas 'Harvie' Mudrunka 2019. https://github.com/harvie/ps2dev
 * Release into public domain.

Further reading:
 * https://www.arduinolibraries.info/libraries/ps2dev
 * https://libraries.io/platformio/ps2dev (might not be up to date?)
 * https://www.avrfreaks.net/sites/default/files/PS2%20Keyboard.pdf (PS2 specification)
 * https://github.com/ndusart/ps2-keyboard (similar library using timers instead of delays)

# Electrical connections

The following is an easy way to get started:

 * Take a PS/2 mouse apart without cutting any cables.
 * Desolder the socket that the PS/2 cable is connected to.
 * Use a multimeter to find the pinout of the socket.
 * Solder the socket to an Arduino proto shield. If the pitch does not match you can also consider simply using mounting the socket vertically with super glue and soldering wires directly to the socket.
 * Solder three wires on the Arduino proto shield as follows.

| Arduino pin | PS/2 pin | example color (on a HP mouse) |
| ----------- | -------- | ----------------------------- |
|      2      |   DATA   |           brown               |
|      3      |   CLK    |           orange              |
|     GND     |   GND    |           green               |

There is no need to connect the VCC. If you want to use different pins on the Arduino side you can just adjust the call to the PS2dev constructor. In theory this should allow you to emulate multiple keyboard on one Arduino but this has not been tested.

# Notes

 * If you are implementing keyboard you should read the commands from computer quite often. I don't know how often, but 1 second is not enough. You will miss stuff like num/caps/scroll LED changes and maybe even more important stuff needed for proper ps2 protocol initialisation.
 * Calling `keyboard_handle(...);` each 10ms should be enough...
 * Serial debug is not intended to be enabled in production code. Use it only during development. Otherwise it might mess up the protocol timing.

# TODO

 * Move `leds` variable to the ps2dev class
 * Add mode, where attachInterrupt() will be used to watch CLK pin instead of need to constantly poll it using available() or keyboard_handle()
 * Add mouse protocol handling directly to library
 * Check against Arduino API style guide: https://www.arduino.cc/en/Reference/APIStyleGuide
 * Check why there are occasional issues with PS2Keyboard library https://github.com/PaulStoffregen/PS2Keyboard
   * I've been using this to send keystrokes between two arduinos (from ps2dev to PS2Keyboard) and there are some issues (missed keystrokes). So i had to use SoftwareSerial.h instead, ditching the PS2 compatibility. We need to debug this...

# Special thanks

 * Cleveron https://cleveron.cz/
 * Spoje.NET http://spoje.net/
 * Brmlab Hackerspace Prague http://brmlab.cz/
