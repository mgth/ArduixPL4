#include <SPI.h>
#include <Ethernet.h>
#include "PubSubClient.h"

#include <arduha.h>
#include <wire.h>

#include "ha_DHT.h"

byte mac[] = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
byte server[] = { 172, 16, 0, 2 };
byte ip[] = { 172, 16, 0, 100 };


HA_DHT<DHT11> sensorUp(6);
HA_DHT<DHT11> sensorDn(7);

RecurrentTask t1(sensorUp, 5000);
RecurrentTask t2(sensorDn, 5000);
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

	t1.trigTask(2000);
	t2.trigTask(2000);

	client.connect("Cave");

	sensorUp.temperature.link(new MQTT("TempUp"));
	sensorUp.humidity.link(new MQTT("HumUp"));
	sensorDn.temperature.link(new MQTT("TempDn"));
	sensorDn.humidity.link(new MQTT("HumDn"));

}

void loop()
{
	Task::loop();
	client.loop();
}
