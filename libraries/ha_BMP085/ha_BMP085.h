/*  ArduHA - ArduixPL - xPL library for Arduino(tm)  Copyright (c) 2012/2014 Mathieu GRENET.  All right reserved.  This file is part of ArduHA / ArduixPL.    ArduixPL is free software: you can redistribute it and/or modify    it under the terms of the GNU General Public License as published by    the Free Software Foundation, either version 3 of the License, or    (at your option) any later version.    ArduixPL is distributed in the hope that it will be useful,    but WITHOUT ANY WARRANTY; without even the implied warranty of    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    GNU General Public License for more details.    You should have received a copy of the GNU General Public License    along with ArduixPL.  If not, see <http://www.gnu.org/licenses/>.	  Modified 2014-3-16 by Mathieu GRENET 	  mailto:mathieu@mgth.fr	  http://www.mgth.fr*/// xPL_Sensor_Dallas.h

#ifndef HA_BMP085_h
#define HA_BMP085_h

#include <ArduHA.h>
#include "utility/task.h"
#include "utility/sensor.h"
#include "Wire.h"
//#include "utility/BMP085.h"typedef enum{	UltraLow,	Standard,	High,	UltraHigh} BMP085_res_t;class HA_BMP085:public Task {
int _pin;byte _type;uint8_t _oversampling;int16_t _calibration[11];private:
	uint8_t read8(uint8_t addr);
	uint16_t read16(uint8_t addr);
	void write8(uint8_t addr, uint8_t data);

	uint16_t readRawTemperature();	uint32_t readRawPressure();public:FilterPin<int> temperature;FilterPin<long> pressure;	HA_BMP085(BMP085_res_t mode=UltraHigh);		int readTemperature();	long readPressure();
	void run();};
#endif

