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

#ifndef ADAPTER_H
#define ADAPTER_H
#include <Printable.h>

#include <ArduHA.h>
#include <xpl.h>
#include "task.h"

/// <summary>Base class for sending xPL messages</summary>
class xPL_Adapter : public Task, public Stream

#ifdef XPL_ADAPLTER_MULTI
	, public AutoList<xPL_Adapter>
#endif

{
#ifndef XPL_ADAPLTER_MULTI
	static xPL_Adapter* _adapter;
#endif

	void parse();

	// prepare a new message
	virtual bool start() = 0;
	// send message
	virtual bool send() = 0;

public:
	xPL_Adapter(time_t interval=100);

	//send message
	size_t sendMessage(const Printable& p);

	///<summary>send message with all installed adapters</summary>
	static size_t send(const Printable& p);

	///<summary>for task support</summary>
	void run();

	/// <summary>Read string until terminator, or line end</summary>
	String readStringUntil(char terminator);

	void flushUntil(char terminator);
	int readChar(char c);
	char cmpChar(char c, char terminator);

	bool cmpUntil(char term, StringRom cmp);
	bool cmpUntil(char term, const String& cmp);
	bool cmpUntil(char term, char cmp);

};

#endif