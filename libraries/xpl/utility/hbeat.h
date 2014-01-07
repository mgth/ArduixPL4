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
#ifndef HBEAT_H
#define HBEAT_H

#include "tasks.h"
#include "listeners.h"
#include "message.h"

class HbeatRequest : public MessageParser
{
public:
	static StringRom schClass() { return S(hbeat); }
	static StringRom schType() { return S(request); }
	bool parse();

};

class HbeatContent : public Printable
{
	virtual size_t printTo(Print& p) const;
};

class HbeatTask : public Task
{
	uint32_t _interval;
	unsigned long _lastHbeatTime;
	bool _trigHbeat;
	bool _configured;

public:
	static HbeatTask Task;

	bool trig() { return _trigHbeat = true; }

	virtual bool begin();

	virtual void loop();

	uint16_t intervalMinutes() const { return _interval / 60000; }
};

#endif