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
	bool _microsTiming;

	/// <returns>scheduled position against t (<0 if before, >0 if after)</returns>
	/// <param name="t">time in ms or us</param>
	/// <param name="microsTiming">if true delay is us</param>
	long compare(time_t t, bool microsTiming = false) const;


public:

	/// <summary>run next task in queue if it's time to</summary>
	/// <param name="sleep">if true reduce power consuption until next task</param>
	static void loop(bool sleep=false);

	/// <summary>do nothing for some time, potencialy use power save</summary>
	/// <param name="delay">delay in ms</param>
	void sleep(time_t delay);

	/// <summary>to be overriden for task execution</summary>
	virtual void run() = 0;

	bool microsTiming();

	/// <summary>schedule next execution at determined due time</summary>
	/// <param name="duetime">time in ms or us</param>
	/// <param name="microsTiming">if true duetime is us</param>
	void trigTaskAt(time_t duetime, bool microsTiming = false);

	/// <summary>schedule next execution at determined delay from now</summary>
	/// <param name="delay">delay in ms</param>
	/// <param name="microsTiming">if true delay is us</param>
	void trigTask(time_t delay = 0, bool microsTiming = false);

	/// <summary>get scheduled execution time</summary>
	/// <returns>scheduled execution time</returns>
	time_t dueTime() const { return _dueTime; }


	/// <remarq>by default new task is not scheduled</remarq>
	Task(bool microsTiming=false) :_microsTiming(microsTiming){}

	/// <summary>New task to be scheduled </summary>
	/// <param name="delay">schedule first execution in ms from now</param>
	Task(time_t delay, bool microsTiming=false):_microsTiming(microsTiming) {
		trigTask(delay);
	}

	/// <remarks>for Task to be sortable</remarks>
	/// <param name="task">the task to compare this to</param>
	/// <returns><0 if this is scheduled before task, >0 if this is scheduled after</returns>
	int compare(const Task& task) const;
};


class RecurrentTaskFixed : public Task
{
	time_t _interval;
	Task& _task;

	RecurrentTaskFixed(Task& task, time_t interval) :
		_interval(interval),
		_task(task)
		{}

	void run()
	{
		_task.run();
		trigTaskAt(_dueTime + _interval);
	}
};

class RecurrentTaskFromStart : public Task
{
	time_t _interval;
	Task& _task;

	RecurrentTaskFromStart(Task& task, time_t interval) :
		_interval(interval),
		_task(task)
	{}

	void run()
	{
		trigTask(_interval);
		_task.run();
	}
};

class RecurrentTask : public Task
{
	time_t _interval;
	Task& _task;

	void run()
	{
		_task.run();
		trigTask(_interval);
	}

public:
	RecurrentTask(Task& task, time_t interval) :
		_interval(interval),
		_task(task)
	{}

	/// <summary>get interval for recuring tasks</summary>
	/// <returns>interval in ms</returns>
	time_t interval() const { return _interval; }

};

#endif