#include <ps2dev.h>    //Emulate a PS/2 device
PS2dev keyboard(3,2);  //clock, data

unsigned long timecount = 0;

void setup()
{
  keyboard.keyboard_init();
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  //Handle PS2 communication and react to keyboard led change
  //This should be done at least once each 10ms
  unsigned char leds;
  if(keyboard.keyboard_handle(&leds)) {
    //Serial.print('LEDS');
    //Serial.print(leds, HEX);
    digitalWrite(LED_BUILTIN, leds);
  }

  //Print a number every second
  if((millis() - timecount) > 1000) {
    keyboard.keyboard_mkbrk(PS2dev::ONE);
    Serial.print('.');
    timecount = millis();
  }
}
