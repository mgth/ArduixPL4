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

template<class cls> class List;
template<class cls> class Iterator;


template<class cls>
class Node 
{
private:
	cls* _data;
	Node<cls>* _next;

public:
	Node(cls& data):_data(&data),_next(NULL) { }

	friend class List<cls>;
	friend class Iterator<cls>;
};

template<class cls>
class Iterator
{
private:
	Node<cls>* _position;
	Node<cls>* _first;

public:
	Iterator() {};
	cls& operator*() const { return *_position->_data; }     /* use instead of * */
	cls* operator->() const { return _position->_data; }
	void operator++() { _position = _position->_next; }            /* use instead of ++ */
	bool operator==(Iterator b) const { return _position = b._position; } /* use instead of == */

	operator bool() const {
		if (_position) return (_position->_next != NULL);
		return _first == NULL;
	}

	friend class List<cls>;
};


template<class cls>
class List
{
private:
	Node<cls>* _first;
public :
	Iterator<cls> first()
	{
		Iterator<cls> iter;
		iter._position = iter._first = _first;
		return iter;
	}

	List() :_first(NULL) { }

	void add(cls& data)
	{
		Node<cls>* newNode = new Node<cls>(data);
		newNode->_next = _first;
		_first = newNode;
	}

	void add(cls* data)
	{
		if (data)
			add(*data);
	}

	void insert(Iterator<cls> iter, cls& data)
	{
		if (iter._position == NULL)
		{
			add(data);
		}
		else
		{
			Node<cls>* newNode = new Node<cls>(data);
			newNode->_next = iter._position->_next;
			iter._position->_next = newNode;
		}
	}


};

#endif