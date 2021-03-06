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
#ifndef HA_BH1750_H
#define HA_BH1750_H

#include <ArduHA.h>
#include "task.h"
#include "i2c.h"
#include "sensor.h"

#define BH_POWER_OFF   0x00
#define BH_POWER_ON    0x01
#define BH_RESET       0x07
#define BH_CONTINUOUS  0x10
#define BH_ONE_TIME    0x20
#define BH_HRES_1      0x00
#define BH_HRES_2      0x01
#define BH_LRES        0x03
#define BH_MT_H        0x40
#define BH_MT_L		   0x60

const uint8_t BH_DEFAULT_MT=69;
const uint16_t BH_H_TIME=180;
const uint16_t BH_L_TIME=24;

/// <summary>BH1750 Ambient Light Sensor</summary>
/// Digital 16bit Serial Output Type 
/// Ambient Light Sensor IC

class HA_BH1750 :public Task, public I2C {
	uint8_t _mode;
	uint8_t _mt;
//	unsigned long _time;
	bool _waiting;
	time_t _waitTime();

public:

	HA_BH1750(
		int addr = 0x23,
		byte mode = 1,
		bool continuous = false,
		byte mt = 69
		);

	void run();

	void setMode(byte mode=1,bool continuous=false,byte mt=69);

	FilterPin<int> out;

};


#endif

