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
#include "debug.h"

#include "ha_BH1750.h"


HA_BH1750::HA_BH1750(
	time_t first,
	time_t interval,
	int addr,
	byte mode,
	bool continuous,
	byte mt
	) : I2C(addr), Task(first,interval)
{
	_waiting=false;
	setMode(mode, continuous, mt);
}

time_t HA_BH1750::_waitTime()
{
	time_t delay = ((_mode & 0x3) == BH_LRES) ? BH_L_TIME : BH_H_TIME;
	if (_mt != BH_DEFAULT_MT)
	{
		delay *= _mt;
		delay /= BH_DEFAULT_MT;
	}
	return delay;
}

void HA_BH1750::setMode(byte mode, bool continuous, byte mt) {

	_mt=(mt<32)?32:(mt>254)?254:mt;

	_mode=(continuous)?BH_CONTINUOUS:BH_ONE_TIME;

	if (!mode) _mode |= BH_LRES;
	else if (mode == 2) _mode |= BH_HRES_2;

}


void HA_BH1750::run()
{
	if (!_waiting)
	{
		uint8_t data[] = { BH_POWER_ON, BH_MT_H | (_mt >> 5), BH_MT_L | (_mt & 0x1F) };
		send(data,3);
		data[0] = _mode;
		send(data, 1);

		_waiting=true;
		trigTask(_waitTime());
	}
	else
	{
		uint32_t val = readBE<uint16_t>(0);

		val *= 10 * BH_DEFAULT_MT;
		val /= 12 * _mt;
				
	//	if (_mode & 0x03 == BH_HRES_2) val >>= 1;

		out.write(val);
		
		if ((_mode & 0xF0) == BH_ONE_TIME)
		{
			uint8_t data[] = { BH_POWER_OFF };
			send(data,1);
			_waiting=false;
		}
		else 
			trigTask(_waitTime());
	}
}
