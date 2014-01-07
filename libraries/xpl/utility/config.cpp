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
#include "config.h"

ConfigTask ConfigTask::Task;

bool ConfigListParser::parse()
{
	if (isCommandRequest())
		ConfigTask::Task.trigList();
}
bool ConfigCurrentParser::parse()
{
	if (isCommandRequest())
		ConfigTask::Task.trigCurrent();
}

bool ConfigResponseParser::parse()
{
	if (isCommandRequest())
		ConfigTask::Task.trigCurrent();
}


size_t ConfigList::printTo(Print& p) const
{

}

size_t ConfigCurrent::printTo(Print& p) const
{

}