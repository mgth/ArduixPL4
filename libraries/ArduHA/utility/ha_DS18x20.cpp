// // // #include "ha_DS18x20.h"#include <avr/crc16.h>
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
OneWire HA_DS18x20::_wire(ONE_WIRE_BUS);bool HA_DS18x20::_parasite = false;bool HA_DS18x20_Multi::_globalPendingRequest = false;typedef enum scratch {TEMP_LSB,
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
}HA_DS18x20* HA_DS18x20_Multi::getByAddr(uint8_t addr[8]){	foreach(HA_DS18x20_Multi, d)	{		if (d->_addr == addr)		{			return d;		}	}	return NULL;}void HA_DS18x20_Multi::discover(uint8_t resolution){	OneWireSearch s(_wire);

	while (s.search())
	{
		if (validAddress(s.addr()))
		{
			new HA_DS18x20_Multi(s.addr(), resolution);
		}
	}
}HA_DS18x20::HA_DS18x20(uint8_t* addr, uint8_t resolution){	if (addr)		memcpy(_addr, addr, 8);	else		memset(_addr, 0, 8);	if (!_parasite && readPowerSupply()) _parasite = true;

	setResolution(resolution);	Filter::_next = NULL; //TODO: 	_pendingRequest = false;}int16_t HA_DS18x20::read(){	uint8_t data[8];	_wire.request(READSCRATCH,_addr);	uint8_t crc = 0;	for (byte i = 0; i < 8; ++i)	{		data[i] = _wire.read();		crc = _crc_ibutton_update(crc, data[i]);	}	//check crc, if not ok sensor might not be there	if (crc != _wire.read()) return SHRT_MAX;	//_wire.reset();	//check if configuration is ok	if (data[CONFIGURATION] != 0xFF && data[CONFIGURATION] != _configuration)	{		_wire.request(WRITESCRATCH,_addr);		_wire.write_bytes(&data[HIGH_ALARM_TEMP],3);		_wire.request(COPYSCRATCH, _addr); //, _parasite	}	//_wire.reset();	int16_t raw = (((int16_t)data[TEMP_LSB]) << 3) | (((int16_t)(data[TEMP_MSB])) << 11);	if (data[CONFIGURATION] == 0xFF)
		raw = ((raw & 0xfff0) << 3) - 16 + (((data[COUNT_PER_C] - data[COUNT_REMAIN]) << 7) / data[COUNT_PER_C]);
	
	_pendingRequest = false;
		return raw;}bool HA_DS18x20::startConversion(){	if (_wire.request(STARTCONVO, _addr))	{		_pendingRequest = true;		return true;	}	return false;}bool HA_DS18x20_Multi::startGlobalConversion(){	if (_wire.request(STARTCONVO, NULL))	{		_globalPendingRequest = true;		return true;	}	return false;}time_t HA_DS18x20::conversionDuration(){	return 750 / (1 << (3 - (_configuration >> 5)));}void HA_DS18x20_Task::run(){	if (_sensor.pendingRequest())	{		int16_t raw = _sensor.read();		if (raw!=SHRT_MAX) _sensor.input(raw);	}	else if (_sensor.startConversion())
	{		trigTask(_sensor.conversionDuration());		return;	}	trigTask(_interval);}void HA_DS18x20_GlobalTask::run(){	if (HA_DS18x20_Multi::globalPendingRequest())	{		foreach(HA_DS18x20_Multi, sensor)		{			int16_t raw = sensor->read();			if (raw != SHRT_MAX) sensor->input(raw);		}	}	else if (HA_DS18x20_Multi::startGlobalConversion())
	{		time_t duration = 0;		foreach(HA_DS18x20_Multi, sensor)		{			duration = max(duration,sensor->conversionDuration());		}		trigTask(duration);		return;	}	trigTask(_interval);}