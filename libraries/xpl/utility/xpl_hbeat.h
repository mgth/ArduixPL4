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
#ifndef XPL_HBEAT_H
#define XPL_HBEAT_H
#include <xpl.h>
#include "utility/option.h"

class xPL_NewconfOption : public OptionString
{
public:
	xPL_NewconfOption() : OptionString(7, cs_reconf, F("newconf"), 16, F("default")){}
	void parse(const String& value);
};

class xPL_Hbeat
{
public:
	static OptionT<int> interval;
	static OptionString newconf;
	static String instance(){ return newconf.; }
	static String source() { return String(xPL::vendor()) + "-" + String(xPL::device()) + "." + instance(); }
};

#endif