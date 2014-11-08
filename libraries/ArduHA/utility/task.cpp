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

#define MAX_MICROS ((2^31)/1000)

Task* Task::_millisTasks = NULL;

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
void Task::sleep(time_t us) {
	while (us >= 8000000) { _sleep(WDTO_8S); us -= 8000000; }
	if (us >= 4000000)    { _sleep(WDTO_4S); us -= 4000000; }
	if (us >= 2000000)    { _sleep(WDTO_2S); us -= 2000000; }
	if (us >= 1000000)    { _sleep(WDTO_1S); us -= 1000000; }
	if (us >= 500000)     { _sleep(WDTO_500MS); us -= 500000; }
	if (us >= 250000)     { _sleep(WDTO_250MS); us -= 250000; }
	if (us >= 125000)     { _sleep(WDTO_120MS); us -= 120000; }
	if (us >= 64000)      { _sleep(WDTO_60MS); us -= 60000; }
	if (us >= 32000)      { _sleep(WDTO_30MS); us -= 30000; }
	if (us >= 16000)      { _sleep(WDTO_15MS); us -= 15000; }
}

// run task if time to, or sleep if wait==true
void Task::_run(bool wait)
{
	long d = compare(micros());
	if (wait) while (d > 0)	{
		sleep(d);
		d = compare(micros());
	}

	if (d <= 0)
	{
		//detach task before execution
		unlink();
		//actual task execution
		run();
	}
}

// task in millis queue
bool Task::_runMillis()
{
	long d = compare(millis());

	if (d <= (2^31)/1000 )
	{
		//detach task before execution
		unlink(_millisTasks);
		//convert dueTime to micros
		_dueTime *= 1000;
		//requeue in micros
		relocate();
		return true;
	}
	return false;
}

// run next task in the queue
void Task::loop(bool sleep)
{
	if (first())
	{
		first()->_run(sleep);
	}

	// deal with millis queue 
	// TODO : check is done each loop
	while (_millisTasks && _millisTasks->_runMillis());
}

void Task::trigTaskAt(time_t dueTime)
{
	_dueTime = dueTime;
	relocate();
}

void Task::trigTask(time_t delay)
{
	trigTaskAt(micros() + delay);
}

void Task::trigTaskAtMillis(time_t dueTime, Task* task)
{
	long delay = dueTime - millis();
	if (delay <= MAX_MICROS)
	{
		if (task) delete(task);
		trigTaskAt(dueTime * 1000);
	}
	else
	{
		_dueTime = dueTime;
		if(!task) task = new TaskMillis(*this);
		task->trigTask(MAX_MICROS);
	}
}


void Task::trigTaskMillis(time_t delay, Task* task)
{
	if (delay <= MAX_MICROS)
	{
		if (task) delete(task);
		trigTask(delay * 1000);
	}
	else
		//trigTaskAtMillis(millis() + delay);
	{
		_dueTime = millis() + delay;
		if (!task) task = new TaskMillis(*this);
		task->trigTask(MAX_MICROS);
	}
}

Task* Task::trigReccurent(time_t delay, time_t interval)
{
	Task* t = new RecurrentTask(*this, interval);
	t->trigTask(delay);
	return t;
}

Task* Task::trigReccurentFixed(time_t delay, time_t interval)
{
	Task* t = new RecurrentTask(*this, interval);
	t->trigTask(delay);
	return t;
}

Task* Task::trigReccurentFromStart(time_t delay, time_t interval)
{
	Task* t = new RecurrentTask(*this, interval);
	t->trigTask(delay);
	return t;
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

