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
#include "devices.h"

#include "adapter.h"
#include "message.h"

xPL_Device* DeviceParser::device() { return xPL_Device::get(_keyDevice, _keyType); }

xPL_Device* xPL_Device::get(const String& name, const String& type)
{
	foreach (xPL_Device, d)
	{
		if (d->is(name, type)) return &*d;
	}
	return NULL;
}

size_t SensorContent::printTo(Print& p) const {
	size_t n =
		printKeyTo(p, F("device"), _device.name()) +
		printKeyTo(p, F("type"), _device.type());

	String value;

	if (_request == F("current"))
		n += printKeyTo(p, _request, _device);
	else if (_request == F("name"))
		n += printKeyTo(p, _request, _device.name());
	else return n;

	if (_request == F("current")) n += printKeyTo(p, F("units"), _device.units());
	return n;
}

class SensorRequest : public DeviceParser
{
	String _request;
public:
	SensorRequest() :DeviceParser(MessageHeader(cs_cmnd, F("sensor"), F("request"))) {}

	bool parse(const KeyValue& kv)
	{
		return kv.parse(F("request"),_request)
			|| DeviceParser::parse(kv);
	}
	bool process()
	{
		return Adapter::send(Message(MessageHeader(cs_stat, F("sensor"), F("basic")), "*", SensorContent(*device(), _request)));
	}
} _sensorRequest;

class ControlBasic : public DeviceParser
{
public:
	ControlBasic() :DeviceParser(MessageHeader(cs_cmnd, F("control"), F("basic"))){}
	String _current;
	String _data1;

	bool parse(const KeyValue& key)
	{
		return key.parse(F("current"),_current)
			|| DeviceParser::parse(key);
	}
	bool process()
	{
		if (device())
		{
			device()->setValue(_current, _data1);
			return true;
		}
		return false;
	}
} _controlBasic;