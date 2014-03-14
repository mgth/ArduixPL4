#include <xpl.h>

#include <xpl_ENC28J60.h>
#include "DallasTemperature.h"
#include "utility/hbeat.h"
ENC28J60Adapter adapter;
DallasTemperatureGlobalTask tempTask(60000);

_SETUP()
{
	DallasTemperatureMulti::discover(12);

	int i = 0;
	foreach(DallasTemperatureMulti, s)
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
	//a = MessageParser::selectAll();
	//DBGLN("all:",a);
	//a = MessageParser::selectAll(MSGTYPE, "cmnd");
	//DBGLN("cmnd:",a);
	//a = MessageParser::selectAll(SCHCLASS, "config");
	//DBGLN("config:",a);
	//a = MessageParser::selectAll(SCHTYPE, "list");
	//DBGLN("current:",a);
	//a =MessageParser::parseSelected(KeyValue("command", "request"));
	//DBGLN("parse:",a);
	//a =	MessageParser::processSelected();
	//DBGLN("process:",a);
	DBGLN("tasks:", Task::count());
}

_LOOP()
{
//	DBG('.'); delay(100);
	Task::loop();
}
_END
