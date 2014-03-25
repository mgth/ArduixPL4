/*  ArduHA - ArduixPL - xPL library for Arduino(tm)  Copyright (c) 2012/2014 Mathieu GRENET.  All right reserved.  This file is part of ArduHA / ArduixPL.    ArduixPL is free software: you can redistribute it and/or modify    it under the terms of the GNU General Public License as published by    the Free Software Foundation, either version 3 of the License, or    (at your option) any later version.    ArduixPL is distributed in the hope that it will be useful,    but WITHOUT ANY WARRANTY; without even the implied warranty of    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    GNU General Public License for more details.    You should have received a copy of the GNU General Public License    along with ArduixPL.  If not, see <http://www.gnu.org/licenses/>.	  Modified 2014-3-23 by Mathieu GRENET 	  mailto:mathieu@mgth.fr	  http://www.mgth.fr*/#
/*
Largely derived from Miles Burton Arduino Temperature Control Library
http://www.milesburton.com/?title=Dallas_Temperature_Control_Library
https://github.com/milesburton/Arduino-Temperature-Control-Library
*/
#include "ha_DS18x20.h"#include <avr/crc16.h>
#include "limits.h"
#include "debug.h"
// Model IDs
#define DS18S20MODEL 0x10  // also DS1820
#define DS18B20MODEL 0x28
#define DS1822MODEL  0x22
#define DS1825MODEL  0x3B
// OneWire commands
#define STARTCONVO      0x44  // Tells device to take a temperature reading and put it on the scratchpad
#define COPYSCRATCH     0x48  // Copy EEPROM
#define READSCRATCH     0xBE  // Read EEPROM
#define WRITESCRATCH    0x4E  // Write to EEPROM
#define RECALLSCRATCH   0xB8  // Reload from last known
#define READPOWERSUPPLY 0xB4  // Determine if device needs parasite power
#define ALARMSEARCH     0xEC  // Query bus for devices with an alarm condition
OneWire HA_DS18x20::_wire(ONE_WIRE_BUS);bool HA_DS18x20::_parasite = false;time_t HA_DS18x20::_globalInterval = maxValue<time_t>();typedef enum scratch {TEMP_LSB,
TEMP_MSB,
HIGH_ALARM_TEMP,
LOW_ALARM_TEMP,
CONFIGURATION,
INTERNAL_BYTE,
COUNT_REMAIN,
COUNT_PER_C,
SCRATCHPAD_CRC};// returns true if address is valid
bool validAddress(const uint8_t* addr)
{
	uint8_t crc = 0;
	for (byte i = 0; i < 7; i++) crc = _crc_ibutton_update(crc, addr[i]);
	return (crc == addr[7]);
}void HA_DS18x20::setResolution(uint8_t resolution){	if (_addr[0] == DS18S20MODEL) resolution = 12;
	_configuration = ((resolution-9) << 5) | B11111;}bool HA_DS18x20::readPowerSupply()
{
	if (_wire.request(READPOWERSUPPLY,_addr))
	{
		uint8_t power = _wire.read_bit();
		return power == 0;
	}
}HA_DS18x20* HA_DS18x20::getByAddr(uint8_t addr[8]){	foreach(HA_DS18x20, d)	{		if (d->_addr == addr)		{			return d;		}	}	return NULL;}void HA_DS18x20::discover(uint8_t resolution){	OneWireSearch s(_wire);

	while (s.search())
	{
		if (validAddress(s.addr()))
		{
			new HA_DS18x20(s.addr(), resolution);
		}
	}
}HA_DS18x20::HA_DS18x20(uint8_t* addr, uint8_t resolution){	if (addr)		memcpy(_addr, addr, 8);	else		memset(_addr, 0, 8);	if (!_parasite && readPowerSupply()) _parasite = true;

	setResolution(resolution);	_pendingRequest = false;}int16_t HA_DS18x20::read(){	uint8_t data[8];	_wire.request(READSCRATCH,_addr);	uint8_t crc = 0;	for (byte i = 0; i < 8; ++i)	{		data[i] = _wire.read();		crc = _crc_ibutton_update(crc, data[i]);	}	//check crc, if not ok sensor might not be there	if (crc != _wire.read()) return SHRT_MAX;	//_wire.reset();	//check if configuration is ok	if (data[CONFIGURATION] != 0xFF && data[CONFIGURATION] != _configuration)	{		_wire.request(WRITESCRATCH,_addr);		_wire.write_bytes(&data[HIGH_ALARM_TEMP],3);		_wire.request(COPYSCRATCH, _addr); //, _parasite	}	//_wire.reset();	int16_t raw = (((int16_t)data[TEMP_LSB]) << 3) | (((int16_t)(data[TEMP_MSB])) << 11);	if (data[CONFIGURATION] == 0xFF)
		raw = ((raw & 0xfff0) << 3) - 16 + (((data[COUNT_PER_C] - data[COUNT_REMAIN]) << 7) / data[COUNT_PER_C]);
	
	_pendingRequest = false;
		return raw;}bool HA_DS18x20::startConversion(){	if (_wire.request(STARTCONVO, _addr))	{		_pendingRequest = true;		return true;	}	return false;}bool HA_DS18x20::startGlobalConversion(time_t interval){	bool q = false;	foreach(HA_DS18x20, sensor)	{		if (!sensor->_pendingRequest)		{			if (!q) {				_wire.request(STARTCONVO, NULL); q = true;			}			sensor->_pendingRequest = true;			sensor->trigTask(sensor->conversionDuration(),interval);		}	}	return q;}time_t HA_DS18x20::conversionDuration(){	return 750 / (1 << (3 - (_configuration >> 5)));}void HA_DS18x20::run(){	if (_pendingRequest)	{		int16_t raw = read();		if (raw!=SHRT_MAX) temperature.write(raw);		_pendingRequest = false;		foreach(HA_DS18x20, sensor)		{			if (sensor->_pendingRequest) return;		}	}	else if (startConversion())
	{		trigTask(conversionDuration());		return;		startGlobalConversion();	}}