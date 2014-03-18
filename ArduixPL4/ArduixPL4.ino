#include "Wire.h"

#include <ArduHA.h>
#include "ha_DS18x20.h"
#include "ha_DHT.h"
#include "ha_BMP085.h"

#include <xpl.h>
#include <xpl_ENC28J60.h>
#include <utility/xpl_Serial.h>
#include "utility/xpl_hbeat.h"
#include "utility/xpl_sensor.h"

xPL_ENC28J60Adapter adapter;
xPL_Serial debugAdapter;

HA_DS18x20_GlobalTask tempTask(60000);
HA_SensorDHT dht(6,22);
HA_BMP085 bmp;


_SETUP()
{

	HA_DS18x20_Multi::discover(12);

	int i = 0;
	foreach(HA_DS18x20_Multi, s)
	{
		String name = "T" + String(i++);
		
		s->temperature.link(new Calibration2ndOrder_Shifted(-7539, 192, 19))
			->out.link(new ThresholdFilter<int>(12, 5 * 60000))
		//	->out.link(new KalmanFilter<int,100>(1))
			->out.link(new xPL_Sensor<int, 128>(name, F("temp"), F("c")));
	}

	dht.temperature.link(new ThresholdFilter<int>(24, 5 * 60000))
		->out.link(new xPL_Sensor<int, 128>(F("dht_t"), F("temp"), F("c")));
	dht.humidity.link(new ThresholdFilter<int>(32, 5 * 60000))
		->out.link(new xPL_Sensor<int, 128>(F("dht_h"), F("humidity"), F("%")));

	bmp.temperature.link(new ThresholdFilter<int>(64, 5 * 60000))
		->out.link(new xPL_Sensor<int, 128>(F("bmp_t"), F("temp"), F("c")));
	bmp.pressure.link(new ThresholdFilter<long>(100, 5 * 60000))
		->out.link(new xPL_Sensor<long, 100>(F("bmp_p"), F("pressure"), F("hPa")));


//	xPL_Sensor(bmp.pressure, F("bmp_t"), F("temp"), F("c"))

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