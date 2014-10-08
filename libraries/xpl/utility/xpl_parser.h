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

#ifndef XPL_PARSER_H
#define XPL_PARSER_H
#include <xpl.h>

#include "linkedlist.h"
#include "xpl_message.h"

typedef byte nbparsers_t;

class xPL_KeyValue
{
public:
	String key;
	String value;
	xPL_KeyValue():key(""), value("") {}
	xPL_KeyValue(String k, String v) :key(k), value(v) {}

	bool is(StringRom _key,StringRom _value) const { return (key == _key && value == _value); }

	bool parse(StringRom _key, String& store) const
	{
		if (key == _key) { store = value; return true; }
		return false;
	}

	int count(char separator=',') const {
		int pos = 0;
		int c = 0;
		while (pos > -1)
		{
			pos = value.indexOf(separator, pos);
			if (pos > -1) pos++;
			c++;
		}
		return c;
	}

	String val(byte n, char separator=',') const
	{
		int pos = 0;
		int c = 0;
		while (pos > -1 && c<n)
		{
			pos = value.indexOf(separator, pos);
			if (pos > -1) pos++;
			c++;
		}

		int end = value.indexOf(separator, pos);

		if (end < 0) return value.substring(pos);
		else return value.substring(pos, end - 1);
	}
};

class xPL_MessageParser : public AutoList<xPL_MessageParser>
{
	bool _selected;
private:
	xPL_MessageHeader _header;

protected:

public:
	xPL_MessageHeader& header() { return _header; }
	xPL_MessageParser(xPL_MessageHeader header) :_header(header) {};

	virtual bool parse(const xPL_KeyValue& key) = 0;
	static nbparsers_t parseSelected(const xPL_KeyValue& kv);

	virtual bool process() { return true; }
	static nbparsers_t processSelected();

	bool selected() const { return _selected; }
	bool select(bool sel);
	static nbparsers_t selectAll();

	nbparsers_t select(msgPart part, const String& s);
	static nbparsers_t selectAll(msgPart part, const String& s);
};





#endif