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

	  Modified 2014-3-20 by Mathieu GRENET 
	  mailto:mathieu@mgth.fr
	  http://www.mgth.fr
*/
#include "utility/option.h"

class xPL_Option : public AutoList<xPL_Option>
{
	byte _nb;
	byte _type;
	StringRom _name;

public:
	byte nb() const { return _nb; }
	byte type() const { return _type; }
	StringRom name() const { return _name; }
	xPL_Option(ConstString_t type, StringRom name)
		:_type(_type), _name(name){}


};

template<typename T>
class xPL_OptionT : public OptionT<T>, public xPL_Option
{
public:
	xPL_OptionT(int addr, T defaultValue, ConstString_t type, StringRom name)
	:OptionT<T>(addr, defaultValue), xPL_Option(type, name){}

};

template<>
class xPL_OptionT<String> : public OptionT<String>, public xPL_Option
{
public:
	xPL_OptionT(int addr, byte size, StringRom defaultValue, ConstString_t type, StringRom name)
		:OptionT<String>(addr, size, defaultValue), xPL_Option(type, name){}

	void operator=(const String& s) {
		OptionT<String>::operator=(s);
	
	}
};