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
#include "xpl_parser.h"

bool xPL_MessageParser::select(bool sel = true)
{
	_selected = sel;
	return sel;
}

nbparsers_t xPL_MessageParser::selectAll()
{
	nbparsers_t count = 0;
	foreach(xPL_MessageParser, p){ count += p->select(); }
	return count;
}
nbparsers_t  xPL_MessageParser::parseSelected(const xPL_KeyValue& kv)
{
	nbparsers_t count = 0;
	foreach(xPL_MessageParser, p)
	{
		DBGLN("nb:", count);
		DBGLN("count:", xPL_MessageParser::count());
		if (p->selected())
		{
			int a = p->parse(kv);
			DBGLN("class:", p->header().schClass());
			DBGLN("type:", p->header().schType());

			DBGLN("a:", a);
			count += a;
		}
	}
	return count;
}

nbparsers_t  xPL_MessageParser::processSelected()
{
	nbparsers_t count = 0;
	foreach(xPL_MessageParser, p)
	{
		if (p->selected())
		{
			count += p->process();
		}
	}
	return count;
}
nbparsers_t xPL_MessageParser::select(msgPart part, const String& s)
{
	if (s != _header.part(part)) select(false);
	return selected() ? 1 : 0;
}

nbparsers_t xPL_MessageParser::selectAll(msgPart part, const String& s) {
	nbparsers_t count = 0;
	foreach(xPL_MessageParser, p) { count += p->select(part, s); }
	return count;
}



