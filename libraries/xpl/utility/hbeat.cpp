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

#include "hbeat.h"
#include "listeners.h"
#include "adapter.h"
#include "tasks.h"
#include "message.h"


unsigned long _lastHbeatTime;
bool _trigHbeat = true;

StringRom s_interval() { return F("interval"); }
StringRom s_newconf() { return F("newconf"); }
StringRom s_default() { return F("default"); }


OptionT<int> Hbeat::interval(24, cs_option, s_interval(), XPL_CONFIG_INTERVAL);
OptionString Hbeat::newconf(7, cs_reconf, s_newconf(), 16, s_default());

/*
Hbeat message content
*/
class HbeatContent : public Printable
{
	virtual size_t printTo(Print& p) const
	{
		printKeyTo(p, F("interval"), Hbeat::interval);
	}
};


// TASK : HbeatTask : send hbeat.basic or hbeat.config at every _interval.

class HbeatTask: public Task
{
public:
	//first hbeat occure at boot
	HbeatTask() { trigTask(); }
	virtual void run()
	{
		Adapter::send(
			Message(
				MessageHeader(
						cs_stat,
						Option::configured?F("hbeat"):F("config"),
						F("basic")
					)
				, "*"
				, HbeatContent()));

		trigTask(Hbeat::interval*60000);
	}
} _hbeatTask;

class NullPrintable : public Printable
{
	size_t printTo(Print& p) const { return 0; }
};

// TASK : NewConf

class NewConf : public Task
{
	String _newname;
public:
	NewConf(const String& newname) :_newname(newname) {};

	void run() {
		MessageHeader h(
			cs_stat,
			Option::configured ? F("hbeat") : F("config"),
			F("end")
		);

		if ( Adapter::send(Message(h, "*", NullPrintable())) )
		{
			Hbeat::newconf = _newname;
			Option::configured = true;
			_hbeatTask.trigTask();
			delete(this);
		}
	}
};

void NewconfOption::parse(const String& value) {
	(new NewConf(value))->trigTask();
};


// PARSER : xpl-cmnd - hbeat.request

class HbeatRequestParser : public MessageParser
{
public:
	HbeatRequestParser() :MessageParser(MessageHeader(cs_cmnd, F("hbeat"), F("request"))) {}
	bool parse(const KeyValue& kv)
	{
		if (kv.is(F("command"),F("request")))
		{
			_hbeatTask.trigTask();
			return true;
		}
		return false;
	}
} _hbeatRequest;

