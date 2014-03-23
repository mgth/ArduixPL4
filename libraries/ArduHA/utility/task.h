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

#ifndef TASKS_H
#define TASKS_H

#include "linkedlist.h"
#include "limits.h"

typedef unsigned long time_t;

/// <summary>Base class for task scheduling</summary>
class Task
	:public LinkedList<Task>
{
	time_t _dueTime;

protected:
	/// <returns>scheduled position against t (<0 if before, >0 if after)</returns>
	long compare(time_t t) const;

	/// <summary>to be overriden for task execution</summary>
	virtual void run() = 0;

public:

	/// <summary>run next task in queue if it's time to</summary>
	static long loop();

	/// <summary>schedule next execution at determined delay from now</summary>
	/// <param name="delay">delay in ms</param>
	void trigTask(time_t delay = 0);

	/// <summary>get scheduled execution time</summary>
	/// <returns>scheduled execution time</returns>
	time_t dueTime() const { return _dueTime; }

	/// <remarq>by default new task is not scheduled</remarq>
	Task() { }

	/// <summary>New task to be scheduled by default</summary>
	/// <param name="delay">schedule first execution in ms from now</param>
	Task(time_t delay) { trigTask(delay); }

	/// <remarks>for Task to be sortable</remarks>
	/// <param name="task">the task to compare this to</param>
	/// <returns><0 if this is scheduled before task, >0 if this is scheduled after</returns>
	int compare(const Task& task) const;
};

#endif