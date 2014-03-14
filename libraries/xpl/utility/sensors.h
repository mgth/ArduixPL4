/*
  ArduixPL - xPL library for Arduino(tm)
  Copyright (c) 2012/2013 Mathieu GRENET.  All right reserved.

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

	  Modified 2013-2-19 by Mathieu GRENET 
	  mailto:mathieu@mgth.fr
	  http://www.mgth.fr
*/
#ifndef SENSORS_H
#define SENSORS_H
#include "devices.h"

class Filter:public LinkedList<Filter>
{
public:
	virtual void input(double value) { output(value); }
	virtual void input(long value) { output(value); }
	virtual void input(int value) { output(value); }
	void addFilter(Filter* f) { f->link(last()); }

protected:
	template<typename TOut> 
	void output(TOut value) const {
		if (_next) _next->input(value);
	}
};

#include "float.h"
template<typename T,int _div=1>
class KalmanFilter : public Filter
{
	T _ratio;
	T _value;

	T initValue(double r) const { return DBL_MAX; }
	T initValue(int r) const { return 2^15; }

public:
	KalmanFilter(T ratio)
		:_ratio(ratio) {
		_value = initValue(ratio);
	}

	void input(T in)
	{
		if (_value == initValue(_ratio))
			_value = in;
		
		_value += (in - _value) * _ratio / (T)_div;

		output(_value);
	}
};

/*****************************************************************************
xPL Sensor
*****************************************************************************/

template<typename T,int _div>
class Calibration2ndOrder : public Filter {
	T _ax2;
	T _bx;
	T _c;

public:
	Calibration2ndOrder(T ax2, T bx, T c):_ax2(ax2),_bx(bx),_c(c) {}

	T& ax2() { return _ax2; }
	T& bx() { return _bx; }
	T& c() { return _c; }
	T& ax2(T v) { return _ax2 = v; }
	T& bx(T v) { return _bx = v; }
	T& c(T v) { return _c = v; }

	//void input(double value)
	//{
	//	value *= _bx + value * _ax2;
	//	value += _c;
	//	output(value);
	//}

	void input(T value)
	{
		long v = (long)value * (_bx + ((long)value * _ax2)/_div);
		v /= _div;
		v += _c;
		output((int)v);
	}
};
template<typename Tin, typename Tout>
class LutElement
	: public LinkedList<LutElement<Tin,Tout> >
{
	Tin _in;
	Tout _out;


	int compare(Tin i) { return sgn(_in - i); }
	int compare(const LutElement& e)
	{
		if (_in < e._in) return -1;
		if (_in > e._in) return 1;
		return 0;
	}

	Tout convert(Tin in)
	{
		Tout out;
		if (!LutElement::next()) return _out;
		if (in >= LutElement::next()->_in) return LutElement::next()->convert(in);
		Tin deltaIn = LutElement::next()->_in - _in;
		Tout deltaOut = LutElement::next()->_out - _out;
		return _out + ((in - _in)*deltaOut) / deltaIn;
	}
};

template<typename Tin, typename Tout>
class CalibrationLUT : public Filter {
	LutElement<Tin,Tout>* _lut;

	void input(Tin in) { output(_lut->convert(in)); }

};

template<typename T>
class ThresholdFilter : public Filter {
	T _publicValue;
	time_t _publicTime;

	T _threshold;
	long _timeThreshold;

	bool _reset;

	void input(T value)
	{
		T threshold = _threshold;
		if (!_reset && _timeThreshold > 0)
		{
			time_t duration = (millis() - _publicTime) / _timeThreshold;
			threshold /= 1 << duration;
		}

		if (_reset || abs(value - _publicValue) >= threshold)
		{
			_publicTime = millis();
			output(_publicValue = value);
		}
		_reset = false;
	}
public:
	void setThreshold(T t, long tt = 0) { _threshold = t; _timeThreshold = tt; }
	ThresholdFilter(T t, long tt = 0):_reset(true) { setThreshold(t, tt); }
};

/********************************************************************
Sensor Generic (float)
********************************************************************/
#include <float.h>


#include "utility/adapter.h"
template<typename T>
class OutputFilterSerial :public Filter
{
	int _nb;
public:
	OutputFilterSerial() :_nb(0) {}
	void input(T value)
	{
		Serial.print(_nb);
		Serial.print(':');
		Serial.println(value,5);
		_nb++;
	}
};

template<typename T,int _div>
class xPL_Sensor : public Filter, public xPL_Device
{
	T _lastValue;
public:
	xPL_Sensor(String name, StringRom typ, StringRom units) 
		:xPL_Device(name,typ,units) {}

	void input(T value)
	{
		_lastValue = value;
		Adapter::send(Message(MessageHeader(cs_trig, F("sensor"), F("basic")), "*", SensorContent(*this, F("current"))));
	}

	size_t printValueTo(Print& p, int value) const
	{
		int left = value / _div;
		int right = value - left * _div;
		size_t n = p.print(left);
		if (right) n +=p.print('.');
		while (right)
		{
			value = right * 10;
			n += p.print(left = value / _div);
			right = value - left * _div;
		}
		return n;
	}

	size_t printValueTo(Print& p, double v) const
	{
		return p.print(v, 2);
	}

	size_t printTo(Print& p) const {
		return printValueTo(p,_lastValue);
	}
};

class SensorDimmer : public xPL_Device {

	float _value;
	float _tmpValue;
	bool _power;

	virtual StringRom type() const { return (StringRom)F("dimmer"); }


	virtual String stringValue() const;

public:
	SensorDimmer(const String& name):xPL_Device(name,F("dimmer"),F("%")),_value(0.0),_tmpValue(0.0),_power(false) { }

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