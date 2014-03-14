#ifndef CONFIG_H
#define CONFIG_H

#include "linkedlist.h"
#include "romstrings.h"
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
	byte _optionType;
	StringRom _name;
	uint8_t* _addr;
public:
	byte nb() const { return _nb; }
	byte optionType() const { return _optionType; }
	StringRom name() const { return _name; }

	Option(int addr, byte optionType, StringRom name)
		:_addr((uint8_t*)addr), _optionType(optionType), _name(name){}
		
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
#ifdef DEBUG 
		DBG(F("Eeprom corrupted... reboot"));
		for (;;);
#else
		WDTCSR = (1 << WDE) | (1 << WDCE);
		WDTCSR = (1 << WDE);
#endif
	}
};

template <typename _type>
class OptionT : public Option
{

public:
	OptionT(int addr, byte optionType, StringRom name, _type def)
		:Option(addr, optionType, name) {
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
	OptionString(int addr, byte optionType, StringRom name, byte size, StringRom def)
		:_size(size),Option(addr, optionType, name) {
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