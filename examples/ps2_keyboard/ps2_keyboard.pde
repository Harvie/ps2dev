#include <ps2dev.h>

PS2dev keyboard(3, 2); //clock, data

void setup() {
}

void loop() {
  keyboard.write(0x34); //Make
  delay(100);
  keyboard.write(0xF0); //Break
  delay(100);
  keyboard.write(0x34);
  delay(1000);
}
