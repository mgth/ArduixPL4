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

/// <summary>base class for eeprom storage</summary>
class Option : public Printable
{
	uint8_t* _addr;
public:
	Option(int addr)
		:_addr((uint8_t*)addr){}
		
	static bool configured;

	/// <summary>to test reset pin status</summary>
	/// <returns> true if reset pin is grounded</returns>
	static bool reset()
	{
		pinMode(HA_RESET_PIN, INPUT);
		digitalWrite(HA_RESET_PIN, HIGH); // let it float
		if (digitalRead(HA_RESET_PIN)) return false;
		return true;
	}

	/// <summary>get address of specified index within option</summary>
	/// <param name="pos">index within option</param>
	/// <returns>address of the nth byte of the option</returns>
	uint8_t*  addr(byte pos=0) const { return _addr + pos; }

	/// <summary>get address of crc byte</summary>
	uint8_t*  addrCrc() const { return addr(size()); Option::reset(); }

	/// <summary>get next address </summary>
	uint8_t*  addrNext() const { return addrCrc() + 1; }

	/// <summary>get option size in bytes</summary>
	virtual byte size() const = 0;

	/// <summary>get byte at specified location</summary>
	/// <param name="pos">index within option</param>
	byte get(byte pos) const;

	/// <summary>set byte at specified location</summary>
	/// <param name="pos">index within option</param>
	void set(byte pos, byte b) const;

	/// <summary>check crc</summary>
	/// <returns>true if crc is ok</returns>
	bool checkCrc() const;


	/**************************************************
	String Parsing
	***************************************************/
public:
	/// <summary>try to generate value from string</summary>
	/// <returns>true if conversion is succesful</returns>
	virtual bool parse(const String& s) { };

protected:

	/// <summary>get an int from string value</summary>
	static bool parseObj(const String& value, int& obj)
	{
		obj = value.toInt();
	}

	/// <summary>get a float from string value</summary>
	static bool parseObj(const String& value, float& obj)
	{
		obj = value.toFloat();
	}
	
	/// <summary>get a float from string value</summary>
	static bool parseObj(const String& value, String& obj)
	{
		obj = value;
	}


	/**************************************************
	EEPROM
	***************************************************/

protected:
	/// <summary>Store binay object to eeprom</summary>
	bool storeObj(void* obj);

	/// <summary>Store String to eeprom</summary>
	/// <remarq>If exed capacity size, the string is truncated</remarq>
	void storeObj(const String& s);

	/// <summary>Store program space string to eeprom</summary>
	/// <remarq>used for default value storage</remarq>
	void storeObj(StringRom obj);

	/// <summary>Read binary object</summary>
	/// <returns>true if read is succesful and crc is ok</returns>
	void read(char* obj) const;

	/// <summary>Fatal function to be called when crc check fail</summary>
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

/// <summary>Helper to access option raw value and compute CRC</summary>
class OptionWriter
{
	const Option& _option;
	byte _pos;
	byte _crc;

public:
	OptionWriter(const Option& option) :_option(option), _crc(0), _pos(0) {}

	/// <summary>store byte to next position</summary>
	bool store(byte b);

	/// <summary>read next byte</summary>
	int read();

	/// <summary>current reader/writer position</summary>
	byte pos() const { return _pos; }
};

/// <summary>template </summary>
template <typename T>
class OptionT : public Option
{

public:
	OptionT(int addr, T def)
		:Option(addr) {
		if (reset() || !checkCrc()) storeObj((char*)&def);
	}

	/// <summary>binay size of the option</summary>
	byte size() const { return sizeof(T); }

	/// <summary>get actual option value</summary>
	operator T() const
	{ 
		T obj;
		read((char*)&obj);
		return obj;
	}

	/// <summary>parse a string to convert to actual type</summary>
	bool parse(const String& s)  {
		T obj;
		return parseObj(s, obj) && storeObj((char*)&obj);
	}

	/// <summary>affect value to option</summary>
	T operator=(T obj)  {
		storeObj((char*)&obj);
		return obj;
	}
private:
	/// <summary>vey basic Printable implementation</summary>
	size_t printTo(Print& p) const
	{
		return p.print((T)*this);
	}
};

/// <summary>specific String implementation</summary>
template<>
class OptionT<String> : public Option
{
	byte _size;
public:
	OptionT(int addr, byte size, StringRom def)
		:_size(size),Option(addr) {
		if (!checkCrc() || reset()) storeObj(def);
	}

	/// <summary>binay size of the option</summary>
	/// <remarq>note this is not actual string size, but option capacity</remarq>
	byte size() const { return _size; }

	/// <summary>parse implementation for string</summary>
	bool parse(const String& s)  { storeObj(s); return true; }

	/// <summary>affect value to option</summary>
	void operator=(const String& s)  { storeObj(s); }
	void operator=(StringRom s)  { storeObj(s); }


	/// <summary>print the string sirectly from eeprom</summary>
	size_t printTo(Print&p) const
	{
		size_t n = 0;
		int c;
		OptionWriter opt(*this);
		while ((c = opt.read()) >= 0) if (c) n += p.print((char)c);
		if (c == -1) return n;
		corrupted();
		return 0;
	}

	/// <summary>get actual option value</summary>
	operator String()
	{
		String s("");

		s.reserve(size());

		int c;
		OptionWriter opt(*this);
		while ((c = opt.read()) > -1) { if (c) s += (char)c; }

		if (c == -1) return s;
		corrupted();
		return "";
	}


};


#endif