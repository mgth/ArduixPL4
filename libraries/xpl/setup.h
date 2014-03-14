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

/*****************************************************************************
CORE XPL OPTIONS
*****************************************************************************/
// Allow device to be configured (about 3kB flash)
//#define XPL_CONFIG
//#define XPL_GROUPS
//#define XPL_FILTERS
//interval to send config.basic messages
#define XPL_CONFIG_INTERVAL 1
//interval to send hbeat.basic messages
#define XPL_HBEAT_INTERVAL 5
//Allow interrupts usage
//#define XPL_INTERRUPTS

/*****************************************************************************
DEBUG OPTIONS
*****************************************************************************/
// To print debug info
#define DEBUG 3
// to print memory related debug info
#define DEBUG_MEM

// to print debug info on LCD
//(you should include Wire.h & LiquidCrystal_I2C.h)
//#define DEBUG_LCD

//uncomment to reduce main loop speed, mainly useful to know whitch module hang
//#define SLOWDEBUG 500


/*****************************************************************************
ETHERNET OPTIONS
*****************************************************************************/

#define ENC28J60_PIN 10
//#define XPL_IP
#define XPL_DHCP 
// xPL default port, do not change
#define XPL_PORT 3865

#define PS2_CLK_PIN 6
#define PS2_DATA_PIN 5

// xPL device configurable (1534 octets)
#define XPL_CONFIGURE