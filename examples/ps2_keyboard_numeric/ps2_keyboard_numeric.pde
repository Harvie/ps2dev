#include <ps2dev.h>

PS2dev keyboard(3, 2); //clock, data

void make_break_kb(byte data)
{
// makes and breaks the key
    keyboard.write(data);
    delay(50);
    keyboard.write(0xF0);
    delay(50);
    keyboard.write(data);
    delay(50);
}

void setup() {
}

char scancodes[] = {0x45, 0x16, 0x1E, 0x26, 0x25, 0x2E, 0x36, 0x3D, 0x3E, 0x46, 0x7C, 0x7C}; //Scancodes for numbers 0-9, *, #

void loop() {
  for(int i = 0; i < 11; i++) {
    make_break_kb(scancodes[i]);
    delay(1000);
  }
}
