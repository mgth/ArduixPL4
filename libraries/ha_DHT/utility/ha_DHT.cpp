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

#include "../ha_DHT.h"
#include "debug.h"
#include "util/delay.h"

HA_DHTxx::HA_DHTxx( uint8_t pin) : _pin(pin)
{
	_lastSampling = micros();
}

// dewPoint function NOAA
// reference: http://wahiduddin.net/calc/density_algorithms.htm 

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


dhtStatus_t HA_DHTxx::read11(double& temp, double& hum)
{
	byte bits[4];

	// READ VALUES
	dhtStatus_t rv = read(bits,true);
	if (rv != DHT_OK) return rv;

	// CONVERT AND STORE
	hum = bits[0];
	temp = bits[2];

	return DHT_OK;
}



dhtStatus_t HA_DHTxx::read(double& temp, double& hum)
{
	byte bits[4];

	// READ VALUES
	dhtStatus_t rv = read(bits,false);
	if (rv != DHT_OK) return rv;

	// CONVERT AND STORE
	hum = (long)word(bits[0], bits[1]) / 10;
	temp = (long)word(bits[2] & 0x7F, bits[3]) / 10;

	if (bits[2] & 0x80) temp = -temp;

	return DHT_OK;
}

int HA_DHTxx::readWhile(bool state)
{
	int duration;
	time_t m = micros();
	while ( (duration=micros()-m)<100)
	if (digitalRead(_pin) != state) return duration;

	return -1;
}

dhtStatus_t HA_DHTxx::read(uint8_t* bits, bool dht11)
{

	// INIT BUFFERVAR TO RECEIVE DATA
	uint8_t sum = 0;

	// REQUEST SAMPLE
	pinMode(_pin, OUTPUT);
	digitalWrite(_pin, LOW);
	if (dht11) _delay_us(18); else _delay_ms(1); // 1 to 10 ms
	pinMode(_pin, INPUT_PULLUP);

	// GET ACKNOWLEDGE or TIMEOUT
	if (readWhile(HIGH)<0) return DHT_TIMEOUT; // 20-40us

	if (readWhile(LOW) < 0 ) return DHT_TIMEOUT; // 80us
	if (readWhile(HIGH) < 0 ) return DHT_TIMEOUT; // 80us

	// READ THE OUTPUT - 40 BITS => 5 BYTES
	byte value = 0;
	for (byte i = 0; i < 40; i++)
	{
		value <<= 1;

		if (readWhile(LOW) < 0 ) return DHT_TIMEOUT; // 50us

		long duration = readWhile(HIGH);

		if ( duration < 0 ) return DHT_TIMEOUT;

		if (duration > 50) value |= 1 ; // 26-28us -> 0; 70us 1

		if ( (i & B11100111) == B111)   // next byte?
		{
			sum += (*bits++ = value);
		}
	}

	// TEST CHECKSUM

	if (sum != value) return DHT_BADCHECKSUM;

	_lastSampling = micros();

	return DHT_OK;
}