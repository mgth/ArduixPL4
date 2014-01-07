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
#include "devices.h"

#include "xpl.h"
#include "message.h"

DevicesClass Devices;
Device* DeviceParser::device() { return Devices.get(_keyDevice, _keyType); }

Device* DevicesClass::get(const String& name, const String& type) {
//	for (Device* dev = this->child(); dev; dev=dev->next())
	for (Iterator<Device> iDevice = List<Device>::first(); iDevice; ++iDevice)
	{
		if (iDevice->Is(name, type)) return &*iDevice;
	}
	return NULL;
}

size_t SensorContent::printTo(Print& p) const {
	size_t n =
		Message::printKeyTo(p, S(device), _device.name()) +
		Message::printKeyTo(p, S(type), _device.type());

	String value;

	if (_request == S(current)) value = _device.current();
	else if (_request == S(name)) value = _device.name();
	else return n;
	n += Message::printKeyTo(p, _request, value);
	return n;
}

void SensorRequest::process() {
		xPL.send(Message(S(stat), S(sensor), S(basic), "*", SensorContent(*device(), _request)));
	
	}