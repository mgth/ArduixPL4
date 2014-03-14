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
#include <WString.h>

//typedef const char* StringRom;

typedef const __FlashStringHelper* StringRom;

/**
* Create constant string in program memory. Allow IOStream output
* operator. Allows link time reduction of equal strings or substrings.
* @param[in] s string literal (at compile time).
* @return string literal in program memory.
*/
#if 1

#undef PSTR
#define PSTR(str) __PSTR1(str,__COUNTER__)
#define __PSTR1(str,num) __PSTR2(str,num)
#define __PSTR2(str,num) \
	(__extension__(\
{ \
	const char* ptr; \
	asm volatile (\
	".pushsection .progmem.data, \"SM\", @progbits, 1" "\n\t" \
	"PSTR_" #num ": .string " #str "\n\t" \
	".popsection" "\n\t" \
	); \
	asm volatile (\
	"ldi %A0, lo8(PSTR_" #num ")" "\n\t" \
	"ldi %B0, hi8(PSTR_" #num ")" "\n\t" \
	: "=d" (ptr) \
	); \
	ptr; \
} \
	))
	
//#define S(string) F(#string)

typedef enum ConstString_t
#ifdef __GXX_EXPERIMENTAL_CXX0X__
: unsigned char
#endif
{
	cs_stat,
	cs_trig,
	cs_cmnd,
	cs_reconf,
	cs_option,
	cs_device,
	cs_type
};

StringRom ConstString(ConstString_t cs);

#else


#define S(string) (StringRom)s_##string

#define C(string)      extern  const char s_##string[] ;
#define CF(var,string) extern  const char s_##var[] ;

#include "../romstrings.lst"

#undef C
#undef CF
#define C(string) cs_##string ,
#define CF(var,string) cs_##var ,

typedef enum ConstString_t 
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	: unsigned char
#endif
{
	cs_stat,
	cs_trig,
	cs_cmnd
};

#define CS(string) ConstString(cs_##string)

StringRom ConstString(ConstString_t cs);

#endif

#endif


