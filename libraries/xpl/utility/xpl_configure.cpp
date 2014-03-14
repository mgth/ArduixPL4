#ifdef XPL_CONFIGURE

class ConfigList : public Task, public Printable
{
public:
	ConfigList() :Task(0){}

	size_t printTo(Print& p) const {
		size_t n = 0;
		foreach(Option, opt)
		{
			n += printlnTo(p, opt->optionType(), '=');
			n += printlnTo(p, opt->name(), (opt->nb() <= 1) ? '\n' : '['); //10
			if (opt->nb() > 1) //4
			{
				n += printlnTo(p, opt->nb(), ']'); //64
				n += p.print('\n'); // 8
			}
		}
		return n;
	}

	void run()
	{
		if (xPL_Adapter::send(Message(xPL_MessageHeader(cs_stat, F("config"), F("list")), "*", *this)))
			delete(this);
	}
};

class ConfigCurrent : public Task, public Printable
{
public:
	ConfigCurrent() :Task(0){}
	size_t printTo(Print& p) const {
		size_t n = 0;
		foreach(Option, opt)
		{
			n += printKeyTo(p, opt->name(), *opt);
		}
		return n;
	}

	void run()
	{
		if (xPL_Adapter::send(Message(xPL_MessageHeader(cs_stat, F("config"), F("current")), "*", *this)))
			delete(this);
	}

};

class ConfigListParser : public xPL_MessageParser
{
public:
	ConfigListParser() : xPL_MessageParser(xPL_MessageHeader(cs_cmnd, F("config"), F("list"))){}
	bool parse(const xPL_KeyValue& key)
	{
		if (key.is(F("command"), F("request")))
		{
			new ConfigList();
			return true;
		}
		return false;
	}
} _configList;

class ConfigCurrentParser : public xPL_MessageParser
{
public:
	ConfigCurrentParser() :xPL_MessageParser(xPL_MessageHeader(cs_cmnd, F("config"), F("current"))) {}
	bool parse(const xPL_KeyValue& kv)
	{
		if (kv.is(F("command"), F("request")))
		{
			new ConfigCurrent();
			return true;
		}
		return false;
	}
} _configCurrent;

class ConfigResponseParser : public xPL_MessageParser
{
public:
	ConfigResponseParser() :xPL_MessageParser(xPL_MessageHeader(cs_cmnd, F("config"), F("response"))) {}
	bool parse(const xPL_KeyValue& kv)
	{
		bool result = false;
		foreach(Option, opt)
		{
			if (kv.key == opt->name())
			{
				opt->parse(kv.value);
				result = true;
			}
		}
		return result;
	}
} _configResponse;

#endif