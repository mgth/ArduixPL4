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

#include "xpl_adapter.h"
#include "xpl_parser.h"
#include "xpl_message.h"

#ifndef XPL_ADAPLTER_MULTI
xPL_Adapter* xPL_Adapter::_adapter = NULL;
#endif

xPL_Adapter::xPL_Adapter(time_t interval)
:Task(0,interval)
{
#ifndef XPL_ADAPLTER_MULTI
	_adapter = this;
#endif
};

void xPL_Adapter::run() {
	if (available())
	{
		parse();
		flush();
	}
}

size_t xPL_Adapter::sendMessage(const Printable& p)
{
	if (start())
	{
		size_t n = print(p);
		if (send()) return n;
	}
	return 0;
}

size_t xPL_Adapter::send(const Printable& p)
{
#ifndef XPL_ADAPLTER_MULTI
	if (_adapter) return _adapter->sendMessage(p);
	return 0;
#else
	size_t n = 0;
	foreach(xPL_Adapter, a)
		n += a->sendMessage(p);
	return n;
#endif
}

String xPL_Adapter::readStringUntil(char terminator)
{
	String ret = "";
	int c = readChar(terminator);
	while ( c != -1 )
	{
		ret += (char)c;
		c = readChar(terminator);
	}
	return ret;
}

int xPL_Adapter::readChar(char terminator)
{
	int c = read();
	if (c == -1 || terminator == (char)c || '\n' != (char)c) return -1;
	return c;
}

void xPL_Adapter::flushUntil(char terminator)
{
	while (readChar(terminator) >= 0);
}

char xPL_Adapter::cmpChar(char c, char terminator)
{
	int r = readChar(terminator);
	if (c == 0)
	{
		if (r == -1) return 0; // strings are eguals
	}
	else
	{
		if (r == -1) return -1; // strings are different
		if (r == c) return 1; // ok until there
	}

	flushUntil(terminator);
	return -1;
}

bool xPL_Adapter::cmpUntil(char terminator, StringRom cmp)
{
	const char* pos = (const char*)cmp;
	int c = cmpChar(pgm_read_byte(pos),terminator); 
	while (c == 0)
	{
		c = cmpChar(pgm_read_byte(pos), terminator);  pos++;
	}
	if (c>0) return true;
	return false;
}

bool xPL_Adapter::cmpUntil(char terminator, const String& cmp)
{
	int pos = 0;
	int c = cmpChar(cmp.charAt(pos), terminator);
	while (c == 0)
	{
		c = cmpChar(cmp.charAt(pos), terminator);
		pos++;
	}
	if (c>0) return true;
	return false;
}

bool xPL_Adapter::cmpUntil(char terminator, char cmp)
{
	int res = cmpChar( cmp , terminator );
	if (res == 0) { if (cmpChar('\0', terminator) > 0) return true; }
	return false;
}

void xPL_Adapter::parse() {

	String tmp;


	if (!cmpUntil('-', F("xpl"))) return;
	if (!xPL_MessageParser::selectAll()) return;
	
	if (!xPL_MessageParser::selectAll(MSGTYPE,readStringUntil('\n'))) return;

	if (!cmpUntil('\n', '{')) return;
	if (!cmpUntil('=', F("hop"))) return;

	readStringUntil('\n');

	if (!cmpUntil('=', F("source"))) return;
	tmp = readStringUntil('\n');

	if (!cmpUntil('=', F("target"))) return;
	tmp = readStringUntil('\n');

//	if (tmp != "*" && tmp != Message::source()) return;

	if (!cmpUntil('\n', '}')) return;

	if (!xPL_MessageParser::selectAll(SCHCLASS,readStringUntil('.'))) return;	
	if (!xPL_MessageParser::selectAll(SCHTYPE,readStringUntil('\n'))) return;

	if (!cmpUntil('\n', '{')) return;

	while (available())
	{
		if (peek() == '}')
		{
			xPL_MessageParser::processSelected();
			break;
		}
		xPL_KeyValue key;
		key.key = readStringUntil('=');
		key.value = readStringUntil('\n');
		xPL_MessageParser::parseSelected(key);
	}
}