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
#include "xpl.h"

 size_t Message::printTo(Print& p) const
	{
		return
			p.print("xpl-") +
			p.print(_msgType) +
			p.print("\n{\nhop=1\n") +
			printKeyTo(p,S(source),xPL.Source()) +
			printKeyTo(p, S(target),_target) +
			p.print("}\n") +
			p.print(_schClass) + p.print('.') + p.print(_schType) +
			p.print("\n{\n") +
			_content.printTo(p) +
			p.print("}\n");
	}