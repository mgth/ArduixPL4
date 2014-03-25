/*  ArduHA - ArduixPL - xPL library for Arduino(tm)  Copyright (c) 2012/2014 Mathieu GRENET.  All right reserved.  This file is part of ArduHA / ArduixPL.    ArduixPL is free software: you can redistribute it and/or modify    it under the terms of the GNU General Public License as published by    the Free Software Foundation, either version 3 of the License, or    (at your option) any later version.    ArduixPL is distributed in the hope that it will be useful,    but WITHOUT ANY WARRANTY; without even the implied warranty of    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    GNU General Public License for more details.    You should have received a copy of the GNU General Public License    along with ArduixPL.  If not, see <http://www.gnu.org/licenses/>.	  Modified 2014-3-16 by Mathieu GRENET 	  mailto:mathieu@mgth.fr	  http://www.mgth.fr*/// xPL_Sensor_Dallas.h

#ifndef HA_BMP085_h
#define HA_BMP085_h

#include <ArduHA.h>
#include "task.h"
#include "sensor.h"
#include "i2c.h"
//#include "Wire.h"
//#include "utility/BMP085.h"typedef enum{	UltraLow,	Standard,	High,	UltraHigh} BMP085_res_t;typedef struct
{
	int16_t  ac1;
	int16_t  ac2;
	int16_t  ac3;
	uint16_t ac4;
	uint16_t ac5;
	uint16_t ac6;
	int16_t  b1;
	int16_t  b2;
	int16_t  mb;
	int16_t  mc;
	int16_t  md;
} bmp085_calib_data;typedef enum{	state_init,	state_none,	state_temperature,	state_pressure} state_t;class HA_BMP085:public Task, public I2C {
uint8_t _oversampling;bmp085_calib_data _calibration;state_t _state = state_init;int32_t _B5;private:

	void init();	void readRawTemperature();	uint32_t readRawPressure();	void process();public:FilterPin<int> temperature;FilterPin<long> pressure;	HA_BMP085(time_t first, time_t interval, BMP085_res_t mode=UltraHigh);	bool queryTemperature();
	bool queryPressure();
	void run();};
#endif

