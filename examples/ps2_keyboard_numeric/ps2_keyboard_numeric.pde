#include <ps2dev.h>

PS2dev keyboard(3, 2); //clock, data

unsigned long timecount = 0;
int scancodes_idx = 0;

void setup() {
  keyboard.keyboard_init();
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
  unsigned char leds;
  if(keyboard.keyboard_handle(&leds)) {
    Serial.print('LEDS');
    Serial.print(leds, HEX);
    digitalWrite(LED_BUILTIN, leds);
  }

  if (millis() - timecount > 1000) {
    keyboard.keyboard_mkbrk(scancodes[scancodes_idx++]);
    if (scancodes_idx >= sizeof(scancodes)) {
      scancodes_idx = 0;
    }
    timecount = millis();
  }
}
