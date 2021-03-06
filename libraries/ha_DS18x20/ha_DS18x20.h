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

	  Modified 2014-3-23 by Mathieu GRENET 
	  mailto:mathieu@mgth.fr
	  http://www.mgth.fr
*/
#
/*
Largely derived from OneWire lib from Miles Burton Arduino Temperature Control Library
http://www.milesburton.com/?title=Dallas_Temperature_Control_Library
https://github.com/milesburton/Arduino-Temperature-Control-Library
*/


#ifndef HA_DS18x20_H
#define HA_DS18x20_H

#include "utility/ha_1wire.h"
#include "sensor.h"
#include "task.h"


class HA_DS18x20_Wire 
{
private:

protected:

public:
	bool parasite;
	OneWire& wire ;

	HA_DS18x20_Wire(OneWire w) :wire(w) {};

	void discover(uint8_t resolution = 12);

	temperature_t read(byte* addr = NULL, byte configuration = 12);

	/// ask for conversion to all connected devices
	/// <remarq>faster, even with only one device connected cause it does not require to send address</remarq>
	time_t startConversion();
};

class HA_DS18x20 : public Task, public AutoList<HA_DS18x20>
{
	friend HA_DS18x20_Wire;

private:

	bool readPowerSupply();
	void request(uint8_t v);

	time_t _interval;

protected:
	HA_DS18x20_Wire& _wire;
	byte _addr[8];
	bool _pendingRequest;
	uint8_t _configuration;

public:
	FilterPin<temperature_t> temperature;


	/// <rerturn>pointer to sensor of specified address or null when not found</return>
	static HA_DS18x20* getByAddr(uint8_t addr[8]);


	/// set device resolution
	/// <param name='resolution'>significant bit count (9 to 12)<param>
	/// <remarq>9 for faster sampling, 12 for higher resolution</remarq>
	/// <remarq> 9 bits :  93.75ms</remarq>
	/// <remarq>10 bits : 187.50ms</remarq>
	/// <remarq>11 bits : 375.00ms</remarq>
	/// <remarq>12 bits : 750.00ms</remarq>
	/// <remarq>result always return an int of �C x 100</remarq>
	void setResolution(uint8_t resolution = 12);

	/// <return>current resolution</return>
	uint8_t getResolution(uint8_t resolution) {return 9 + (_configuration >> 5);}

	/// starts converstion for this specific device
	bool startConversion();

	///<return>actual conversion duration</return>
	time_t conversionDuration();

	/// read last sampled value
	temperature_t read() { return _wire.read(_addr,_configuration); }

	bool pendingRequest() { return _pendingRequest; }

	time_t interval() { return _interval; }



	HA_DS18x20(HA_DS18x20_Wire &wire, uint8_t addr[8], uint8_t resolution = 12);

	void run();

};



#endif

