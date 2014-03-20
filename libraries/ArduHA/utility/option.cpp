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
#include <avr/crc16.h>
#include <avr/eeprom.h>


#include "option.h"
#include "task.h"

bool Option::configured = true;

//retrieve byte at option position
byte Option::get(byte pos) const { return eeprom_read_byte(addr(pos)); }
void Option::set(byte pos, byte b) const
{
	if (b != get(pos))
	{
		eeprom_write_byte(addr(pos), b);
	}
}

// store next byte to eeprom
// returns true if not finished otherwise false
bool OptionWriter::store(byte b)
{

	if (_pos >= _option.size()) return false; // secure to not overflow
	_option.set(_pos, b);
	_pos++;
	_crc_ibutton_update(_crc, b); // update crc with new value;

	if (_pos < _option.size()) return true; //object writing not finisned

	_option.set(_pos,_crc); // object writing finished, store Crc.

	return false;
}

// read next byte from eeprom
// returns -1 if finished ok
// return -2 if checksum is bad
int OptionWriter::read()
{

	byte b = _option.get(_pos++);
	
	if (_pos <= _option.size())
	{
		_crc_ibutton_update(_crc, b);
		return b;
	}
	if (b == _crc) return -1;
	return -2;

}

// Store binary object to eeprom
void Option::storeObj(void* obj) //108
{
	char* addr = (char*)obj;
	OptionWriter opt(*this);
	while (opt.store(*addr++));
}

// store a string to eeprom
void Option::storeObj(const String& s) 
{
	OptionWriter opt(*this);
	while ( opt.store((opt.pos() < s.length()) ? s.charAt(opt.pos()) : 0) );
}

//store programme space sting 
void Option::storeObj(StringRom s) 
{
	OptionWriter opt(*this);
	while (opt.store(
		(opt.pos() < strlen_P((const char*)s))?
		(pgm_read_byte((const char *)s+opt.pos()))
			: 0
			));
}

//return stored option as a string
 OptionString::operator String()
{
	String s = "";
	int c;
	OptionWriter opt(*this);
	while ( (c=opt.read()) > -1 ) { if (c) s += (char)c; }

	if (c==-1) return s;
	corrupted();
	return "";
}

bool Option::checkCrc() const
{
	OptionWriter opt(*this);
	int result;
	while ( (result=opt.read()) > -1);
	return result == -1;
}

size_t OptionString::printTo(Print&p) const
{
	size_t n = 0;
	int c; 
	OptionWriter opt(*this);
	while ( ( c = opt.read() ) > 0) n += p.print((char)c);
	return n;
}

bool Option::read(char* obj) const
{

	OptionWriter opt(*this);
	int c; 
	while ( (c=opt.read()) > -1)
	{
		*(obj++) = (char)c;
	}
	return (c==-1);
}

