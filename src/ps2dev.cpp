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
//#define _PS2DBG Serial

#define BYTE_INTERVAL_MICROS 100

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
  leds = 0;
  handling_io_abort = false;
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

int PS2dev::do_write(unsigned char data)
{
  int ret;
  if ((ret = write(data)) == EABORT && !handling_io_abort) {
    handling_io_abort = true;
    keyboard_handle(&leds);
    handling_io_abort = false;
  }
  return ret;
}

int PS2dev::write(unsigned char data)
{
  unsigned char i;
  unsigned char parity = 1;

#ifdef _PS2DBG
  _PS2DBG.print(F("sending "));
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
      if (digitalRead(_ps2clk) == LOW) { /* I/O request from host */
        /* Abort */
        gohi(_ps2data);
        return EABORT;
      }
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
  if (digitalRead(_ps2clk) == LOW) { /* I/O request from host */
    /* Abort */
    gohi(_ps2data);
    return EABORT;
  }

  // stop bit
  gohi(_ps2data);
  delayMicroseconds(CLKHALF);
  golo(_ps2clk);
  delayMicroseconds(CLKFULL);
  gohi(_ps2clk);
  delayMicroseconds(CLKHALF);

  delayMicroseconds(BYTEWAIT);

#ifdef _PS2DBG
  _PS2DBG.print(F("sent "));
  _PS2DBG.println(data,HEX);
#endif

  return ENOERR;
}

int PS2dev::available() {
  //delayMicroseconds(BYTEWAIT);
  return ( (digitalRead(_ps2data) == LOW) || (digitalRead(_ps2clk) == LOW) );
}

int PS2dev::do_read(unsigned char * value)
{
  int ret;
  if ((ret = read(value)) == EABORT && !handling_io_abort) {
    handling_io_abort = true;
    keyboard_handle(&leds);
    handling_io_abort = false;
  }
  return ret;
}

int PS2dev::read(unsigned char * value)
{
  unsigned int data = 0x00;
  unsigned int bit = 0x01;

  unsigned char calculated_parity = 1;
  unsigned char received_parity = 0;

  //wait for data line to go low and clock line to go high (or timeout)
  unsigned long waiting_since = millis();
  while((digitalRead(_ps2data) != LOW) || (digitalRead(_ps2clk) != HIGH)) {
    if (!available()) return ECANCEL; /* Cancelled */
    if((millis() - waiting_since) > TIMEOUT) return ETIMEOUT;
  }

  delayMicroseconds(CLKHALF);
  golo(_ps2clk);
  delayMicroseconds(CLKFULL);
  gohi(_ps2clk);
  delayMicroseconds(CLKHALF);

  if (digitalRead(_ps2clk) == LOW) { /* I/O request from host */
    /* Abort */
    return EABORT;
  }

  while (bit < 0x0100) {
    if (digitalRead(_ps2data) == HIGH)
      {
        data = data | bit;
        calculated_parity = calculated_parity ^ 1;
      } else {
        calculated_parity = calculated_parity ^ 0;
      }

    bit = bit << 1;

    delayMicroseconds(CLKHALF);
    golo(_ps2clk);
    delayMicroseconds(CLKFULL);
    gohi(_ps2clk);
    delayMicroseconds(CLKHALF);

    if (digitalRead(_ps2clk) == LOW) { /* I/O request from host */
      /* Abort */
      return EABORT;
    }
  }
  // we do the delay at the end of the loop, so at this point we have
  // already done the delay for the parity bit

  // parity bit
  if (digitalRead(_ps2data) == HIGH)
    {
      received_parity = 1;
    }

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


  *value = data & 0x00FF;

#ifdef _PS2DBG
  _PS2DBG.print(F("received data "));
  _PS2DBG.println(*value,HEX);
  _PS2DBG.print(F("received parity "));
  _PS2DBG.print(received_parity,BIN);
  _PS2DBG.print(F(" calculated parity "));
  _PS2DBG.println(received_parity,BIN);
#endif
  if (received_parity == calculated_parity) {
    return ENOERR;
  } else {
    return ECANCEL;
  }

}


void PS2dev::keyboard_init()
{
  delay(200);
  write(0xAA);
  return;
}

void PS2dev::ack()
{
  delayMicroseconds(BYTE_INTERVAL_MICROS);
  write(0xFA);
  delayMicroseconds(BYTE_INTERVAL_MICROS);
  return;
}

int PS2dev::keyboard_reply(unsigned char cmd, unsigned char *leds_)
{
  unsigned char val;
  switch (cmd)
  {
  case 0xFF: //reset
    ack();
    //the while loop lets us wait for the host to be ready
    while (write(0xFA)!=0) delay(1); //send ACK
    while (write(0xAA) != 0) delay(1); // send BAT_SUCCESS
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
    ack();
    break;
  case 0xF4: //enable data reporting
    //FM
    ack();
    break;
  case 0xF3: //set typematic rate
    ack();
    if(!read(&val)) ack(); //do nothing with the rate
    break;
  case 0xF2: //get device id
    ack();
    do {
      // ensure ID gets written, some hosts may be sensitive
      if (do_write(0xAB) == EABORT) continue;
      // this is critical for combined ports (they decide mouse/kb on this)
      if (do_write(0x83) == EABORT) continue;
      break;
    } while (!handling_io_abort);
    break;
  case 0xF0: //set scan code set
    ack();
    if(!read(&val)) ack(); //do nothing with the rate
    break;
  case 0xEE: //echo
    //ack();
    delayMicroseconds(BYTE_INTERVAL_MICROS);
    write(0xEE);
    delayMicroseconds(BYTE_INTERVAL_MICROS);
    break;
  case 0xED: //set/reset LEDs
    //ack();
    while (write(0xFA) != 0) delay(1);
    if(!read(&leds)) {
       while (write(0xFA) != 0) delay(1);
    } 
#ifdef _PS2DBG
    _PS2DBG.print("LEDs: ");
    _PS2DBG.println(leds, HEX);
    //digitalWrite(LED_BUILTIN, leds);
#endif
	*leds_ = leds;
    return 1;
    break;
  }
  return 0;
}

int PS2dev::keyboard_handle(unsigned char *leds_) {
  unsigned char c;  //char stores data recieved from computer for KBD
  if(available())
  {
    if(!read(&c)) return keyboard_reply(c, leds_);
  }
  return 0;
}

// Presses then releases one of the non-special characters
int PS2dev::keyboard_mkbrk(unsigned char code)
{
	do {
		if (do_write(code) == EABORT) continue;
		break;
	} while (!handling_io_abort);
	do {
		if (do_write(0xF0) == EABORT) continue;
		if (do_write(code) == EABORT) continue;
		break;
	} while (!handling_io_abort);

  return 0;
}

// Presses one of the non-special characters
int PS2dev::keyboard_press(unsigned char code)
{
	do {
		if (do_write(code) == EABORT) continue;
		break;
	} while (!handling_io_abort);

	return 0;
}

// Releases one of the non-special characters
int PS2dev::keyboard_release(unsigned char code)
{
	do {
		if (do_write(0xf0) == EABORT) continue;
		if (do_write(code) == EABORT) continue;
		break;
	} while (!handling_io_abort);

	return 0;
}

// Presses one of the special characters
int PS2dev::keyboard_press_special(unsigned char code)
{
	do {
		if (do_write(0xe0) == EABORT) continue;
		if (do_write(code) == EABORT) continue;
		break;
	} while (!handling_io_abort);

	return 0;
}

// Releases one of the special characters
int PS2dev::keyboard_release_special(unsigned char code)
{
	do {
		if (do_write(0xe0) == EABORT) continue;
		if (do_write(0xf0) == EABORT) continue;
		if (do_write(code) == EABORT) continue;
		break;
	} while (!handling_io_abort);

	return 0;
}

// Presses then releases one of the special characters
int PS2dev::keyboard_special_mkbrk(unsigned char code)
{
	do {
		if (do_write(0xe0) == EABORT) continue;
		if (do_write(code) == EABORT) continue;
		break;
	} while (!handling_io_abort);
	do {
		if (do_write(0xe0) == EABORT) continue;
		if (do_write(0xf0) == EABORT) continue;
		if (do_write(code) == EABORT) continue;
		break;
	} while (!handling_io_abort);

	return 0;
}

// Presses Printscreen
int PS2dev::keyboard_press_printscreen()
{
	do {
		if (do_write(0xe0) == EABORT) continue;
		if (do_write(0x12) == EABORT) continue;
		if (do_write(0xe0) == EABORT) continue;
		if (do_write(0x7c) == EABORT) continue;
		break;
	} while (!handling_io_abort);


	return 0;
}

// Releases Printscreen
int PS2dev::keyboard_release_printscreen()
{
	do {
		if (do_write(0xe0) == EABORT) continue;
		if (do_write(0xf0) == EABORT) continue;
		if (do_write(0x7c) == EABORT) continue;
		if (do_write(0xe0) == EABORT) continue;
		if (do_write(0xf0) == EABORT) continue;
		if (do_write(0x12) == EABORT) continue;
		break;
	} while (!handling_io_abort);

	return 0;
}

// Presses then releases Printscreen
int PS2dev::keyboard_mkbrk_printscreen()
{
	keyboard_press_printscreen();
	keyboard_release_printscreen();

	return 0;
}

// Presses/Releases Pause/Break
int PS2dev::keyboard_pausebreak()
{
	do {
		if (do_write(0xe1) == EABORT) continue;
		if (do_write(0x14) == EABORT) continue;
		if (do_write(0x77) == EABORT) continue;
		break;
	} while (!handling_io_abort);
	do {
		if (do_write(0xe1) == EABORT) continue;
		if (do_write(0xf0) == EABORT) continue;
		if (do_write(0x14) == EABORT) continue;
		if (do_write(0xe0) == EABORT) continue;
		if (do_write(0x77) == EABORT) continue;
		break;
	} while (!handling_io_abort);

	return 0;
}
