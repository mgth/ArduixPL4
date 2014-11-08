#include <SPI.h>
#include <Ethernet.h>
#include "PubSubClient.h"

#include <ArduHA.h>
//#include <wire.h>

#include "ha_DHT.h"
#include "ha_RF24L01.h"
#include "mqttsn.h"

byte mac[] = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
byte server[] = { 172, 16, 0, 2 };
byte ip[] = { 172, 16, 0, 100 };


HA_DHT<DHT11> sensorUp(6);
HA_DHT<DHT11> sensorDn(7);

HA_RF24L01 rf24(7,8);

void callb(char* topic, byte* payload, unsigned int length);
EthernetClient ethClient;
PubSubClient client(server, 1883, callb, ethClient);
void callb(char* topic, byte* payload, unsigned int length) {
	byte* p = (byte*)malloc(length);
	memcpy(p, payload, length);
	client.publish("outTopic", p, length);
	// Free the memory
	free(p);
}

class MQTT_SN : Filter<double>
{

};

class MQTT : Filter<double>
{
	String _topic;

	void input(double temp)
	{
		client.publish(_topic.c_str(), String(temp).c_str());
	}

public:
	MQTT(const String& topic) :_topic(topic){}
};


void setup()
{
	client.connect("Cave");

	sensorUp.temperature.link(new MQTT("TempUp"));
	sensorUp.humidity.link(new MQTT("HumUp"));
	sensorDn.temperature.link(new MQTT("TempDn"));
	sensorDn.humidity.link(new MQTT("HumDn"));

	sensorUp.trigReccurent(2000, 5000);
	sensorDn.trigReccurent(2000, 5000);
}

void loop()
{
	Task::loop();
	client.loop();
}
