/*  ArduHA - ArduixPL - xPL library for Arduino(tm)  Copyright (c) 2012/2014 Mathieu GRENET.  All right reserved.  This file is part of ArduHA / ArduixPL.    ArduixPL is free software: you can redistribute it and/or modify    it under the terms of the GNU General Public License as published by    the Free Software Foundation, either version 3 of the License, or    (at your option) any later version.    ArduixPL is distributed in the hope that it will be useful,    but WITHOUT ANY WARRANTY; without even the implied warranty of    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    GNU General Public License for more details.    You should have received a copy of the GNU General Public License    along with ArduixPL.  If not, see <http://www.gnu.org/licenses/>.	  Modified 2014-3-14 by Mathieu GRENET 	  mailto:mathieu@mgth.fr	  http://www.mgth.fr*/#
// Derived from DHT Temperature & Humidity Sensor library for Arduino by Rob Tillaart
// http://arduino.cc/playground/Main/DHTLib

#ifndef XPL_DHT_h
#define XPL_DHT_h

#include <ArduHA.h>
#include "utility/task.h"
#include "utility/sensor.h"

#define DHTLIB_OK				0
#define DHTLIB_ERROR_CHECKSUM	-1
#define DHTLIB_ERROR_TIMEOUT	-2
#define DHTLIB_INVALID_VALUE	-999
class HA_SensorDHT:public Task {
	int _pin;	byte _type;	public:	FilterPin<int> temperature;	FilterPin<int> humidity;	int read(uint8_t bits[5]);
	int read(int& t, int& h);
	int readWhile(bool state);	HA_SensorDHT(uint8_t pin, byte type);	void run();};
#endif

