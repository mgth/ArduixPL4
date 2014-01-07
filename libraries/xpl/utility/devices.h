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
#ifndef DEVICES_H
#define DEVICES_H
#include "defines.h"
#include <Print.h>

#include "listeners.h"
#include "nodes.h"

class Device : public Node<Device> {
protected:
	String _name;

public:

	String name() const { return _name; }
	void setName(const String& name) { _name = name; }
	virtual StringRom type() const { return S(basic); }

	bool Is(const String& sName, const String& sType) {
		if (sName != _name) return false;
		if (sType != type()) return false;
		return true;
	}

	virtual void setCurrent(const String& current, const String& data1) = 0;
	virtual String current() const = 0;

	virtual size_t PrintCurrent(Print& p) { return 0; }
};

class DeviceParser : public MessageParser
{
	String _keyDevice;
	String _keyType;
public:

	bool parse()
	{
		return parseKey(S(device), _keyDevice)
			|| parseKey(S(type), _keyType);
	}
public:
	Device* device();
};


class SensorContent : public Printable
{
	const String& _request;
	const Device& _device;

public:
	SensorContent(const Device &device,const String& request) :_request(request), _device(device)	{}

	size_t printTo(Print& p) const;
};

class SensorRequest : public DeviceParser
{
public:
	static StringRom schClass() { return S(sensor); }
	static StringRom schType() { return S(request); }
	String _request;

	bool parse()
	{
		return parseKey(S(request), _request)
		|| DeviceParser::parse();
	}

	void process();
};

class ControlBasic : public DeviceParser
{
public:
	static StringRom schClass() { return S(control); }
	static StringRom schType() { return S(basic); }
	String _current;
	String _data1;

	bool parse()
	{
		return parseKey(S(current), _current)
			|| DeviceParser::parse();
	}
	void process() { if (device()) device()->setCurrent(_current, _data1); }
};

class DevicesClass : public Parent<Device>
{
	//Hooker<SensorRequest> _sensorRequest;

public:
	Device* get(const String& name, const String& type);


	void begin() {
		Listeners.hook<SensorRequest>();
		Listeners.hook<ControlBasic>();
	}

	bool reg(Device& dev) { this->add(&dev); }

	bool ParseKey(const String& key, const String& value);

};
extern DevicesClass Devices;



#endif