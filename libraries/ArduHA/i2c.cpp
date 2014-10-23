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
#include "i2c.h"
//#include "utility/twi.h"
#include "utility/twi.h"
#include "avr/delay.h"
#include "arduha.h"


I2C::I2C(uint8_t addr) :_addr(addr){ 
}

void I2C::init(){
	twi_init();
}

int8_t I2C::request(uint8_t cmd, void* data, int size) const {

	uint8_t err = 0;

	if (cmd)
		err = twi_writeTo(_addr, &cmd, 1, 1, true);

	if (!err)
	{
		return request(data,size);
	}
	return err;
}

int8_t I2C::request(void* data, int size) const {

	if (size) return twi_readFrom(_addr, (uint8_t*)data, size, true);

	return 0;
}

int8_t I2C::send(uint8_t cmd, const void* data, int size) const {

	uint8_t err = twi_writeTo(_addr, &cmd, 1, 1, false);
	if (!err)
	{
		return send(data, size);
	}
	return err;
}


int8_t I2C::send(const void* data, int size) const {

	return twi_writeTo(_addr, (uint8_t*)data, size, 1, true);

	return 0;
}

