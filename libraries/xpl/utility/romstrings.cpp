#include "romstrings.h"
/*
#define C(string)      const char s_##string[] PROGMEM = #string;
#define CF(var,string) const char s_##var[]    PROGMEM = string;

#include "../romstrings.lst"

#define C(string) case cs_##string: return (StringRom)s_##string;
#define CF(var,string) case cs_##var: return (StringRom)s_##var;
*/
StringRom ConstString(ConstString_t cs)
{
	switch (cs) {
		case cs_cmnd: return F("cmnd");
		case cs_stat: return F("stat");
		case cs_trig: return F("trig");
		case cs_reconf: return F("reconf");
		case cs_option: return F("option");
		case cs_device: return F("device");
		case cs_type: return F("type");
	}
}
