#include <ArduHA.h>
#include "ha_DS18x20.h"

#include <xpl.h>
#include <xpl_ENC28J60.h>
#include "utility/xpl_hbeat.h"
#include "utility/xpl_sensor.h"

xPL_ENC28J60Adapter adapter;
HA_DS18x20_GlobalTask tempTask(60000);

_SETUP()
{
	HA_DS18x20_Multi::discover(12);

	int i = 0;
	foreach(HA_DS18x20_Multi, s)
	{
		String name = "T" + String(i++);
		//s->addFilter(new KalmanFilter<int,100>(1));
		//s->addFilter(new Calibration2ndOrder(-0.000132642563198119, 1.00520159709975, 0.144975994513031));
		//s->addFilter(new Calibration2ndOrder<int,128>(0, 129, 19));
		s->addFilter(new ThresholdFilter<int>(12, 5 * 60000));
		s->addFilter(new xPL_Sensor<int,128>(name, F("temp"), F("c") ));
//		if (i == 0) s->addFilter(new OutputFilterSerial);
//		i++;
	}

	DBG_MEM(F("loop"));
	//int a;
	//a = xPL_MessageParser::selectAll();
	//DBGLN("all:",a);
	//a = xPL_MessageParser::selectAll(MSGTYPE, "cmnd");
	//DBGLN("cmnd:",a);
	//a = xPL_MessageParser::selectAll(SCHCLASS, "config");
	//DBGLN("config:",a);
	//a = xPL_MessageParser::selectAll(SCHTYPE, "list");
	//DBGLN("current:",a);
	//a =xPL_MessageParser::parseSelected(xPL_KeyValue("command", "request"));
	//DBGLN("parse:",a);
	//a =	xPL_MessageParser::processSelected();
	//DBGLN("process:",a);
	DBGLN("tasks:", Task::count());
}

_LOOP()
{
//	DBG('.'); delay(100);
	Task::loop();
}
_END
