/*  ArduHA - ArduixPL - xPL library for Arduino(tm)  Copyright (c) 2012/2014 Mathieu GRENET.  All right reserved.  This file is part of ArduHA / ArduixPL.    ArduixPL is free software: you can redistribute it and/or modify    it under the terms of the GNU General Public License as published by    the Free Software Foundation, either version 3 of the License, or    (at your option) any later version.    ArduixPL is distributed in the hope that it will be useful,    but WITHOUT ANY WARRANTY; without even the implied warranty of    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    GNU General Public License for more details.    You should have received a copy of the GNU General Public License    along with ArduixPL.  If not, see <http://www.gnu.org/licenses/>.	  Modified 2014-3-14 by Mathieu GRENET 	  mailto:mathieu@mgth.fr	  http://www.mgth.fr*/#
// Derived from DHT Temperature & Humidity Sensor library for Arduino by Rob Tillaart
// http://arduino.cc/playground/Main/DHTLib
#include "../ha_DHT.h"HA_SensorDHT::HA_SensorDHT(uint8_t pin, byte type) :_pin(pin), _type(type){	trigTask();}// dewPoint function NOAA
// reference: http://wahiduddin.net/calc/density_algorithms.htm 
/*
double dewPoint(double celsius, double humidity)
{
        double RATIO = 373.15 / (273.15 + celsius);  // RATIO was originally named A0, possibly confusing in Arduino context
        double SUM = -7.90298 * (RATIO - 1);
        SUM += 5.02808 * log10(RATIO);
        SUM += -1.3816e-7 * (pow(10, (11.344 * (1 - 1/RATIO ))) - 1) ;
        SUM += 8.1328e-3 * (pow(10, (-3.49149 * (RATIO - 1))) - 1) ;
        SUM += log10(1013.246);
        double VP = pow(10, SUM - 3) * humidity;
        double T = log(VP/0.61078);   // temp var
        return (241.88 * T) / (17.558 - T);
}
*/
int HA_SensorDHT::read(int& temperature, int& humidity)
{
	byte bits[5];

	// READ VALUES
	int rv = read(bits);
	if (rv != DHTLIB_OK)
	{
		humidity = DHTLIB_INVALID_VALUE; // invalid value, or is NaN prefered?
		temperature = DHTLIB_INVALID_VALUE; // invalid value
		return rv;
	}

	// CONVERT AND STORE
	long h = word(bits[0], bits[1]) << 7;  // bits[1] == 0;
	long t = word(bits[2] & 0x7F, bits[3]) << 7;  // bits[3] == 0;

	if (_type > 11)
	{
		h /= 10;
		t /= 10;
		if (bits[2] & 0x80) t = -t;
	}
	humidity = h; // invalid value, or is NaN prefered?
	temperature = t; // invalid value
	// TEST CHECKSUM
	uint8_t sum = bits[0] + bits[1] + bits[2] + bits[3];
	if (bits[4] != sum) return DHTLIB_ERROR_CHECKSUM;

	return DHTLIB_OK;
}
void HA_SensorDHT::run(){	int temperature;	int humidity;	int res = read(temperature, humidity);		if (res==DHTLIB_OK)		{			_temperature.input(temperature);			_humidity.input(humidity);			//double hn = _dht.humidity * exp(
			//								4892.32623 *
			//								( 1/(273.15 + _tempNorm) - 1/(273.15 + _dht.temperature) )
			//							);
			//_humidityNorm.input(hn);			//tempDewPoint.input(dewPoint(_dht.temperature,_dht.humidity));		}		trigTask((_type == 22) ? 2000 : 1000);}// #define TIMEOUT 10000
// uint16_t for UNO, higher CPU speeds => exceed MAXINT.
// works for DUE
// 16 MHz => 10000
// 84 MHz => 52500
// 100MHz => 62500
#define TIMEOUT (F_CPU/1600)
int HA_SensorDHT::read(uint8_t bits[5])
{

	// INIT BUFFERVAR TO RECEIVE DATA
	uint8_t mask = 128;
	uint8_t idx = 0;

	// EMPTY BUFFER
	memset(bits, 0, 5);

	// REQUEST SAMPLE
	pinMode(_pin, OUTPUT);
	digitalWrite(_pin, LOW);
	delay(20);
	digitalWrite(_pin, HIGH);
	delayMicroseconds(40);
	pinMode(_pin, INPUT);

	// GET ACKNOWLEDGE or TIMEOUT
	unsigned int loopCnt = TIMEOUT;
	while (digitalRead(_pin) == LOW)
	if (--loopCnt == 0) return DHTLIB_ERROR_TIMEOUT;

	loopCnt = TIMEOUT;
	while (digitalRead(_pin) == HIGH)
	if (--loopCnt == 0) return DHTLIB_ERROR_TIMEOUT;

	// READ THE OUTPUT - 40 BITS => 5 BYTES
	for (uint8_t i = 0; i<40; i++)
	{
		loopCnt = TIMEOUT;
		while (digitalRead(_pin) == LOW)
		if (--loopCnt == 0) return DHTLIB_ERROR_TIMEOUT;

		unsigned long t = micros();

		loopCnt = TIMEOUT;
		while (digitalRead(_pin) == HIGH)
		if (--loopCnt == 0) return DHTLIB_ERROR_TIMEOUT;

		if ((micros() - t) > 40) bits[idx] |= mask;
		mask >>= 1;
		if (mask == 0)   // next byte?
		{
			mask = 128;
			idx++;
		}
	}

	return DHTLIB_OK;
}