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

//#include <arduino.h>
#include "xpl.h"
#include "hbeat.h"
#include "listeners.h"

HbeatTask HbeatTask::Task;
bool HbeatRequest::parse()
{
	return isCommandRequest() && HbeatTask::Task.trig();
}

size_t HbeatContent::printTo(Print& p) const
{
	Message::printKeyTo(p, S(interval), String(HbeatTask::Task.intervalMinutes()));
}

bool HbeatTask::begin()
{
	DBG(F("<begin hbeat>"))
	_trigHbeat = true;
	_interval = XPL_CONFIG_INTERVAL * 60000;
	_configured = false;

	Listeners.hook<HbeatRequest>();
	return true;
}

void HbeatTask::loop()
{
	if (_trigHbeat || (millis() - _lastHbeatTime) >= _interval) {
		_lastHbeatTime = millis();
		_trigHbeat = false; 

		xPL.send(Message(S(stat), _configured?S(hbeat):S(config), S(basic), "*", HbeatContent()));
	}

}

