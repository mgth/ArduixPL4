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

#include "task.h"
#include <avr/wdt.h>
#include <avr/sleep.h>
#include "debug.h"

SIGNAL(WDT_vect) {
	wdt_disable();
	wdt_reset();
	WDTCSR &= ~_BV(WDIE);
}

void Task::_sleep(uint8_t wdt_period) {
	wdt_enable(wdt_period);
	wdt_reset();
	WDTCSR |= _BV(WDIE);
	//  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	set_sleep_mode(SLEEP_MODE_EXT_STANDBY);
	sleep_mode();
	wdt_disable();
	WDTCSR &= ~_BV(WDIE);
}

// sleep reducing power consumption
void Task::sleep(time_t milliseconds) {
	while (milliseconds >= 8000) { _sleep(WDTO_8S); milliseconds -= 8000; }
	if (milliseconds >= 4000)    { _sleep(WDTO_4S); milliseconds -= 4000; }
	if (milliseconds >= 2000)    { _sleep(WDTO_2S); milliseconds -= 2000; }
	if (milliseconds >= 1000)    { _sleep(WDTO_1S); milliseconds -= 1000; }
	if (milliseconds >= 500)     { _sleep(WDTO_500MS); milliseconds -= 500; }
	if (milliseconds >= 250)     { _sleep(WDTO_250MS); milliseconds -= 250; }
	if (milliseconds >= 125)     { _sleep(WDTO_120MS); milliseconds -= 120; }
	if (milliseconds >= 64)      { _sleep(WDTO_60MS); milliseconds -= 60; }
	if (milliseconds >= 32)      { _sleep(WDTO_30MS); milliseconds -= 30; }
	if (milliseconds >= 16)      { _sleep(WDTO_15MS); milliseconds -= 15; }
}

// run task if time to, or sleep if wait==true
void Task::_run(bool wait)
{
	long d= compare(millis());
	if (wait)
	{
		while (d > 0)
		{
			sleep(d);
			d = compare(millis());
		}
	}
	if (d <= 0)
	{
		//detach task before execution
		unlink();
		//actual task execution
		run();

		if (recurrent() && !linked()) trigTask(_interval);
	}
}

// run next task in the queue
void Task::loop(bool sleep)
{
	if (first())
	{
		first()->_run(sleep);
	}
	else
	{
		//TODO: what should we do if no more tasks in queue ?
		// that should never happen, maybe we should reboot ?
	}
}

// returns scheduled execution time
void Task::trigTask(time_t delay)
{
	_dueTime = millis() + delay;
	relocate();
}
void Task::trigTask(time_t delay,time_t interval)
{
	_interval = interval;
	trigTask(delay);
}

// returns scheduled position against t
long Task::compare(time_t t) const
{
	return _dueTime - t;
}

//for Task to be sortable
int Task::compare(const Task& task) const
{
	long diff = compare(task.dueTime());
	return sgn(diff);
}

