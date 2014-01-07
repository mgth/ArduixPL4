/*
  ArduixPL - xPL for arduino
  Copyright (c) 2012/2013 Mathieu GRENET.  All right reserved.

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

	  Modified 2013-1-22 by Mathieu GRENET 
	  mailto:mathieu@mgth.fr
	  http://www.mgth.fr
*/
/********************************************************************
Here goes all string constants, it may reduce rom size a bit, might be a better
way to handle that
*/

#ifndef ROMSTRINGS_H
#define ROMSTRINGS_H

#include <avr/pgmspace.h>
/*
#if !defined(pgm_get_far_address)
#undef PROGMEM
#define PROGMEM __attribute__(( section(".progmem.data") ))
#endif
*/
#define S(s) CStr::s_##s()

class CStr 
{
public:

#define C(s) static const __FlashStringHelper* s_##s()  { return F(#s); }; 
#define CF(f,s) static const __FlashStringHelper* s_##f()  { return F(s); }; 


#include "../romstrings.lst"
};



#endif


