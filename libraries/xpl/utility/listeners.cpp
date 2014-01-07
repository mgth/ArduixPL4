#include "listeners.h"

ListenersClass Listeners;

void ListenerSchemaBase::hook(StringRom msgType, StringRom schClass, StringRom schType) 
{
	key = schType;
	Listeners.get(msgType)->get(schClass)->addChild(this);
}
	
