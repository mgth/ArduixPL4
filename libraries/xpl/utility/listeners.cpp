#include "listeners.h"

bool MessageParser::select(bool sel = true)
{
	_selected = sel;
	return sel;
}

nbparsers_t MessageParser::selectAll()
{
	nbparsers_t count = 0;
	foreach(MessageParser, p){ count += p->select(); }
	return count;
}
nbparsers_t  MessageParser::parseSelected(const KeyValue& kv)
{
	nbparsers_t count = 0;
	foreach(MessageParser, p)
	{
		DBGLN("nb:", count);
		DBGLN("count:", MessageParser::count());
		if (p->selected())
		{
			int a = p->parse(kv);
			DBGLN("class:", p->header().schClass());
			DBGLN("type:", p->header().schType());

			DBGLN("a:", a);
			count += a;
		}
	}
	return count;
}

nbparsers_t  MessageParser::processSelected()
{
	nbparsers_t count = 0;
	foreach(MessageParser, p)
	{
		if (p->selected())
		{
			count += p->process();
		}
	}
	return count;
}
nbparsers_t MessageParser::select(msgPart part, const String& s)
{
	if (s != _header.part(part)) select(false);
	return selected() ? 1 : 0;
}

nbparsers_t MessageParser::selectAll(msgPart part, const String& s) {
	nbparsers_t count = 0;
	foreach(MessageParser, p) { count += p->select(part, s); }
	return count;
}



