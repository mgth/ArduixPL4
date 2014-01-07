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
#ifndef LISTENER_H
#define LISTENER_H
#include "defines.h"

#include "nodes.h"

class Listener
{
public:
	StringRom key;
	Listener(StringRom s) :key(s) {}
};

template<class cls>
class ListenerParent : public Parent<cls>
{
public:
	cls* get(const StringRom& s)
	{
		for (cls* c = this->child(); c; c = c->next())
		{
			if (c->key == s) return c;
		}
		return this->add(new cls(s));
	}

	cls* get(const String& s)
	{
		for (cls* c = this->child(); c; c = c->next())
		{
			DBGLN_3(c->key, );
			if (s == c->key) return c;
		}
		return NULL;
	}

};



class MessageParser {
protected:
	String _key;
	String _value;


	bool isCommandRequest() { return (_key == S(command) && _value == S(request)); }

public:
	static StringRom msgType() { return S(cmnd); }

	bool parseKey(const StringRom& key, String& store)
	{
		if (_key == key) { store = _value; return true; }
		return false;
	}
	void setKey(const String& key){ _key = key; DBG(key); }
	void setValue(const String& value){ _value = value; DBG(value); }
	virtual bool parse() = 0;
	virtual void process() {};
	void finish() { process();  delete this; }
};

class ListenerSchemaBase : public Listener, public Node<ListenerSchemaBase>
{
public:
	ListenerSchemaBase() :Listener(S(xpl)) {}
	ListenerSchemaBase(StringRom sr) :Listener(sr) {}
	void hook(StringRom msgType, StringRom schClass, StringRom schType);
	virtual MessageParser * getParser() { return NULL; }
};

template <class cls> 
class ListenerSchema : public ListenerSchemaBase
{
public:
	ListenerSchema<cls>(StringRom sr) : ListenerSchemaBase(sr) {}
	cls* getParser() { return new cls(); }
};

class ListenerSchClass : public Listener, public Parent<ListenerSchemaBase>, public Node<ListenerSchClass>
{
public:
	ListenerSchClass(StringRom sr) :Listener(sr) {}

	//template<typename StringT>
	ListenerSchemaBase* get(const String& s)
	{
		for (ListenerSchemaBase* c = this->child(); c; c = c->next())
		{
			DBG_3(c->key);
			if (s == c->key) return c;
		}
		return NULL;
	}

};

class ListenerMsgType : public Listener, public ListenerParent<ListenerSchClass>, public Node<ListenerMsgType>
{
public:
	ListenerMsgType(StringRom sr) :Listener(sr) {}
};

class ListenersClass : public ListenerParent<ListenerMsgType>
{
public:
	template <class parser>
	void hook() { get(parser::msgType())->get(parser::schClass())->add(new ListenerSchema<parser>(parser::schType())); }
//	void hook() { get(S(xpl))->get(S(xpl))->AddChild(new ListenerSchema<parser>(parser::schType)); }
	/*
	ListenerSchemaBase& reg(StringRom msgType, StringRom schClass, StringRom schType, ListenerSchemaBase* schema)
	{
		ListenerParent<ListenerMsgType>::get(msgType)->get(schClass)->AddChild(schema);
	}*/
};




extern ListenersClass Listeners;




#endif