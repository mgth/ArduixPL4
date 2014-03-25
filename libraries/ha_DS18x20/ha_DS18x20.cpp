/*
/*
Largely derived from Miles Burton Arduino Temperature Control Library
http://www.milesburton.com/?title=Dallas_Temperature_Control_Library
https://github.com/milesburton/Arduino-Temperature-Control-Library
*/

#include "limits.h"
#include "debug.h"

#define DS18S20MODEL 0x10  // also DS1820
#define DS18B20MODEL 0x28
#define DS1822MODEL  0x22
#define DS1825MODEL  0x3B

#define STARTCONVO      0x44  // Tells device to take a temperature reading and put it on the scratchpad
#define COPYSCRATCH     0x48  // Copy EEPROM
#define READSCRATCH     0xBE  // Read EEPROM
#define WRITESCRATCH    0x4E  // Write to EEPROM
#define RECALLSCRATCH   0xB8  // Reload from last known
#define READPOWERSUPPLY 0xB4  // Determine if device needs parasite power
#define ALARMSEARCH     0xEC  // Query bus for devices with an alarm condition

TEMP_MSB,
HIGH_ALARM_TEMP,
LOW_ALARM_TEMP,
CONFIGURATION,
INTERNAL_BYTE,
COUNT_REMAIN,
COUNT_PER_C,
SCRATCHPAD_CRC
bool validAddress(const uint8_t* addr)
{
	uint8_t crc = 0;
	for (byte i = 0; i < 7; i++) crc = _crc_ibutton_update(crc, addr[i]);
	return (crc == addr[7]);
}
	_configuration = ((resolution-9) << 5) | B11111;
{
	if (_wire.request(READPOWERSUPPLY,_addr))
	{
		uint8_t power = _wire.read_bit();
		return power == 0;
	}
}

	while (s.search())
	{
		if (validAddress(s.addr()))
		{
			new HA_DS18x20(s.addr(), resolution);
		}
	}
}

	setResolution(resolution);
		raw = ((raw & 0xfff0) << 3) - 16 + (((data[COUNT_PER_C] - data[COUNT_REMAIN]) << 7) / data[COUNT_PER_C]);
	
	_pendingRequest = false;
	
	{