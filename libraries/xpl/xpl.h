/*
  ArduixPL - xPL library for Arduino(tm)
  Copyright (c) 2012/2014 Mathieu GRENET.  All right reserved.

  This file is part of ArduixPL.

    ArduixPL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ArduixPL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ArduixPL.  If not, see <http://www.gnu.org/licenses/>.

	  Modified 2014-1-7 by Mathieu GRENET 
	  mailto:mathieu@mgth.fr
	  http://www.mgth.fr
*/
#ifndef XPL_H
#define XPL_H
#if defined(ARDUINO) && ARDUINO >= 100#include "Arduino.h"#else#include "WProgram.h"#endif

#include "setup.h"

#define GCC_VERSION (__GNUC__ * 10000 \
	+ __GNUC_MINOR__ * 100 \
	+ __GNUC_PATCHLEVEL__)


#if 1
//typedef const __FlashStringHelper* StringRom;
#include <avr/pgmspace.h>
#include "WString.h"
//typedef const __FlashStringHelper* StringRom;
#else
#include "WString.h"
typedef const String StringRom;
#ifdef F
#undef F
#define F(string_literal) string_literal
#endif
#endif

typedef  unsigned long time_t;

#if !defined(RAMSIZE)
#if defined(RAMSTART)
#define RAMSIZE RAMEND-RAMSTART
#else
#define RAMSIZE RAMEND-0x0FF
#endif
#endif


#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1284P__)


// RESET_PIN : pin used to restore default config, if this pin is HIGH at boot default config is loaded
// but the config is actually stored only if device is configured.


// Options for Arduino Mega
#ifndef RESET_PIN
#define RESET_PIN 46
#endif
#else

// Options for Arduino Uno
#ifndef RESET_PIN
#define RESET_PIN 7
#endif
#endif

#define MAX_GROUPS 3
#define MAX_FILTERS 3

#define LIGHTING_MAX_CHANNELS 16
#define LIGHTING_MAX_SCENES 32

#include "utility/romstrings.h"

#include "utility/debug.h"


class xPL
{
public:
	static StringRom vendor() { return (StringRom)F("arduixpl"); }
	static StringRom device() { return (StringRom)F("device"); }

};


#endif
