/*
 * ps2dev.cpp - an interface library for ps2 host.
 * limitations:
 *      we do not handle parity errors.
 *      The timing constants are hard coded from the spec. Data rate is
 *         not impressive.
 *      probably lots of room for optimization.
 */

#include "ps2dev.h"

//Enable serial debug mode?
#define _PS2DBG Serial

//since for the device side we are going to be in charge of the clock,
//the two defines below are how long each _phase_ of the clock cycle is
#define CLKFULL 40
// we make changes in the middle of a phase, this how long from the
// start of phase to the when we drive the data line
#define CLKHALF 20

// Delay between bytes
// I've found i need at least 400us to get this working at all,
// but even more is needed for reliability, so i've put 1000us
#define BYTEWAIT 1000

// Timeout if computer not sending for 30ms
#define TIMEOUT 30


/*
 * the clock and data pins can be wired directly to the clk and data pins
 * of the PS2 connector.  No external parts are needed.
 */
PS2dev::PS2dev(int clk, int data)
{
  _ps2clk = clk;
  _ps2data = data;
  gohi(_ps2clk);
  gohi(_ps2data);
}

/*
 * according to some code I saw, these functions will
 * correctly set the clock and data pins for
 * various conditions.  It's done this way so you don't need
 * pullup resistors.
 */
void
PS2dev::gohi(int pin)
{
  pinMode(pin, INPUT);
  digitalWrite(pin, HIGH);
}

void
PS2dev::golo(int pin)
{
  digitalWrite(pin, LOW);
  pinMode(pin, OUTPUT);
}

int PS2dev::write(unsigned char data)
{
  delayMicroseconds(BYTEWAIT);

  unsigned char i;
  unsigned char parity = 1;

#ifdef _PS2DBG
  _PS2DBG.print("sending ");
  _PS2DBG.println(data,HEX);
#endif

  if (digitalRead(_ps2clk) == LOW) {
    return -1;
  }

  if (digitalRead(_ps2data) == LOW) {
    return -2;
  }

#ifdef _PS2DBG
  _PS2DBG.print("sent ");
  _PS2DBG.println(data,HEX);
#endif

  golo(_ps2data);
  delayMicroseconds(CLKHALF);
  // device sends on falling clock
  golo(_ps2clk);	// start bit
  delayMicroseconds(CLKFULL);
  gohi(_ps2clk);
  delayMicroseconds(CLKHALF);

  for (i=0; i < 8; i++)
    {
      if (data & 0x01)
	{
	  gohi(_ps2data);
	} else {
	golo(_ps2data);
      }
      delayMicroseconds(CLKHALF);
      golo(_ps2clk);
      delayMicroseconds(CLKFULL);
      gohi(_ps2clk);
      delayMicroseconds(CLKHALF);

      parity = parity ^ (data & 0x01);
      data = data >> 1;
    }
  // parity bit
  if (parity)
    {
      gohi(_ps2data);
    } else {
    golo(_ps2data);
  }
  delayMicroseconds(CLKHALF);
  golo(_ps2clk);
  delayMicroseconds(CLKFULL);
  gohi(_ps2clk);
  delayMicroseconds(CLKHALF);

  // stop bit
  gohi(_ps2data);
  delayMicroseconds(CLKHALF);
  golo(_ps2clk);
  delayMicroseconds(CLKFULL);
  gohi(_ps2clk);
  delayMicroseconds(CLKHALF);

  delayMicroseconds(BYTEWAIT);

  return 0;
}

int PS2dev::available() {
  //delayMicroseconds(BYTEWAIT);
  //return ( (digitalRead(_ps2data) == LOW) || (digitalRead(_ps2clk) == LOW) );
  return ( (digitalRead(_ps2clk) == LOW) );
}

int PS2dev::read(unsigned char * value)
{
  unsigned char data = 0x00;
  unsigned char i;
  unsigned char bit = 0x01;

  unsigned char parity = 1;

  //wait for data line to go low and clock line to go high (or timeout)
  unsigned long waiting_since = millis();
  while((digitalRead(_ps2data) != LOW) || (digitalRead(_ps2clk) != HIGH)) {
    if((millis() - waiting_since) > TIMEOUT) return -1;
  }

  delayMicroseconds(CLKHALF);
  golo(_ps2clk);
  delayMicroseconds(CLKFULL);
  gohi(_ps2clk);
  delayMicroseconds(CLKHALF);

  for (i=0; i < 8; i++)
    {
      if (digitalRead(_ps2data) == HIGH)
	{
	  data = data | bit;
	} else {
      }


      bit = bit << 1;

      delayMicroseconds(CLKHALF);
      golo(_ps2clk);
      delayMicroseconds(CLKFULL);
      gohi(_ps2clk);
      delayMicroseconds(CLKHALF);

      parity = parity ^ (data & 0x01);
    }
  // we do the delay at the end of the loop, so at this point we have
  // already done the delay for the parity bit

  // stop bit
  delayMicroseconds(CLKHALF);
  golo(_ps2clk);
  delayMicroseconds(CLKFULL);
  gohi(_ps2clk);
  delayMicroseconds(CLKHALF);


  delayMicroseconds(CLKHALF);
  golo(_ps2data);
  golo(_ps2clk);
  delayMicroseconds(CLKFULL);
  gohi(_ps2clk);
  delayMicroseconds(CLKHALF);
  gohi(_ps2data);


  *value = data;

#ifdef _PS2DBG
  _PS2DBG.print("Recv ");
  _PS2DBG.println(*value,HEX);
#endif

  return 0;
}


void PS2dev::keyboard_init()
{
  while(write(0xAA)!=0);
  delay(10);
  return;
}

void PS2dev::ack()
{
  while(write(0xFA));
  return;
}

int PS2dev::keyboard_reply(unsigned char cmd, unsigned char *leds)
{
  unsigned char val;
  unsigned char enabled;
  switch (cmd)
  {
  case 0xFF: //reset
    ack();
    //the while loop lets us wait for the host to be ready
    while(write(0xAA)!=0);
    break;
  case 0xFE: //resend
    ack();
    break;
  case 0xF6: //set defaults
    //enter stream mode
    ack();
    break;
  case 0xF5: //disable data reporting
    //FM
    enabled = 0;
    ack();
    break;
  case 0xF4: //enable data reporting
    //FM
    enabled = 1;
    ack();
    break;
  case 0xF3: //set typematic rate
    ack();
    if(!read(&val)) ack(); //do nothing with the rate
    break;
  case 0xF2: //get device id
    ack();
    write(0xAB);
    write(0x83);
    break;
  case 0xF0: //set scan code set
    ack();
    if(!read(&val)) ack(); //do nothing with the rate
    break;
  case 0xEE: //echo
    //ack();
    write(0xEE);
    break;
  case 0xED: //set/reset LEDs
    ack();
    if(!read(leds)) ack(); //do nothing with the rate
#ifdef _PS2DBG
    _PS2DBG.print("LEDs: ");
    _PS2DBG.println(*leds, HEX);
    //digitalWrite(LED_BUILTIN, *leds);
#endif
    return 1;
    break;
  }
  return 0;
}

int PS2dev::keyboard_handle(unsigned char *leds) {
  unsigned char c;  //char stores data recieved from computer for KBD
  if(available())
  {
    if(!read(&c)) return keyboard_reply(c, leds);
  }
  return 0;
}

int PS2dev::keyboard_mkbrk(unsigned char code)
{
  write(code);
  write(0xF0);
  write(code);
  return 0;
}
