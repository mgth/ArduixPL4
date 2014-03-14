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

#include <avr/crc16.h>
#include <avr/eeprom.h>


#include "options.h"
#include "adapter.h"
#include "message.h"
#include "listeners.h"
#include "tasks.h"

bool Option::configured = true;

//EepromCell::operator byte() const { return eeprom_read_byte(_addr); }
byte EepromCell::get() const { return eeprom_read_byte(_addr); }
void EepromCell::set(byte b)
{
	if (b != get())
	{
		eeprom_write_byte(_addr, b);
	}
}

bool OptionWriter::store(byte b)
{

	if (_cell.addr() >= _option.addrEnd()) return false; // secure to not overflow
	_cell.set(b);
	_cell++;
	_crc_ibutton_update(_crc, b); // update crc with new value;

	if (_cell.addr() < _option.addrEnd()) return true; //object writing not finisned
//	DBGLN("byte:", _crc)

	_cell.set(_crc); // object writing finished, store Crc.
	return false;
}

int OptionWriter::read()
{
	//byte b = _option[_pos++];
	//if (_pos < _option.size())
	//{
	//	_crc_ibutton_update(_crc, b);
	//	return b;
	//}
	//if ( b == _crc ) return -1;
	//return -2;

	byte b = _cell.get();
	_cell++;
	if (_cell.addr() <= _option.addrEnd())
	{
		_crc_ibutton_update(_crc, b);
		return b;
	}
	if (b == _crc) return -1;
	return -2;

}

//byte Option::operator[](byte pos) const {
//	return EepromCell(addr(pos));
////	return eeprom_read_byte(addr(pos));
//}

//void Option::write(byte pos,char c) const
//{
//	if ((*this)[pos] == c) return;
//	eeprom_write_byte((uint8_t*)addr(pos), c);
//}

void Option::storeObj(char* obj) //108
{
	OptionWriter opt(*this);
	while (opt.store(*obj++));
}

void Option::storeObj(const String& s) //138
{
	OptionWriter opt(*this);
	while ( opt.store((opt.pos() < s.length()) ? s.charAt(opt.pos()) : 0) );
}

void Option::storeObj(StringRom s) //138
{
	OptionWriter opt(*this);
	while (opt.store((opt.pos() < strlen_P((const char*)s)) ? ((const char *)s)[opt.pos()] : 0));
}

 OptionString::operator String()
{
	String s = "";
	int c;
	OptionWriter opt(*this);
	while ( (c=opt.read()) > -1 ) { s += (char)c; }

	if (c==-1) return s;
	corrupted();
	return "";
}

bool Option::checkCrc() const
{
	OptionWriter opt(*this);
	int result;
	while ( (result=opt.read()) > -1);
	return result == -1;
}

size_t OptionString::printTo(Print&p) const
{
	size_t n = 0;
	int c; 
	OptionWriter opt(*this);
	while ( ( c = opt.read() ) > 0) n += p.print((char)c);
	return n;
}

bool Option::read(char* obj) const
{

	OptionWriter opt(*this);
	int c; 
	while ( (c=opt.read()) > -1)
	{
		*(obj++) = (char)c;
	}
	return (c==-1);
}

#ifdef XPL_CONFIGURE

class ConfigList : public Task, public Printable
{
public:
	ConfigList():Task(0){}

	size_t printTo(Print& p) const {
		size_t n = 0;
		foreach(Option, opt)
		{
			n += printlnTo(p, opt->option(), '=');
			n += printlnTo(p, opt->name(), (opt->nb() <= 1) ? '\n' : '['); //10
			if (opt->nb() > 1) //4
			{
				n += printlnTo(p, opt->nb(), ']'); //64
				n += p.print('\n'); // 8
			}
		}
		return n;
	}

	void run()
	{
		if (Adapter::send(Message(MessageHeader(cs_stat, F("config"), F("list")), "*", *this)))
			delete(this);
	}
};

class ConfigCurrent : public Task, public Printable
{
public:
	ConfigCurrent() :Task(0){}
	size_t printTo(Print& p) const {
		size_t n = 0; 
		foreach(Option, opt)
		{
			n += printKeyTo(p, opt->name(), *opt);
		}
		return n;
	}

	void run()
	{
			if (Adapter::send(Message(MessageHeader(cs_stat, F("config"), F("current")), "*", *this)))
				delete(this);
	}

};

class ConfigListParser : public MessageParser
{
public:
ConfigListParser() : MessageParser(MessageHeader(cs_cmnd, F("config"), F("list"))){}
	bool parse(const KeyValue& key)
	{
		if (key.is(F("command"), F("request")))
		{
			new ConfigList();
			return true;
		}
		return false;
	}
} _configList;

class ConfigCurrentParser : public MessageParser
{
public:
	ConfigCurrentParser() :MessageParser(MessageHeader(cs_cmnd, F("config"), F("current"))) {}
	bool parse(const KeyValue& kv)
	{
		if (kv.is(F("command"), F("request")))
		{
			new ConfigCurrent();
			return true;
		}
		return false;
	}
} _configCurrent;

class ConfigResponseParser : public MessageParser
{
public:
	ConfigResponseParser() :MessageParser(MessageHeader(cs_cmnd, F("config"), F("response"))) {}
	bool parse(const KeyValue& kv)
	{
		bool result = false;
		foreach(Option, opt)
		{
			if (kv.key == opt->name())
			{
				opt->parse(kv.value);
				result = true;
			}
		}
		return result;
	}
} _configResponse;

#endif