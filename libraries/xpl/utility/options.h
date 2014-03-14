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

#ifndef CONFIG_H
#define CONFIG_H

#include <xpl.h>
#include "linkedlist.h"
#include "listeners.h"
class Option;

class EepromCell
{
	uint8_t* _addr;
	public:
		EepromCell(uint8_t* addr) :_addr(addr){}
		//operator byte() const;
		byte get() const;

		void set(byte b);
		EepromCell& operator++(int)
		{
			EepromCell tmp = *this;
			_addr++;
			return tmp;
		}

		uint8_t* addr() const { return _addr; }
};

class Option : public AutoList<Option>, public Printable
{
	byte _nb;
	ConstString_t _option;
	StringRom _name;
	uint8_t* _addr;
public:
	byte nb() const { return _nb; }
	StringRom option() const { return ConstString(_option); }
	StringRom name() const { return _name; }

	Option(int addr, ConstString_t option, StringRom name)
		:_addr((uint8_t*)addr), _option(option), _name(name){}
		
	static bool configured;
	uint8_t*  addr(byte pos) const { return _addr + pos; }
	uint8_t*  addrEnd() const { return addr(size()); }

	virtual byte size() const = 0;

	//const EepromCell operator[](byte i) const;
	const EepromCell& operator[](byte pos) { return EepromCell(_addr+pos); }
	const EepromCell& operator[](byte pos) const { return EepromCell(_addr+pos); }
	bool checkCrc() const;

	// parse : parse string to initialise value
	virtual void parse(const String& s) { };

	static void parseObj(const String& value, byte* obj)
	{
		// dummy function to aloow compilation
	}

	static void parseObj(const String& value, int& obj)
	{
		obj = value.toInt();
	}

	static void parseObj(const String& value, float& obj)
	{
		obj = value.toFloat();
	}
	
	static void parseObj(const String& value, String& obj)
	{
		obj = value;
	}


	//size_t printTo(Print&p, int i) const { return p.print(i); }
	//size_t printTo(Print&p, float f) const { return p.print(f); }
	//size_t printTo(Print&p, const String& s) const { return p.print(s); }
	//size_t printTo(Print&p, byte*) const { return 0; }


	/**************************************************
	EEPROM
	***************************************************/

//	void write(byte pos,char c) const;

	void storeObj(char* obj);

	void storeObj(const String& s);

	void storeObj(StringRom obj);

//	String const getString() const;

	bool read(char* obj) const;

	static bool corrupted()
	{
		DBG(F("Eeprom corrupted... reboot"));
		//delay(1000);
		//WDTCSR = (1 << WDE) | (1 << WDCE);
		//WDTCSR = (1 << WDE);
		for (;;);
	}
};

template <typename _type>
class OptionT : public Option
{

public:
	OptionT(int addr, ConstString_t option, StringRom name, _type def)
		:Option(addr, option, name) {
		if (!checkCrc()) storeObj((char*)&def);
	}

	byte size() const { return sizeof(_type); }

	operator _type() const
	{ 
		_type obj;
		if (read((char*)&obj)) return obj;
		corrupted();
	}

	void parse(const String& s)  {
		_type obj;
		parseObj(s, obj);
		storeObj((char*)&obj);
	}

	_type operator=(_type obj)  {
		storeObj((char*)&obj);
		return obj;
	}

	size_t printTo(Print& p) const
	{
		return p.print((_type)*this);
	}
};

class OptionString : public Option
{
	byte _size;
public:
	OptionString(int addr, ConstString_t option, StringRom name,byte size, StringRom def)
		:_size(size),Option(addr, option, name) {
		if (!checkCrc()) storeObj(def);
	}

	byte size() const { return _size; }

	void parse(const String& s)  { storeObj(s); }
	void operator=(const String& s)  { parse(s); }

	operator String();
	size_t printTo(Print& p) const;
};

class OptionWriter
{
	const Option& _option;
//	byte _pos;
	byte _crc;
	EepromCell _cell;
public:
	OptionWriter(const Option& option) :_option(option), _crc(0), _cell(option.addr(0)) {}
	bool store(byte b);
	int read();
	uint8_t* waddr() const { return _cell.addr(); }// _option.addr(_pos);

	byte pos() const { return _cell.addr()- _option.addr(0); }
};

#endif