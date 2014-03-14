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
#ifndef XPL_DEVICE_H
#define XPL_DEVICE_H
#include <Print.h>
#include <xpl.h>

#include "utility/linkedlist.h"
#include "xpl_parser.h"

class xPL_Device : public AutoList<xPL_Device>, public Printable {
protected:
	String _name;
	StringRom _type;
	StringRom _units;
public:
	xPL_Device(const String& name, StringRom typ, StringRom units) 
		:_name(name),_type(typ),_units(units) {}
	String name() const { return _name; }
	StringRom type() const { return _type; }
	StringRom units() const { return _units; }

	void setName(const String& name) { _name = name; }

	bool is(const String& name, const String& typ) {
		if (name != _name) return false;
		if (typ !="" && typ != _type) return false;
		return true;
	}

	virtual void setValue(const String& current, const String& data1) {};
	virtual void setValue(float value) {};

	static xPL_Device* get(const String& name, const String& type);

//	size_t printTo(Print& p) const { p.print(_lastValue); }
};

class xPL_DeviceParser : public xPL_MessageParser
{
	String _keyDevice;
	String _keyType;
public:
	xPL_DeviceParser(xPL_MessageHeader header)
		:xPL_MessageParser(header){}
	bool parse(const xPL_KeyValue& key)
	{
		return key.parse(F("device"),_keyDevice)
			|| key.parse(F("type"),_keyType);
	}

	xPL_Device* device();
};


class SensorContent : public Printable
{
	const String& _request;
	const xPL_Device& _device;
public:
	SensorContent(const xPL_Device &device,const String& request) :_request(request), _device(device)	{}

	size_t printTo(Print& p) const;
};







#endif