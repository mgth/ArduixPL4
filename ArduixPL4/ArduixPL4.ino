#include <SPI.h>
#include "xpl.h"

//#include <Ether net.h>

#include <xpl_ENC28J60.h>
#include <utility/config.h>
#include <utility/devices.h>

ENC28J60Adapter adapter;

class : public Device
{
	String _current;
	StringRom type() const { return S(temp); }
public:
	String current() const { return _current; }
	void setCurrent(const String & current,const String & data1) { _current = current; }
} dummy;

_SETUP()
{

	Tasks.add(ConfigTask::Task);
//	Tasks.add(&Devices::Task);


	xPL.begin(adapter);

	Devices.add(dummy);
	Devices.begin();
	DBG_MEM(S(loop));

}

_LOOP()
{
	xPL.loop();
}
_END
