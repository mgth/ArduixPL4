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
	int compare(time_t t) const;

	/// <summary>to be overriden for task execution</summary>
	virtual void run() = 0;

public:

	/// <summary>run next task in queue if it's time to</summary>
	static void loop();

	// schedule next execution at determined delay in ms
	/// <summary>schedule next execution at determined delay from now</summary>
	/// <param name="delay">delay in ms</param>
	void trigTask(time_t delay = 0);

	// returns scheduled execution time
	time_t dueTime() const { return _dueTime; }

	// by default new task is not scheduled
	Task() { }

	// might give a delay for first execution
	Task(time_t delay) { trigTask(delay); }

	//for Task to be sortable
	int compare(const Task& task) const;
};

#endif