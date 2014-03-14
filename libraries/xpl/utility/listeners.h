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
#ifndef LISTENER_H
#define LISTENER_H
#include <xpl.h>

#include "linkedlist.h"
#include "message.h"

typedef byte nbparsers_t;

class KeyValue
{
public:
	String key;
	String value;
	KeyValue():key(""), value("") {}
	KeyValue(String k, String v) :key(k), value(v) {}

	bool is(StringRom _key,StringRom _value) const { return (key == _key && value == _value); }

	bool parse(StringRom _key, String& store) const
	{
		if (key == _key) { store = value; return true; }
		return false;
	}

};

class MessageParser : public AutoList<MessageParser>
{
	bool _selected;
private:
	MessageHeader _header;

protected:

public:
	MessageHeader& header() { return _header; }
	MessageParser(MessageHeader header) :_header(header) {};

	virtual bool parse(const KeyValue& key) = 0;
	static nbparsers_t parseSelected(const KeyValue& kv);

	virtual bool process() { return true; }
	static nbparsers_t processSelected();

	bool selected() const { return _selected; }
	bool select(bool sel);
	static nbparsers_t selectAll();

	nbparsers_t select(msgPart part, const String& s);
	static nbparsers_t selectAll(msgPart part, const String& s);
};





#endif