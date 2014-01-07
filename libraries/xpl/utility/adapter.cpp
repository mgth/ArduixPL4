/*
  ArduixPL - xPL library for Arduino(tm)
  Copyright (c) 2012/2014 Mathieu GRENET.  All right reserved.

  This file is part of ArduixPL.

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

	  Modified 2014-1-7 by Mathieu GRENET 
	  mailto:mathieu@mgth.fr
	  http://www.mgth.fr
*/

#include "adapter.h"
#include "listeners.h"
#include <avr/crc16.h>
#include <avr/eeprom.h>

void Adapter::setMac(uint8_t* mac)
{
	uint8_t crc = 0;
	for (int i = 0; i < 6; i++)
	{
		mac[i] = eeprom_read_byte((uint8_t*)i);
		crc = _crc_ibutton_update(crc, mac[i]);
	}


	if (crc != eeprom_read_byte((uint8_t*)6))
	{
		randomSeed(analogRead(0));
		crc = 0;
		for (byte i = 0; i < 6; i++)
		{
			mac[i] = random(256);
			if (i == 0) mac[0] = (random(256) & B11111110) | B00000010;
			crc = _crc_ibutton_update(crc, mac[i]);
			eeprom_write_byte((uint8_t*)i, mac[i]);
		}
		eeprom_write_byte((uint8_t*)6, crc);
	}
}

void Adapter::loop() {
	String _source;
	String _target;
	if (available())
	{
		DBGLN_3(F("\nmessage"), );

		if (cmpUntil('-', S(xpl)))
		{
			DBG_3('0');
			ListenerMsgType* msgType = Listeners.get(readStringUntil('\n'));
			if (msgType)
			{
				DBG_3('1');
				if (cmpUntil('\n', S(_open)))
				{
					DBG_3('2');
					if (cmpUntil('=', S(hop)))
					{
						DBG_3('3');
						readStringUntil('\n');
						if (cmpUntil('=', S(source)))
						{
							DBG_3('4');
							_source = readStringUntil('\n');
							if (cmpUntil('=', S(target)))
							{
								DBG_3('5');
								_target = readStringUntil('\n');
								if (cmpUntil('\n', S(_close)))
								{
									DBG_3('6');
									String test = readStringUntil('.');
									//DBGLN_3(S(test),test);
									ListenerSchClass* schClass = msgType->get(test);
									if (schClass)
									{
										DBG_3('7');
										ListenerSchemaBase* schema = schClass->get(readStringUntil('\n'));
										if (schema)
										{
											DBG_3('8');
											if (cmpUntil('\n', S(_open)))
											{
												DBG_3('9');

												MessageParser* parser = schema->getParser();
												if (parser)
												{
													while (available())
													{
														DBG_3('a');
														if (peek() == '}') break;
														DBG_3('b'); 
														parser->setKey(readStringUntil('='));
														DBG_3('c');
														parser->setValue(readStringUntil('\n'));
														DBG_3('d');
														parser->parse();
													}
													parser->finish();
													DBG_3('e');
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}


		flush();
	}
}