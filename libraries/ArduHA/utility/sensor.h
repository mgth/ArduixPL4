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
#ifndef SENSOR_H
#define SENSOR_H
#include <arduha.h>
#include "limits.h"
#include "linkedlist.h"

template <typename T>
class Filter :public LinkedList< Filter<T> >
{
public:
	virtual void input(const T& value) = 0;

};

template <typename T>
class FilterPin {
	Filter<T>* _filter = NULL;
public:
	// Add a filter to listen to this pin
	template <typename obj>
	obj* link(obj* filter) { filter->Filter<T>::link(_filter); return filter; }

	// write information to all listening filters on this pin
	bool write(const T& value)
	{
		foreachfrom(Filter<T>, f, _filter)
		{
			f->input(value);
		}
	}
};

#include "float.h"
template<typename T,int _div=1>
class KalmanFilter : public Filter<T>
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

	FilterPin<T> out;

	void input(T in)
	{
		if (_value == initValue(_ratio))
			_value = in;
		
		_value += (in - _value) * _ratio / (T)_div;

		out.write(_value);
	}
};

#define roundedShift(v,n) ((v >> (n - 1)) & (v >> n))


class Calibration2ndOrder_Shifted : public Filter<int>
{
	int _inv_a;
	int _inv_b;
	int _nc;
	byte _shift;
public:

	FilterPin<int> out;

	// Will return ( ax² + (1+b)x + c ) << shift
	// With :
	// inv_a = 1/a
	// inv_b = 1/b
	// nc = c << shift
	Calibration2ndOrder_Shifted(int inv_a, int inv_b, int nc, byte shift=7) :_inv_a(inv_a), _inv_b(inv_b), _nc(nc), _shift(shift){}

	void input(const int& nx);
};

class Calibration2ndOrder : public Filter<double> {
	double _ax2;
	double _bx;
	double _c;

public:
	Calibration2ndOrder(double ax2, double bx, double c) :_ax2(ax2), _bx(bx), _c(c) {}

	FilterPin<double> out;

	void input(double value)
	{
		value *= _bx + value * _ax2;
		value += _c;
		out.write(value);
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
class CalibrationLUT : public Filter<Tin> {
	LutElement<Tin,Tout>* _lut;

	void input(Tin in) { output(_lut->convert(in)); }

};

template<typename T>
class ThresholdFilter : public Filter<T> {
public:
	void setThreshold(T t, long tt = 0) { _threshold = t; _timeThreshold = tt; }
	ThresholdFilter(T t, long tt = 0) :_reset(true) { setThreshold(t, tt); }

	FilterPin<T> out;

private:
	T _publicValue;
	time_t _publicTime;

	T _threshold;
	long _timeThreshold;

	bool _reset;

	void input(const T& value)
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
			out.write(_publicValue = value);
		}
		_reset = false;
	}
};

template<typename T>
class OutputFilterSerial :public Filter<T>
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

#endif

