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

#ifndef XPL_ENC28J60_H
#define XPL_ENC28J60_H
#include <ArduHA.h>
#include <xpl.h>
#include "task.h"
#include "utility/xpl_adapter.h"
#include "utility/option.h"
#include "ha_spi.h"

class OptionMacAddress : public Option
{
public:
	OptionMacAddress() :Option(0) {
		if (!checkCrc())set();
	}
	byte size() const { return 6; }
	void set()
	{
		randomSeed(analogRead(0));
		OptionWriter opt(*this);
		while (opt.store((opt.pos()==1)?random(256) : (random(256) & B11111110) | B00000010));

//		for (byte i = 0; i < 6; i++){ mac[i] = (i == 1) ? random(256) : ((random(256) & B11111110) | B00000010); }
	}

	bool parse(const String& value) {
		byte mac[6] = { 0, 0, 0, 0, 0, 0 };
		byte pos = 0;

		for (byte i = 0; pos<12; i++)
		{
			byte c = value.charAt(i);
			if (!c) return false;
#if 0
			if (c >= '0' && c <= '9') c -= '0';
			else if (c >= 'A' && c <= 'F') c -= 'A' - 10;
			else if (c >= 'a' && c <= 'f') c -= 'a' - 10;
			else continue;
#else // reduce code by 6 bytes
			c -= '0';
			if (c > 9)
			{
				c -= 'A' - '0'; //6
				if (c > (0xF - 0xA))
				{
					c -= 'a' - 'A'; //6
					if (c > (0xF - 0xA)) continue; // 12
				}
				c += 0xA; // 2
			}
#endif
			if (pos & B1) c <<= 4;

			mac[pos/2] &= c;

			pos++;
		}

		storeObj(mac);

		return true;
	}

	static size_t printHexTo(Print& p, char c)
	{
		c = c & B1111;
		return p.print((char)((c < 10) ? c + '0' : c - 10 + 'A'));
	}

	size_t printTo(Print& p) const
	{
		OptionWriter opt(*this);
		int c;
		while ( (c = opt.read()) > -1)
		{
			printHexTo(p, c<<4);
			printHexTo(p, c);
			if (opt.pos() < 6) p.print(':');
		}
	}
};

class xPL_ENC28J60Adapter : public xPL_Adapter
{
	HA_Spi _spi;

	char _bank=-1;
	int _nextPacketPtr = -1;

	int _available = -1;

	byte readOp(byte op, byte address);
	void writeOp(byte op, byte address, byte data);

	void SetBank(byte address);
	byte readRegByte(byte address);
	word readReg(byte address);
	void writeRegByte(byte address, byte data);
	void writeReg(byte address, word data);
	word readPhyByte(byte address);
	void writePhy(byte address, word data);
	void filterXpl();
	void filterXplCmnd();
	void startWrite(int pos = 0);
	void printByte(byte b);
	word getChecksum(long sum);
	void printBytes(byte b, byte count);
	void printWord(word w);
	void writeHeader();
	void initialize();
	void setReadPos(int pos);
	void packetRelease();
	word packetReceive();

	byte readControlRegister(byte reg);

	void setReceiveBufferReadPointer(word ptr);
public:
	xPL_ENC28J60Adapter() :_spi(ENC28J60_PIN,0,SPI_MODE0,SPI_CLOCK_DIV2) {}

	virtual int available();
	virtual int read();
	virtual int peek();
	virtual void flush();

	virtual bool start();
	virtual size_t write(uint8_t c);
	virtual bool send();

	virtual void begin();

};


#endif