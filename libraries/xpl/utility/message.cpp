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

#include "message.h"
#include "options.h"
#include "hbeat.h"
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

size_t printCloseTo(Print& p)
{
	return printlnTo(p, '}');
}
size_t printOpenTo(Print& p)
{
	return printlnTo(p, '{');
}

class HeaderSection : public Printable
{
	String _target;
public:
	HeaderSection(String target) :_target(target){}
	size_t printTo(Print&p) const
	{
		return
			printKeyTo(p, F("hop"), 1) +
			printlnTo(p, F("source"), '=') + printlnTo(p, xPL::vendor(), '-') + printlnTo(p, xPL::device(), '.') + printlnTo(p, Hbeat::newconf) +
			printKeyTo(p, F("target"), _target);
	}
};

class Section : public Printable
{
	StringRom _left;
	StringRom _right;
	const Printable& _content;
public:
	Section(StringRom left, StringRom right, const Printable& content) :_left(left),_right(right),_content(content){}
	//Section(const Printable& content) :_content(content){}
	size_t printTo(Print& p) const {
		return
			printKeyTo(p, _left, _right, _left==F("xpl")?'-':'.') +
			printOpenTo(p) +
			p.print(_content) +
			printCloseTo(p);
	}
};

size_t printSectionTo(Print& p, StringRom left, StringRom right, const Printable& content)
{
	return
		printKeyTo(p, left, right, left == F("xpl") ? '-' : '.') +
		printOpenTo(p) +
		p.print(content) +
		printCloseTo(p);
}

size_t Message::printTo(Print& p) const
{
	
	return
		printSectionTo(p,F("xpl"), _header.msgType(), HeaderSection(_target)) +
		printSectionTo(p,_header.schClass(), _header.schType(), _content);
}
