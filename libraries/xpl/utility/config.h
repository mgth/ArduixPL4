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

#ifndef CONFIG_H
#define CONFIG_H
#include "xpl.h"
#include "listeners.h"
#include "tasks.h"

class ConfigListParser : public MessageParser
{
public:
	static StringRom msgType() { return S(cmnd); }
	static StringRom schClass() { return S(config); }
	static StringRom schType() { return S(list); }
	bool parse();
};

class ConfigCurrentParser : public MessageParser
{
public:
	static StringRom msgType() { return S(cmnd); }
	static StringRom schClass() { return S(config); }
	static StringRom schType() { return S(current); }
	bool parse();
};

class ConfigResponseParser : public MessageParser
{
public:
	static StringRom msgType() { return S(cmnd); }
	static StringRom schClass() { return S(config); }
	static StringRom schType() { return S(response); }
	bool parse();
};

class ConfigList : public Printable
{
	size_t printTo(Print& p) const;
};

class ConfigCurrent : public Printable
{
	size_t printTo(Print& p) const;
};


class Configurator : Node<Configurator>
{
	byte _nb;
	StringRom _option;
	StringRom _name;

	Configurator(StringRom option, StringRom name, byte nb = 1) :_option(option), _name(name), _nb(nb) {}

	virtual void parse(const String& key, const String& value) {};
	size_t printListTo(Print& p) const {
		size_t n = p.print(_option) + p.print('=') + p.print (_name);
		if (_nb > 1) n += p.print('[') + p.print(_nb) + p.print(']');
		return n;
	}
};

class ConfigTask : public Task, Parent<Configurator>
{

	bool _trigList;
	bool _trigCurrent;
public:
	static ConfigTask Task;

	void trigList() { _trigList = true; }
	void trigCurrent() { _trigCurrent = true; }

	ConfigTask() :_trigList(false), _trigCurrent(false) {}

	bool begin() {
		Task::begin();
		Listeners.hook<ConfigListParser>();
		Listeners.hook<ConfigCurrentParser>();
		Listeners.hook<ConfigResponseParser>();

		return true;
	}

	void loop() {
		if (_trigList)
		{
			xPL.send(Message(S(stat),S(config),S(list),"*",ConfigList()));
		}
		if (_trigCurrent)
		{
			xPL.send(Message(S(stat),S(config),S(current),"*",ConfigCurrent()));
		}

	}
};

#endif