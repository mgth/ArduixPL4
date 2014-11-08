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

	  Modified 2014-3-24 by Mathieu GRENET 
	  mailto:mathieu@mgth.fr
	  http://www.mgth.fr
*/

#ifndef I2C_H
#define I2C_H

#include <ArduHA.h>
#include <inttypes.h>

/// <summary>base class for I2C Devices</summary>
class I2C
{

protected:
	/// i2c device address
	uint8_t _addr;
	I2C(uint8_t addr);

	/// <summary>write 1 byte command and flush response to given buffer</summary>
	/// <param name="cmd">byte command to be sent</param>
	/// <param name="buffer">buffer to get response</param>
	/// <param name="size">buffer size</param>
	int8_t request(uint8_t cmd, void* buffer, int size) const;

	/// <summary>read from I2C to given buffer</summary>
	/// <param name="buffer">buffer to get response</param>
	/// <param name="size">buffer size</param>
	int8_t request(void* ret, int size) const;

	/// <summary>write 1 byte command and send buffer content</summary>
	/// <param name="cmd">byte command to be sent</param>
	/// <param name="buffer">buffer to send</param>
	/// <param name="size">buffer size</param>
	int8_t send(uint8_t cmd, const void* ret, int size) const;

	/// <summary>write given buffer to I2C</summary>
	/// <param name="buffer">buffer to get response</param>
	/// <param name="size">buffer size</param>
	int8_t send(const void* ret, int size) const;

	/// <summary>init I2C bus</summary>
	/// <remarq>must be called before any other command</remarq>
	void init();

	/// <summary>read wariable from I2C</summary>
	/// <param name="cmd">byte command to be sent</param>
	template<typename T>
	T read(uint8_t cmd) const
	{
		T ret;
		request(cmd, &ret, sizeof(T));
		return ret;
	}

	/// <summary>write wariable to I2C</summary>
	/// <param name="cmd">byte command to be sent</param>
	/// <param name="val">variable value to be send after command</param>
	template<typename T>
	uint8_t write(uint8_t cmd,T val) const
	{
		return send(cmd, &val, sizeof(T));
	}

	/// <summary>write wariable to I2C whitout command</summary>
	/// <param name="val">variable value to be send after command</param>
	template<typename T>
	uint8_t write(T val) const
	{
		return send(&val, sizeof(T));
	}

	/// <summary>read wariable from I2C Big Endian</summary>
	/// <param name="cmd">byte command to be sent</param>
	template<typename T>
	T readBE(uint8_t cmd) const
	{
		T ret = read<T>(cmd);

		if (sizeof(T) == 2) return (T)(((uint16_t)ret << 8) | ((uint16_t)ret >> 8));

		uint8_t *p = (uint8_t*)&ret;
		for (uint8_t lo = 0, hi = sizeof(T)-1; hi>lo; lo++, hi--)
		{
			uint8_t tmp = p[lo];
			p[lo] = p[hi];
			p[hi] = tmp;
		}
		return ret;
	}

	//template<>
	//uint16_t readBE<uint16_t>(uint8_t cmd) const
	//{
	//	uint16_t ret;
	//	request(cmd, (uint8_t*)&ret, sizeof(uint16_t));
	//	ret = (ret << 8) | (ret >> 8);
	//	return ret;
	//}

};

#endif