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

	  Modified 2014-3-23 by Mathieu GRENET 
	  mailto:mathieu@mgth.fr
	  http://www.mgth.fr
*/
#
/*
make use of code from Narcoleptic :
https://code.google.com/p/narcoleptic/
*/

#ifndef TASKS_H
#define TASKS_H

#define TASK_NO_RECURRENCE (maxValue<time_t>())

#include <ArduHA.h>
#include "linkedlist.h"

/// <summary>Base class for task scheduling</summary>
class Task
	:public LinkedList<Task>
{

	static void _sleep(uint8_t wdt_period);
	/// <summary>internal task execution engine</summary>
	void _run(bool sleep=false);

protected:
	time_t _dueTime;
	time_t _interval;
	/// <returns>scheduled position against t (<0 if before, >0 if after)</returns>
	long compare(time_t t) const;

	/// <summary>to be overriden for task execution</summary>
	virtual void run() = 0;

public:

	/// <summary>run next task in queue if it's time to</summary>
	static void loop(bool sleep=false);

	/// <summary>do nothing for some time, potencialy use power save</summary>
	/// <param name="delay">delay in ms</param>
	static void sleep(time_t delay);

	/// <summary>schedule next execution at determined delay from now</summary>
	/// <param name="delay">delay in ms</param>
	void trigTask(time_t delay = 0);
	/// <summary>schedule next execution and recurring interval</summary>
	/// <param name="delay">delay in ms</param>
	/// <param name="interval">interval in ms</param>
	void trigTask(time_t delay, time_t interval);

	/// <summary>get scheduled execution time</summary>
	/// <returns>scheduled execution time</returns>
	time_t dueTime() const { return _dueTime; }

	/// <summary>get interval for recuring tasks</summary>
	/// <returns>interval in ms</returns>
	time_t interval() const { return _interval; }

	/// <summary>get task recurrence</summary>
	/// <returns>true if recurrence scheduled</returns>
	bool recurrent() { return _interval != TASK_NO_RECURRENCE; }

	/// <remarq>by default new task is not scheduled</remarq>
	Task() :_interval(TASK_NO_RECURRENCE){}

	/// <summary>New task to be scheduled and reccuring</summary>
	/// <param name="delay">schedule first execution in ms from now</param>
	/// <param name="interval">interval in ms</param>
	Task(time_t delay, time_t interval = TASK_NO_RECURRENCE) { trigTask(delay, interval); }

	/// <remarks>for Task to be sortable</remarks>
	/// <param name="task">the task to compare this to</param>
	/// <returns><0 if this is scheduled before task, >0 if this is scheduled after</returns>
	int compare(const Task& task) const;
};

#endif