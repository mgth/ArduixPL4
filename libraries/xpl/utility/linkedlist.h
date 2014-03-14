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
#ifndef COLLECTION_H
#define COLLECTION_H
#include <Arduino.h>

// macro to iterate all members of a list
#define foreach(cls,obj) for(cls* obj=LinkedList<cls>::first();obj;obj=obj->LinkedList<cls>::next())
// macro to iterate pointers of the linked list
#define foreachlnk(cls,lnk) for (cls** lnk = &LinkedList<cls>::_first; *lnk; lnk = &(*lnk)->LinkedList<cls>::_next)

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

template<class cls>
class Sortable
{
public:
	// place the task at the right position in the duetime queue
	void relocate(cls** lnk)
	{
		((cls*)this)->unlink();
		while (*lnk)
		{
			if ( ((cls*)this)->compare(**lnk)<0 ) break;
			lnk = &(*lnk)->next();
		}
		((cls*)this)->link(*lnk);
	}
};

template<class cls>
class LinkedList
{
protected:
	static cls* _first;
	cls* _next;


public:
	//operator cls*() { return (cls*)this; }
	// returns next element or null if this is last
	cls*& next() { return _next; }
	cls*& last() {
		cls** c = &_next;
		while (*c != NULL) c = &(*c)->_next;
		return *c;
	}
	// returns first element of the list
	static cls*& first() { return _first; }
	
	//add this element to list
	void link(cls*& lnk)
	{
		_next = lnk;
		lnk = (cls*)this;
	}

	//remove an element from the main list
	void unlink()
	{
		if (_next == this) return;
		foreachlnk (cls,lnk)
		{
			if (*lnk == this)
			{
				*lnk = _next;
				_next = (cls*)this;
				return;
			}
		}
	}

	// return number of linked elements
	static int count()
	{
		int c = 0;
		foreach(cls, obj) { c++; }
		return c;
	}

	void relocate(cls** lnk)
	{
		((cls*)this)->unlink();
		while (*lnk)
		{
			if (((cls*)this)->compare(**lnk)<0) break;
			lnk = &(*lnk)->next();
		}
		((cls*)this)->link(*lnk);
	}

	// unlink deleted elements
	~LinkedList<cls>() { unlink(); }

	// unlinked elements should point to itsef
	LinkedList<cls>() { _next = (cls*)this; }
	LinkedList<cls>(cls*& lnk) { link(lnk); }
};

template<class cls>
cls* LinkedList<cls>::_first = NULL;

//AutoCollection : new elements will be linked at construction
template<class cls>
class AutoList : public LinkedList<cls>
{
public:
	AutoList<cls>() : LinkedList<cls>(LinkedList<cls>::_first) {  };
};

#endif