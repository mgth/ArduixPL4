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
#ifndef CONFIG_H
#define CONFIG_H

#include "linkedlist.h"
#include "romstrings.h"
#include "debug.h"
class Option;


class Option : public Printable
{
	uint8_t* _addr;

public:
	Option(int addr)
		:_addr((uint8_t*)addr){}
		
	static bool configured;

	static bool reset() {
		pinMode(HA_RESET_PIN, INPUT);
		digitalWrite(HA_RESET_PIN, HIGH); // let it float
		if (digitalRead(HA_RESET_PIN)) return false;
		return true;
	}

	uint8_t*  addr(byte pos) const { return _addr + pos; }
	uint8_t*  addrCrc() const { return addr(size()); }
	uint8_t*  addrNext() const { return addrCrc()+1; }

	virtual byte size() const = 0;

	byte get(byte pos) const;
	void set(byte pos, byte b) const;

	//const EepromCell operator[](byte i) const;
	//const EepromCell& operator[](byte pos) { return EepromCell(_addr+pos); }
	//const EepromCell& operator[](byte pos) const { return EepromCell(_addr+pos); }


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




	/**************************************************
	EEPROM
	***************************************************/

//	void write(byte pos,char c) const;

	void storeObj(void* obj);

	void storeObj(const String& s);

	void storeObj(StringRom obj);

//	String const getString() const;

	bool read(char* obj) const;

	static bool corrupted()
	{
#ifdef DEBUG 
		DBG(F("Eeprom corrupted... reboot"));
		for (;;);
#else
		WDTCSR = (1 << WDE) | (1 << WDCE);
		WDTCSR = (1 << WDE);
#endif
	}
};

template <typename T>
class OptionT : public Option
{

public:
	OptionT(int addr, T def)
		:Option(addr) {
		if (reset() || !checkCrc()) storeObj((char*)&def);
	}

	byte size() const { return sizeof(T); }

	operator T() const
	{ 
		T obj;
		if (read((char*)&obj)) return obj;
		corrupted();
	}

	void parse(const String& s)  {
		T obj;
		parseObj(s, obj);
		storeObj((char*)&obj);
	}

	T operator=(T obj)  {
		storeObj((char*)&obj);
		return obj;
	}

	size_t printTo(Print& p) const
	{
		return p.print((T)*this);
	}
};

class OptionString : public Option
{
	byte _size;
public:
	OptionString(int addr, byte size, StringRom def)
		:_size(size),Option(addr) {
		if (!checkCrc() || reset()) storeObj(def);
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
	byte _pos;
	byte _crc;
	
public:
	OptionWriter(const Option& option) :_option(option), _crc(0), _pos(0) {}
	bool store(byte b);
	int read();
	uint8_t* addr() const { return _option.addr(_pos); }

	byte pos() const { return _pos; }
};

#endif