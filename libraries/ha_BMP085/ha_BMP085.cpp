/*  ArduHA - ArduixPL - xPL library for Arduino(tm)  Copyright (c) 2012/2014 Mathieu GRENET.  All right reserved.  This file is part of ArduHA / ArduixPL.    ArduixPL is free software: you can redistribute it and/or modify    it under the terms of the GNU General Public License as published by    the Free Software Foundation, either version 3 of the License, or    (at your option) any later version.    ArduixPL is distributed in the hope that it will be useful,    but WITHOUT ANY WARRANTY; without even the implied warranty of    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    GNU General Public License for more details.    You should have received a copy of the GNU General Public License    along with ArduixPL.  If not, see <http://www.gnu.org/licenses/>.	  Modified 2014-3-16 by Mathieu GRENET 	  mailto:mathieu@mgth.fr	  http://www.mgth.fr*/#include "ha_BMP085.h"
#include "debug.h"


#define BMP085_I2CADDR           (0x77)
#define BMP085_CHIPID            (0x55)

typedef enum
{
	READTEMP = 0x2E,
	READPRESSURE = 0x34
} bmp085_cmd;

typedef enum
{
	CAL_AC1 = 0xAA,  // R   Calibration data (16 bits)
	CAL_AC2 = 0xAC,  // R   Calibration data (16 bits)
	CAL_AC3 = 0xAE,  // R   Calibration data (16 bits)
	CAL_AC4 = 0xB0,  // R   Calibration data (16 bits)
	CAL_AC5 = 0xB2,  // R   Calibration data (16 bits)
	CAL_AC6 = 0xB4,  // R   Calibration data (16 bits)
	CAL_B1 = 0xB6,  // R   Calibration data (16 bits)
	CAL_B2 = 0xB8,  // R   Calibration data (16 bits)
	CAL_MB = 0xBA,  // R   Calibration data (16 bits)
	CAL_MC = 0xBC,  // R   Calibration data (16 bits)
	CAL_MD = 0xBE,  // R   Calibration data (16 bits)
	CHIPID = 0xD0,
	VERSION = 0xD1,
	SOFTRESET = 0xE0,
	CONTROL = 0xF4,
	TEMPDATA = 0xF6,
	PRESSUREDATA = 0xF6,
} bmp085_register;


#include <util/delay.h>
HA_BMP085::HA_BMP085(time_t first, time_t interval, BMP085_res_t mode) : _oversampling(mode),Task(first, interval), I2C(BMP085_I2CADDR){}void HA_BMP085::init(){	I2C::init();
	if (read<byte>(CHIPID) != BMP085_CHIPID) return;

	/* read calibration data */
#if 1
	int16_t* cal = (int16_t*)&_calibration;
	for (byte c = 0; c < 11; c++)
	{
//		cal[c] = (int16_t)read<byte>(CAL_AC1 + (2 * c)) << 8;
//		cal[c] |= (int16_t)read<byte>(CAL_AC1 + 1 +(2 * c));
		cal[c] = readBE<int16_t>(CAL_AC1 + (2 * c));
	}
#else
	_calibration.ac1 = readBE<int16_t>(CAL_AC1);
	_calibration.ac2 = readBE<int16_t>(CAL_AC2);
	_calibration.ac3 = readBE<int16_t>(CAL_AC3);
	_calibration.ac4 = readBE<uint16_t>(CAL_AC4);
	_calibration.ac5 = readBE<uint16_t>(CAL_AC5);
	_calibration.ac6 = readBE<uint16_t>(CAL_AC6);
	_calibration.b1 = readBE<int16_t>(CAL_B1);
	_calibration.b2 = readBE<int16_t>(CAL_B2);
	_calibration.mb = readBE<int16_t>(CAL_MB);
	_calibration.mc = readBE<int16_t>(CAL_MC);
	_calibration.md = readBE<int16_t>(CAL_MD);
#endif}bool HA_BMP085::queryTemperature()
{
	uint8_t cmd[] = { CONTROL, READTEMP };
	send(cmd,2);
}
bool HA_BMP085::queryPressure()
{
	uint8_t cmd[] = { CONTROL, READPRESSURE + (_oversampling << 6) };
	send(cmd,2);
}
void HA_BMP085::readRawTemperature()
{
	int32_t UT, X1, X2;

	UT = readBE<uint16_t>(TEMPDATA);

	// do temperature calculations
	X1 = ((UT - _calibration.ac6) * (_calibration.ac5)) >> 15;
	X2 = ((int32_t)_calibration.mc << 11) / (X1 + _calibration.md);
	_B5 = X1 + X2;

	int temp = ((_B5 + 8) << 3) / 10;
	temperature.write(temp);
}


uint32_t HA_BMP085::readRawPressure() {
	uint32_t raw;

	raw = readBE<uint16_t>(PRESSUREDATA);

	raw <<= 8;
	raw |= read<uint8_t>(PRESSUREDATA + 2);
	raw >>= (8 - _oversampling);

	return raw;
}
void HA_BMP085::process() {

	int32_t UP, B3, B6, X1, X2, X3, p;
	uint32_t B4, B7;

	UP = readRawPressure();

#if 0
	// use datasheet numbers!
	_calibration.ac6 = 23153;
	_calibration.ac5 = 32757;
	_calibration.mc = -8711;
	_calibration.md = 2868;
#endif

	// do pressure calcs
	B6 = _B5 - 4000;
	X1 = ((int32_t)_calibration.b2 * ((B6 * B6) >> 12)) >> 11;
	X2 = ((int32_t)_calibration.ac2 * B6) >> 11;
	X3 = X1 + X2;
	B3 = ((((int32_t)_calibration.ac1 * 4 + X3) << _oversampling) + 2) / 4;

	X1 = ((int32_t)_calibration.ac3 * B6) >> 13;
	X2 = ((int32_t)_calibration.b1 * ((B6 * B6) >> 12)) >> 16;
	X3 = ((X1 + X2) + 2) >> 2;
	B4 = ((uint32_t)_calibration.ac4 * (uint32_t)(X3 + 32768)) >> 15;
	B7 = ((uint32_t)UP - B3) * (uint32_t)(50000UL >> _oversampling);

	if (B7 < 0x80000000) {
		p = (B7 * 2) / B4;
	}
	else {
		p = (B7 / B4) * 2;
	}
	X1 = (p >> 8) * (p >> 8);
	X1 = (X1 * 3038) >> 16;
	X2 = (-7357 * p) >> 16;

	p = p + ((X1 + X2 + (int32_t)3791) >> 4);

	pressure.write(p);
}
void HA_BMP085::run(){	switch (_state)	{	case state_init:		init();

	case state_none:		queryTemperature();		_state = state_temperature;
		trigTask(5);		break;	case state_temperature:		readRawTemperature();		queryPressure();		_state = state_pressure;
		switch (_oversampling)
		{
		case UltraLow: trigTask(5); break;
		case Standard: trigTask(8); break;
		case High: trigTask(14); break;
		case UltraHigh: trigTask(26); break;
		}
		break;	case state_pressure:		process();		_state = state_none;		break;	}}/*********************************************************************/

