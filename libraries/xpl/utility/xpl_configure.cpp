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
#include <xpl.h>
#ifdef XPL_CONFIGURE
#include "task.h"
#include "option.h"
#include "xpl_adapter.h"
#include "xpl_message.h"
#include "xpl_parser.h"
#include "xpl_configure.h"

class xPL_ConfigList : public Task, public Printable
{
public:
	xPL_ConfigList() :Task(0){}

	size_t printTo(Print& p) const {
		size_t n = 0;
		foreach(xPL_Option, opt)
		{
			n += printlnTo(p, opt->type(), '=');
			n += printlnTo(p, opt->name(), (opt->nb() <= 1) ? '\n' : '['); //10
			if (opt->nb() > 1) //4
			{
				n += printlnTo(p, opt->nb(), ']'); //64
				n += p.print('\n'); // 8
			}
		}
		return n;
	}

	void run()
	{
		if (xPL_Adapter::send(Message(xPL_MessageHeader(cs_stat, F("config"), F("list")), "*", *this)))
			delete(this);
	}
};

class xPL_ConfigCurrent : public Task, public Printable
{
public:
	xPL_ConfigCurrent() :Task(0){}
	size_t printTo(Print& p) const {
		size_t n = 0;
		foreach(xPL_Option, opt)
		{
			n += printKeyTo(p, opt->name(), *((Option*)opt));
		}
		return n;
	}

	void run()
	{
		if (xPL_Adapter::send(Message(xPL_MessageHeader(cs_stat, F("config"), F("current")), "*", *this)))
			delete(this);
	}

};

class xPL_ConfigListParser : public xPL_MessageParser
{
public:
	xPL_ConfigListParser() : xPL_MessageParser(xPL_MessageHeader(cs_cmnd, F("config"), F("list"))){}
	bool parse(const xPL_KeyValue& key)
	{
		if (key.is(F("command"), F("request")))
		{
			new xPL_ConfigList();
			return true;
		}
		return false;
	}
} _configList;

class xPL_ConfigCurrentParser : public xPL_MessageParser
{
public:
	xPL_ConfigCurrentParser() :xPL_MessageParser(xPL_MessageHeader(cs_cmnd, F("config"), F("current"))) {}
	bool parse(const xPL_KeyValue& kv)
	{
		if (kv.is(F("command"), F("request")))
		{
			new xPL_ConfigCurrent();
			return true;
		}
		return false;
	}
} _configCurrent;

class xPL_ConfigResponseParser : public xPL_MessageParser
{
public:
	xPL_ConfigResponseParser() :xPL_MessageParser(xPL_MessageHeader(cs_cmnd, F("config"), F("response"))) {}
	bool parse(const xPL_KeyValue& kv)
	{
		bool result = false;
		foreach(xPL_Option, opt)
		{
			if (kv.key == opt->name())
			{
				((Option*)opt)->parse(kv.value);
				result = true;
			}
		}
		return result;
	}
} _configResponse;

#endif