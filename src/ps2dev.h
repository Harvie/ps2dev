/*
 * ps2dev.h - a library to interface with ps2 hosts. See comments in
 * ps2.cpp.
 * Written by Chris J. Kiick, January 2008.
 * modified by Gene E. Scogin, August 2008.
 * Release into public domain.
 */

#ifndef ps2dev_h
#define ps2dev_h

#include "Arduino.h"

class PS2dev
{
	public:
		PS2dev(int clk, int data);
		int write(unsigned char data);
		int read(unsigned char * data);
		int available();
		void keyboard_init();
		int keyboard_reply(unsigned char cmd, unsigned char *leds);
		int keyboard_handle(unsigned char *leds);
		int keyboard_mkbrk(unsigned char code);
	private:
		int _ps2clk;
		int _ps2data;
		void golo(int pin);
		void gohi(int pin);
		void ack();
};

#endif /* ps2dev_h */

