#ifndef NODE_H
#define NODE_H
#include <Arduino.h>
#include "debug.h"

class NodeC {
protected:
public:
	NodeC* _next;
	NodeC() { _next = NULL; }
	void add(NodeC& n) {
		n._next = _next;
		_next = &n;
	}
};

template<class cls>
class Node : public NodeC
{
public:
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
	cls* addChild(cls* c)
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