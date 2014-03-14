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
#ifndef ADAPTERW5100_H
#define ADAPTERW5100_H
#include "utility/defines.h"
#include "Stream.h"
#include "Ethernet.h"
#include "EthernetUDP.h"
#include "utility/tasks.h"
#include "utility/adapter.h"

class W5100Adapter : public xPL_Adapter
{
private:
	EthernetUDP _udp;
	uint8_t _mac[6];


//	String readStringUntil(char term) { return _udp.readStringUntil(term); }

	virtual int read() { return _udp.read(); }
	virtual int available() { return _udp.available(); }
	virtual int peek() { return _udp.peek(); }
	virtual void flush() { return _udp.flush(); }

public:
	virtual bool start();
	virtual size_t write(uint8_t c);
	virtual bool send();

	virtual bool begin();

};


#endif