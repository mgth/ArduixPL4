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
#include "xpl_hbeat.h"
#include "xpl_parser.h"
#include "xpl_adapter.h"
#include "task.h"
#include "xpl_message.h"


unsigned long _lastHbeatTime;
bool _trigHbeat = true;

StringRom s_interval() { return F("interval"); }
StringRom s_newconf() { return F("newconf"); }
StringRom s_default() { return F("default"); }


xPL_NewconfOption xPL_Hbeat::newconf(7);
xPL_IntervalOption xPL_Hbeat::interval((int)newconf.addrNext(), XPL_CONFIG_INTERVAL);

/*
xPL_Hbeat message content
*/
class xPL_HbeatContent : public Printable
{
	virtual size_t printTo(Print& p) const
	{
		printKeyTo(p, F("interval"), xPL_Hbeat::interval);
	}
};


// TASK : xPL_HbeatTask : send hbeat.basic or hbeat.config at every _interval.

class xPL_HbeatTask: public Task
{
public:
	//first hbeat occure at boot
	xPL_HbeatTask() :Task(0, xPL_Hbeat::interval * 60000) { }
	virtual void run()
	{
		xPL_Adapter::send(
			Message(
				xPL_MessageHeader(
						cs_stat,
						Option::configured?F("hbeat"):F("config"),
						F("basic")
					)
				, "*"
				, xPL_HbeatContent()));

	}
} _hbeatTask;

class NullPrintable : public Printable
{
	size_t printTo(Print& p) const { return 0; }
};

// TASK : xPL_NewConf

class xPL_NewConf : public Task
{
	String _newname;
public:
	xPL_NewConf(const String& newname) :_newname(newname) {};

	void run() {
		xPL_MessageHeader h(
			cs_stat,
			Option::configured ? F("hbeat") : F("config"),
			F("end")
		);

		if ( xPL_Adapter::send(Message(h, "*", NullPrintable())) )
		{
			xPL_Hbeat::newconf = _newname;
			Option::configured = true;
			_hbeatTask.trigTask();
			delete(this);
		}
	}
};

bool xPL_IntervalOption::parse(const String& value) {
	xPL_OptionT<int>::parse(value);
	_hbeatTask.trigTask(0,*this);
	return true;
};

bool xPL_NewconfOption::parse(const String& value) {
	(new xPL_NewConf(value))->trigTask();
	return true;
};

// PARSER : xpl-cmnd - hbeat.request

class xPL_HbeatRequestParser : public xPL_MessageParser
{
public:
	xPL_HbeatRequestParser() :xPL_MessageParser(xPL_MessageHeader(cs_cmnd, F("hbeat"), F("request"))) {}
	bool parse(const xPL_KeyValue& kv)
	{
		if (kv.is(F("command"),F("request")))
		{
			_hbeatTask.trigTask();
			return true;
		}
		return false;
	}
} _hbeatRequest;

