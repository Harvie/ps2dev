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

char scancodes[] = {
  PS2dev::ZERO,
  PS2dev::ONE,
  PS2dev::TWO,
  PS2dev::THREE,
  PS2dev::FOUR,
  PS2dev::FIVE,
  PS2dev::SIX,
  PS2dev::SEVEN,
  PS2dev::EIGHT,
  PS2dev::NINE,
  PS2dev::ASTERISK
};

void loop() {
  for(int i = 0; i < 11; i++) {
    make_break_kb(scancodes[i]);
    delay(1000);
  }
}
