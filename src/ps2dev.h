/*
 * ps2dev.h - a library to interface with ps2 hosts. See comments in
 * ps2dev.cpp.
 * Written by Chris J. Kiick, January 2008.
 * modified by Gene E. Scogin, August 2008.
 * modified by Tomas 'Harvie' Mudrunka 2019.
 * modified once more by Iggy Glassman 2020.
 * Release into public domain.
 */

#ifndef ps2dev_h
#define ps2dev_h

#include "Arduino.h"
#include <avr/io.h>

class PS2dev
{
	public:
		PS2dev(int clk, int data);

		enum ScanCodes // Print Screen and Pause/Break are multiple sequences
		{
			ESCAPE = 0x76,
			F1 = 0x05,
			F2 = 0x06,
			F3 = 0x04,
			F4 = 0x0c,
			F5 = 0x03,
			F6 = 0x0b,
			F7 = 0x83,
			F8 = 0x0a,
			F9 = 0x01,
			F10 = 0x09,
			F11 = 0x78,
			F12 = 0x07,
			SCROLL_LOCK = 0x7e,
			ACCENT = 0x0e,
			ONE = 0x16,
			TWO = 0x1e,
			THREE = 0x26,
			FOUR = 0x25,
			FIVE = 0x2e,
			SIX = 0x36,
			SEVEN = 0x3d,
			EIGHT = 0x3e,
			NINE = 0x46,
			ZERO = 0x45,
			MINUS = 0x4e,
			EQUAL = 0x55,
			BACKSPACE = 0x66,
			TAB = 0x0d,
			Q = 0x15,
			W = 0x1d,
			E = 0x24,
			R = 2d,
			T = 0x2c,
			Y = 0x35,
			U = 0x3c,
			I = 0x43,
			O = 0x44,
			P = 0x4d,
			OPEN_BRACKET = 0x54,
			CLOSE_BRACKET = 0x5b,
			BACKSLASH = 0x5d,
			CAPS_LOCK = 0x58,
			A = 0x1c,
			S = 0x1b,
			D = 0x23,
			F = 0x2b,
			G = 0x34,
			H = 0x33,
			J = 0x3b,
			K = 0x42,
			L = 0x4b,
			SEMI_COLON = 0x4c,
			TICK_MARK = 0x52,
			ENTER = 0x5a,
			LEFT_SHIFT = 0x12,
			Z = 0x1a,
			X = 0x22,
			C = 0x21,
			V = 0x2a,
			B = 0x32,
			N = 0x31,
			M = 0x3a,
			COMMA = 0x41,
			PERIOD = 0x49,
			SLASH = 4a,
			RIGHT_SHIFT = 0x59,
			LEFT_CONTROL = 0x14,
		};

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

