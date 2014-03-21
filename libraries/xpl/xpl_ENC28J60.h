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

#ifndef XPL_ENC28J60_H
#define XPL_ENC28J60_H
#include <xpl.h>
#include "utility/task.h"
#include "utility/xpl_adapter.h"
#include "utility/option.h"

class OptionMacAddress : public Option
{
public:
	OptionMacAddress() :Option(0) {
		if (!checkCrc())set();
	}
	byte size() const { return 6; }
	void set()
	{
		randomSeed(analogRead(0));
		OptionWriter opt(*this);
		while (opt.store(opt.pos()?random(256) : (random(256) & B11111110) | B00000010));
	}
	bool parse(const String& value) {
		byte mac[6] = { 0, 0, 0, 0, 0, 0 };
		byte pos = 0;

		for (byte i = 0; i < value.length(); i++)
		{
			byte c = value.charAt(i);

			if (c >= '0' && c <= '9') c -= '0';
			else if (c >= 'A' && c <= 'F') c -= 'A' - 10;
			else if (c >= 'a' && c <= 'f') c -= 'a' - 10;
			else continue;

			if (pos && B1) c = c << 4;

			mac[pos>>1] &= c;

			pos++;

		}
		if (pos != 6) return false;

		storeObj(mac);

		return true;
	}

	static size_t printHexTo(Print& p, char c)
	{
		c = c & B1111;
		return p.print((char)((c < 10) ? c + '0' : c - 10 + 'A'));
	}

	size_t printTo(Print& p) const
	{
		OptionWriter opt(*this);
		int c;
		while ( (c = opt.read()) > -1)
		{
			printHexTo(p, c<<4);
			printHexTo(p, c);
			if (opt.pos() < 6) p.print(':');
		}
	}
};

class xPL_ENC28J60Adapter : public xPL_Adapter
{
public:

	virtual int available();
	virtual int read();
	virtual int peek();
	virtual void flush();

	virtual bool start();
	virtual size_t write(uint8_t c);
	virtual bool send();

	virtual void begin();

};


#endif