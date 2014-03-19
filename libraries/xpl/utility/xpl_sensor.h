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
#ifndef XPL_SENSOR_H
#define XPL_SENSOR_H
#include <xpl.h>
#include "utility/sensor.h"
#include "xpl_device.h"
#include "xpl_adapter.h"


class xPL_SensorBase : public xPL_Device
{
protected:
	xPL_SensorBase(String name, StringRom typ, StringRom units)
		:xPL_Device(name, typ, units) {}

	void sendCurrent()
	{
		xPL_Adapter::send(Message(xPL_MessageHeader(cs_trig, F("sensor"), F("basic")), "*", SensorContent(*this, F("current"))));
	}

};

template<typename T, int _div=1, byte _dec=2>
class xPL_Sensor : public Filter<T> , public xPL_SensorBase
{
	T _lastValue;
public:
	xPL_Sensor(String name, StringRom typ, StringRom units)
		:xPL_SensorBase(name, typ, units) {}

	void input(const T& value)
	{
		_lastValue = value;
		sendCurrent();
	}

	size_t printValueTo(Print& p, long value) const
	{
		size_t n = 0;
		if (value < 0)
		{
			n += p.print('-');
			value = -value;
		}
		long left = value / _div;
		long right = value - left * _div;
		n = p.print(left);
		if (right) n += p.print('.');
		byte dec = _dec;
		while (right && dec)
		{
			value = right * 10;
			n += p.print(left = value / _div);
			right = value - left * _div;
			dec--;
		}
		return n;
	}
	size_t printValueTo(Print& p, int value) const
	{
		return printValueTo(p, (long)value);
	}

	size_t printValueTo(Print& p, double v) const
	{
		return p.print(v, 2);
	}

	size_t printTo(Print& p) const {
		return printValueTo(p, _lastValue);
	}
};

class SensorDimmer : public xPL_Device {

	float _value;
	float _tmpValue;
	bool _power;

	virtual StringRom type() const { return (StringRom)F("dimmer"); }


	virtual String stringValue() const;

public:
	SensorDimmer(const String& name) :xPL_Device(name, F("dimmer"), F("%")), _value(0.0), _tmpValue(0.0), _power(false) { }

	void setValue(float v);

	float setTmpValue(float v);

	float move(float v);

	void validate() { setValue(_tmpValue); }
	void cancel() { setTmpValue(_value); }

	bool power() { return _power; }
	bool power(bool p);
	bool toggle();
};

#endif
