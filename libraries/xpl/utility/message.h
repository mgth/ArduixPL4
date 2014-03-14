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

#include <Printable.h>
#include <xpl.h>

enum msgPart
#ifdef __GXX_EXPERIMENTAL_CXX0X__
	: byte
#endif
{ MSGTYPE, SCHCLASS, SCHTYPE };

template<typename typ>
size_t printlnTo(Print& p,const typ& var, char ln = '\n')
{
	int n = p.print(var) + p.print(ln);
#ifdef DEBUG
	delay(1*n); //TODO : on nano, writing to fast crashes, not tested on other boards.
#endif
	return n;
}

template<typename Tkey, typename Tvalue>
size_t printKeyTo(Print& p, const Tkey& key, const Tvalue& value, char separator = '=')
{
	return
		printlnTo(p, key,separator) +
		printlnTo(p, value);
}

class MessageHeader
{
	ConstString_t _msgType;
	StringRom _schClass;
	StringRom _schType;
public:
	MessageHeader(ConstString_t msgType, StringRom schClass, StringRom schType)
		:_msgType(msgType), _schClass(schClass), _schType(schType) {}

	StringRom msgType() const { return ConstString(_msgType); }
	StringRom schClass() const { return _schClass; }
	StringRom schType() const { return _schType; }

	StringRom part(msgPart p) const
	{
		switch (p)
		{
		case MSGTYPE: return msgType();
		case SCHCLASS: return schClass();
		case SCHTYPE: return schType();
		}
	}
};

class Message : public Printable
{
protected:
	const MessageHeader _header;
	const String _target;
	const Printable& _content;

public:

	Message(
		const MessageHeader& header,
		const String& target,
		const Printable& content
		) :_header(header),_target(target), _content(content) {}

	virtual size_t printTo(Print& p) const;
};

#endif