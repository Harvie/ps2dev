# Arduino ps2dev library
This library allows arduino to emulate ps2 keyboard and/or mouse, so you can implement your own ps2 keyboard or whatever...

I want to improve this, provide better documentation and arduino library manager package for it...

Originaly from:
 * https://github.com/grappendorf/arduino-framework/tree/master/ps2dev
 * https://github.com/dpavlin/Arduino-projects/tree/master/libraries/ps2dev
 * Written by Chris J. Kiick, January 2008. https://github.com/ckiick
 * modified by Gene E. Scogin, August 2008.
 * Release into public domain.

Further reading:
 * https://www.avrfreaks.net/sites/default/files/PS2%20Keyboard.pdf (PS2 specification)
 * https://github.com/ndusart/ps2-keyboard (similar library using timers instead of delays)

# Notes

 * If you are implementing keyboard you should read the commands from computer quite often. I don't know how often, but 1 second is not enough. You will miss stuff like num/caps/scroll LED changes and maybe even more important stuff needed for proper ps2 protocol initialisation.
 * Calling `keyboard_handle(...);` each 10ms should be enough...

# TODO

 * Add mode, where attachInterrupt() will be used to watch CLK pin instead of need to constantly poll it using available() or keyboard_handle()
 * Add mouse protocol handling directly to library
 * Add scancode table
 * Check against Arduino API style guide: https://www.arduino.cc/en/Reference/APIStyleGuide
