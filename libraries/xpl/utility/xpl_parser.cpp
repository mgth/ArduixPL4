#include "xpl_parser.h"

bool xPL_MessageParser::select(bool sel = true)
{
	_selected = sel;
	return sel;
}

nbparsers_t xPL_MessageParser::selectAll()
{
	nbparsers_t count = 0;
	foreach(xPL_MessageParser, p){ count += p->select(); }
	return count;
}
nbparsers_t  xPL_MessageParser::parseSelected(const xPL_KeyValue& kv)
{
	nbparsers_t count = 0;
	foreach(xPL_MessageParser, p)
	{
		DBGLN("nb:", count);
		DBGLN("count:", xPL_MessageParser::count());
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

nbparsers_t  xPL_MessageParser::processSelected()
{
	nbparsers_t count = 0;
	foreach(xPL_MessageParser, p)
	{
		if (p->selected())
		{
			count += p->process();
		}
	}
	return count;
}
nbparsers_t xPL_MessageParser::select(msgPart part, const String& s)
{
	if (s != _header.part(part)) select(false);
	return selected() ? 1 : 0;
}

nbparsers_t xPL_MessageParser::selectAll(msgPart part, const String& s) {
	nbparsers_t count = 0;
	foreach(xPL_MessageParser, p) { count += p->select(part, s); }
	return count;
}



