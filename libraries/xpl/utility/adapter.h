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

#ifndef ADAPTER_H
#define ADAPTER_H
#include "tasks.h"
#include "Printable.h"
#include "defines.h"

class Adapter : public Task, public Stream
{
protected:
	void setMac(uint8_t* mac);

public:

	virtual bool start() = 0;
	virtual bool send() = 0;

	virtual void loop();

	size_t send(const Printable& p)
	{
		DBG_MEM(F("send"));
			
		Serial.println(p);

		if (start())
		{
			size_t n = print(p);


			if (send()) return n;
		}
		return 0;
	}

	String readStringUntil(char terminator)
	{
		String ret="";
		int c = read();
		while ( c >= 0 && c != terminator && c != '\n' )
		{
			ret += (char)c;
			c = read();
		}
		return ret;
	}

	bool cmpUntil(char term, StringRom cmp) { return readStringUntil(term) == cmp; }
	bool cmpUntil(char term, String cmp) { return readStringUntil(term) == cmp; }
	bool cmpUntil(char term, char cmp) { return readStringUntil(term)[0] == cmp; }

};

#endif