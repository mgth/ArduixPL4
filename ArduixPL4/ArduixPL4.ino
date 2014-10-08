#include <ArduHA.h>
#include "ha_DS18x20.h"
#include "ha_DHT.h"
#include "ha_BMP085.h"
#include "ha_BH1750.h"

#include <xpl.h>
#include <xpl_ENC28J60.h>
#include <utility/xpl_Serial.h>
#include "utility/xpl_hbeat.h"
#include "utility/xpl_sensor.h"

xPL_ENC28J60Adapter adapter;
xPL_Serial debugAdapter;

HA_DHTxx dht22(0,999,6,22);
HA_BMP085 bmp085(0,998);
HA_BH1750 bh1750(0, 997,0x23,2,false);

OneWire wire(7);

_SETUP()
{
	HA_DS18x20::discover(wire,12);

	int i = 0;
	foreach(HA_DS18x20, s)
	{
		String name = "T" + String(i++);
		
		s->temperature
			.link(new Calibration2ndOrder_Shifted(-7539, 192, 19))->out
			.link(new ThresholdFilter<int>(12, 5 * 60000))->out
		//	.link(new KalmanFilter<int,100>(1))->out
			.link(new xPL_Sensor<int, 128>(name, F("temp"), F("c")))
			;
	}
	//HA_DS18x20::first()->startGlobalConversion(5000);

	dht22.temperature
		.link(new ThresholdFilter<int>(24, 5 * 60000))->out
		.link(new xPL_Sensor<int, 128>(F("dht_t"), F("temp"), F("c")))
		;

	dht22.humidity
		.link(new ThresholdFilter<int>(32, 5 * 60000))->out
		.link(new xPL_Sensor<int, 128>(F("dht_h"), F("humidity"), F("%")))
		;

	bmp085.temperature
		.link(new ThresholdFilter<int>(64, 5 * 60000))->out
		.link(new xPL_Sensor<int, 128>(F("bmp_t"), F("temp"), F("c")))
		;

	bmp085.pressure
		.link(new ThresholdFilter<long>(100, 5 * 60000))->out
		.link(new xPL_Sensor<long, 100>(F("bmp_p"), F("pressure"), F("hPa")))
		;

	bh1750.out
		.link(new ThresholdFilter<int>(1, 5 * 60000))->out
		.link(new xPL_Sensor<int, 2>(F("light"), F("illuminance"), F("lx")))
		;

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
	Task::loop(false);
}
_END
