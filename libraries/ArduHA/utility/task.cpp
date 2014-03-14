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
#include "task.h"

// run next task in the queue
void Task::loop()
{
	if (first())
	{
		Task& task = *first();

		if (task.compare(millis())<0)
		{
			//detach task before execution
			task.unlink();
			//actual task execution
			task.run();
		}
	}
}

// returns scheduled execution time
void Task::trigTask(time_t delay)
{
	_dueTime = millis() + delay;
	relocate(&first());
}

// returns scheduled position against t
int Task::compare(time_t t) const
{
	time_t diff = _dueTime - t;

	if (diff > LONG_MAX) return -1;
	return diff>0;
}

//for Task to be sortable
int Task::compare(const Task& task) const
{
	return compare(task.dueTime());
}

