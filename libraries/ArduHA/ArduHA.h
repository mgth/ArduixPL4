/*
  ArduHA - ArduixPL - xPL library for Arduino(tm)
  Copyright (c) 2012/2014 Mathieu GRENET.  All right reserved.

  This file is part of ArduHA / ArduixPL.

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

	  Modified 2014-3-14 by Mathieu GRENET 
	  mailto:mathieu@mgth.fr
	  http://www.mgth.fr
*/
#ifndef ARDUHA_H
#define ARDUHA_H
#if defined(ARDUINO) && ARDUINO >= 100#include "Arduino.h"#else#include "WProgram.h"#endif

typedef  unsigned long time_t;

#define GCC_VERSION (__GNUC__ * 10000 \
	+ __GNUC_MINOR__ * 100 \
	+ __GNUC_PATCHLEVEL__)

#if !defined(RAMSIZE)
#if defined(RAMSTART)
#define RAMSIZE RAMEND-RAMSTART
#else
#define RAMSIZE RAMEND-0x0FF
#endif
#endif

#include "float.h"
#include "limits.h"
template<typename T> T maxValue() { return 0; }
template<> inline double maxValue<double>()  { return DBL_MAX; }
template<> inline int maxValue<int>()  { return INT_MAX; }
template<> inline unsigned int maxValue<unsigned int>()  { return UINT_MAX; }
template<> inline long maxValue<long>()  { return LONG_MAX; }
template<> inline unsigned long maxValue<unsigned long>()  { return ULONG_MAX; }
template<typename T> T minValue() { return 0; }
template<> inline double minValue<double>()  { return DBL_MIN; }
template<> inline int minValue<int>()  { return INT_MIN; }
template<> inline long minValue<long>()  { return LONG_MIN; }

#include "ha_setup.h"

#endif