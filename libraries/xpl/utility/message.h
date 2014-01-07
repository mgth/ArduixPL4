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

#ifndef MESSAGE_H
#define MESSAGE_H

#include <Arduino.h>
#include "defines.h"

class Message : public Printable
{
	StringRom _msgType;
	StringRom _schClass;
	StringRom _schType;

	String _target;
	const Printable& _content;

public:
	Message(
		StringRom msgType,
		StringRom schClass,
		StringRom schType,
		const String& target,
		const Printable& content
		) :_msgType(msgType), _schClass(schClass), _schType(schType), _target(target), _content(content) {}

	template<typename Tkey,typename Tvalue>
	static size_t printKeyTo(Print& p,const Tkey& key,const Tvalue& value)
	{
		return
			p.print(key) +
			p.print('=') +
			p.print(value) + p.print('\n');
	}

	virtual size_t printTo(Print& p) const;

};

#endif