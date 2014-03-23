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

// run next task in the queue
long Task::loop()
{
	if (first())
	{
		Task& task = *first();
		long d = task.compare(millis());
		if (d<0)
		{
			//detach task before execution
			task.unlink();
			//actual task execution
			task.run();
			if (first())
			{
				// return time to next task
				return first()->compare(millis());
			}
			else
				// no task in q
				return -1;
		}
		// return time to next task
		return d;
	}
	// no task in q
	return -1;
}

// returns scheduled execution time
void Task::trigTask(time_t delay)
{
	_dueTime = millis() + delay;
	relocate();
}

// returns scheduled position against t
long Task::compare(time_t t) const
{
	return _dueTime - t;
	
//	if (diff > LONG_MAX) return -1;
//	return diff>0;
}

//for Task to be sortable
int Task::compare(const Task& task) const
{
	long diff = compare(task.dueTime());
	return sgn(diff);
}

