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