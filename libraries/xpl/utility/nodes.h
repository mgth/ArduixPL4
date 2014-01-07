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
#ifndef NODE_H
#define NODE_H
#include <Arduino.h>
#include "debug.h"

template<class cls>
class Node 
{
private:
	cls* _data;

public:
	cls* _next;
	Node() { _next = NULL; }
	cls* next() { return (cls*)_next; }
};

template<class cls>
class Parent
{
private:
	cls* _child;
public :
	Parent() { _child = NULL; }

	cls* child() { return _child; }
	cls* add(cls* c)
	{
		if (_child)
		{
			cls* node = _child;
			while (node->next())
			{
				DBG('a');
				node = node->next();
			}
			DBG('b');
			node->_next = c;
			DBG('c');

		}
		else
		{
			_child = c;
			DBG('1');
		}
		c->_next = NULL;
/*
		c->_next = _child;
		_child = c;
*/
		return c;
	}
};

#endif