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
#
// Derived from DHT Temperature & Humidity Sensor library for Arduino by Rob Tillaart
// http://arduino.cc/playground/Main/DHTLib

#ifndef XPL_DHT_h
#define XPL_DHT_h

#include <ArduHA.h>
#include "task.h"
#include "sensor.h"


typedef enum { DHT_OK, DHT_BADCHECKSUM, DHT_TIMEOUT, DHT_INVALID_VALUE, DHT_TOO_EARLY } dhtStatus_t;

typedef enum{ DHT11, DHT21, DHT22, DHT33, DHT44 } dhtType_t;
enum {RHT01,RHT02,RHT03,RHT04,RHT05};

class HA_DHTxx: public Task {
	// dht data pin
protected:
	uint8_t _pin;
	time_t _lastSampling;

	public:
	FilterPin<float> temperature;
	FilterPin<float> humidity;

	dhtStatus_t read(uint8_t* bits, bool dht11);
	dhtStatus_t read(float& t, float& h);
	dhtStatus_t read11(float& t, float& h);
	int readWhile(bool state);


	///<param name="pin">arduino pin connected to device data pin</param>
	// TODO : we might try to autodetect device type :
	// - reset seams to need to be at least 18ms to reset a DHT11 while other devices only need 1ms
	// - on DHT11 first two bytes should always be 0 with no negative value
	// - if trying to read device twice two fast what error do we get ?
	HA_DHTxx(uint8_t pin);
};

template<dhtType_t model>
class HA_DHT : public HA_DHTxx
{
public:
	///<param name="pin">arduino pin connected to device data pin</param>
	HA_DHT<model>(uint8_t pin) : HA_DHTxx(pin){};

protected:

	long samplingDuration() { return ((model >= DHT21) ? 2000000 : 1000000) - (micros() - _lastSampling); }

	void run()
	{
		// if sampling duration still running, wait for ending
		long delay = samplingDuration();
		if (delay > 0)
		{
			trigTaskMicros(delay);
			return;
		}

		float t;
		float h;

		dhtStatus_t status;
		if (model>=DHT21)
			status = read(t, h);
		else
			status = read11(t, h);

		if ( status == DHT_OK)
		{
			temperature.write(t);
			humidity.write(h);
			return;
		}
	}
};

template<dhtType_t model>
class HA_DHT_Power : public HA_DHT<model>
{
	uint8_t _powerPin;

	///<param name="pin">arduino pin connected to device data pin</param>
	///<param name="powerPin">pin used to power device (in order to reduce power consumption)</param>
	// TODO : check if power pin is a good idea
	HA_DHT_Power(uint8_t pin, uint8_t powerPin) :_powerPin(powerPin), HA_DHT<model>(pin){}

	void run()
	{
		double t;
		double h;
		// if not powered, switch it on and wait for sampling
		if (!digitalRead(_powerPin))
		{
			pinMode(_powerPin, OUTPUT);
			digitalWrite(_powerPin, HIGH);
			this->_lastSampling = this->now();
			trigTask(this->samplingDuration());
			return;
		}

		HA_DHT<model>::run();

		if (!this->linked())
		{
			// switch off till next sampling
			pinMode(_powerPin, OUTPUT);
			digitalWrite(_powerPin, LOW);
		}
	}

};

#endif

