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
#include "xpl_W5100.h"
#define XPL_DHCP
#define XPL_IP

bool W5100Adapter::start()
{ 
	return _udp.beginPacket(IPAddress(255, 255, 255, 255), XPL_PORT);
}

bool W5100Adapter::send() { return _udp.endPacket(); }

size_t W5100Adapter::write(uint8_t c) {  return _udp.write(c); }

bool W5100Adapter::begin()
{
	setMac(_mac);

#ifdef XPL_IP	
#ifdef XPL_DHCP
	if (!Ethernet.begin(_mac))
	{
		Ethernet.begin(_mac, IPAddress(192, 168, 1, 111), INADDR_NONE, INADDR_NONE, IPAddress(255, 255, 255, 0) );
	}
#else
	memset(_ip.bin, 0, 4);
	memset(_mask.bin, 0, 4);
	Ethernet.begin(_mac.bin, _ip.bin, INADDR_NONE, INADDR_NONE, _mask.bin);
#endif
//	DBGLN(F("IP:"), Ethernet.localIP);
#else
	Ethernet.begin(_mac, IPAddress(192, 168, 1, 111), INADDR_NONE, INADDR_NONE, IPAddress(255, 255, 255, 0));
#endif
	return _udp.begin(XPL_PORT);

}

