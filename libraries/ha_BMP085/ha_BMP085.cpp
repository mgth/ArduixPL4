/*  ArduHA - ArduixPL - xPL library for Arduino(tm)  Copyright (c) 2012/2014 Mathieu GRENET.  All right reserved.  This file is part of ArduHA / ArduixPL.    ArduixPL is free software: you can redistribute it and/or modify    it under the terms of the GNU General Public License as published by    the Free Software Foundation, either version 3 of the License, or    (at your option) any later version.    ArduixPL is distributed in the hope that it will be useful,    but WITHOUT ANY WARRANTY; without even the implied warranty of    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    GNU General Public License for more details.    You should have received a copy of the GNU General Public License    along with ArduixPL.  If not, see <http://www.gnu.org/licenses/>.	  Modified 2014-3-16 by Mathieu GRENET 	  mailto:mathieu@mgth.fr	  http://www.mgth.fr*/#include "ha_BMP085.h"
#include "debug.h"

typedef enum
{
	cal_AC1,
	cal_AC2,
	cal_AC3,
	cal_AC4,
	cal_AC5,
	cal_AC6,
	cal_B1,
	cal_B2,
	cal_MB,
	cal_MC,
	cal_MD
} calData_t;

#define BMP085_I2CADDR 0x77
#define BMP085_CAL  0xAA  // R   Calibration data (16 bits)
#define BMP085_CONTROL           0xF4 
#define BMP085_TEMPDATA          0xF6
#define BMP085_PRESSUREDATA      0xF6
#define BMP085_READTEMPCMD       0x2E
#define BMP085_READPRESSURECMD   0x34

#include <util/delay.h>
HA_BMP085::HA_BMP085(BMP085_res_t mode){	_oversampling = mode;

	Wire.begin();

	if (read8(0xD0) != 0x55) return;

	/* read calibration data */
	for (byte c = cal_AC1; c <= cal_MD; c++)
		_calibration[c] = read16(BMP085_CAL + (c<<1));

	trigTask();
}uint16_t HA_BMP085::readRawTemperature() {
	write8(BMP085_CONTROL, BMP085_READTEMPCMD);
	_delay_us(4500);
	return read16(BMP085_TEMPDATA);
}

uint32_t HA_BMP085::readRawPressure() {
	uint32_t raw;

	write8(BMP085_CONTROL, BMP085_READPRESSURECMD + (_oversampling << 6));

	switch (_oversampling)
	{
	case UltraLow: _delay_us(4500); break;
	case Standard: _delay_us(7500); break;
	case High: _delay_us(13500); break;
	case UltraHigh: _delay_us(25500); break;
	}

	raw = read16(BMP085_PRESSUREDATA);

	raw <<= 8;
	raw |= read8(BMP085_PRESSUREDATA + 2);
	raw >>= (8 - _oversampling);

	/* this pull broke stuff, look at it later?
	if (oversampling==0) {
	raw <<= 8;
	raw |= read8(BMP085_PRESSUREDATA+2);
	raw >>= (8 - oversampling);
	}
	*/
	return raw;
}int HA_BMP085::readTemperature() {
	int32_t UT, X1, X2, B5;     // following ds convention
	int temp;

	UT = readRawTemperature();


	// step 1
	X1 = (UT - (int32_t)(uint16_t)_calibration[cal_AC6]) * ((int32_t)(uint16_t)_calibration[cal_AC5]) >> 15;
	X2 = ((int32_t)_calibration[cal_MC] << 11) / (X1 + (int32_t)_calibration[cal_MD]);
	B5 = X1 + X2;
	temp = (B5 + 8) >> 4;
	temp <<= 7;
	temp /= 10;

	return temp;
}

long HA_BMP085::readPressure() {
	int32_t UT, UP, B3, B5, B6, X1, X2, X3, p;
	uint32_t B4, B7;

	UT = readRawTemperature();
	UP = readRawPressure();

	// do temperature calculations
	X1 = (UT - (int32_t)(uint16_t)_calibration[cal_AC6]) * ((int32_t)(uint16_t)_calibration[cal_AC5]) >> 15;
	X2 = ((int32_t)_calibration[cal_MC] << 11) / (X1 + (int32_t)_calibration[cal_MD]);
	B5 = X1 + X2;

	temperature.write( (((long)B5 + 8)<<3) /10 );

	// do pressure calcs
	B6 = B5 - 4000;
	X1 = ((int32_t)_calibration[cal_B2] * ((B6 * B6) >> 12)) >> 11;
	X2 = ((int32_t)_calibration[cal_AC2] * B6) >> 11;
	X3 = X1 + X2;
	B3 = ((((int32_t)_calibration[cal_AC1] * 4 + X3) << _oversampling) + 2) / 4;

	X1 = ((int32_t)_calibration[cal_AC3] * B6) >> 13;
	X2 = ((int32_t)_calibration[cal_B1] * ((B6 * B6) >> 12)) >> 16;
	X3 = ((X1 + X2) + 2) >> 2;
	B4 = ((uint32_t)(uint16_t)_calibration[cal_AC4] * (uint32_t)(X3 + 32768)) >> 15;
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

	return p;
}
void HA_BMP085::run(){		readPressure();		trigTask();}/*********************************************************************/

uint8_t HA_BMP085::read8(uint8_t a) {
	uint8_t ret;

	Wire.beginTransmission(BMP085_I2CADDR); // start transmission to device 
#if (ARDUINO >= 100)
	Wire.write(a); // sends register address to read from
#else
	Wire.send(a); // sends register address to read from
#endif
	Wire.endTransmission(); // end transmission

	Wire.beginTransmission(BMP085_I2CADDR); // start transmission to device 
	Wire.requestFrom(BMP085_I2CADDR, 1);// send data n-bytes read
#if (ARDUINO >= 100)
	ret = Wire.read(); // receive DATA
#else
	ret = Wire.receive(); // receive DATA
#endif
	Wire.endTransmission(); // end transmission

	return ret;
}

uint16_t HA_BMP085::read16(uint8_t a) {
	uint16_t ret;

	Wire.beginTransmission(BMP085_I2CADDR); // start transmission to device 
#if (ARDUINO >= 100)
	Wire.write(a); // sends register address to read from
#else
	Wire.send(a); // sends register address to read from
#endif
	Wire.endTransmission(); // end transmission

	Wire.beginTransmission(BMP085_I2CADDR); // start transmission to device 
	Wire.requestFrom(BMP085_I2CADDR, 2);// send data n-bytes read
#if (ARDUINO >= 100)
	ret = Wire.read(); // receive DATA
	ret <<= 8;
	ret |= Wire.read(); // receive DATA
#else
	ret = Wire.receive(); // receive DATA
	ret <<= 8;
	ret |= Wire.receive(); // receive DATA
#endif
	Wire.endTransmission(); // end transmission

	return ret;
}

void HA_BMP085::write8(uint8_t a, uint8_t d) {
	Wire.beginTransmission(BMP085_I2CADDR); // start transmission to device 
#if (ARDUINO >= 100)
	Wire.write(a); // sends register address to read from
	Wire.write(d);  // write data
#else
	Wire.send(a); // sends register address to read from
	Wire.send(d);  // write data
#endif
	Wire.endTransmission(); // end transmission
}
