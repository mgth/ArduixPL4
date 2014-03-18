/*
// Derived from DHT Temperature & Humidity Sensor library for Arduino by Rob Tillaart
// http://arduino.cc/playground/Main/DHTLib

#ifndef XPL_DHT_h
#define XPL_DHT_h

#include <ArduHA.h>
#include "utility/task.h"
#include "utility/sensor.h"

#define DHTLIB_OK				0
#define DHTLIB_ERROR_CHECKSUM	-1
#define DHTLIB_ERROR_TIMEOUT	-2
#define DHTLIB_INVALID_VALUE	-999

	int _pin;
	int read(int& t, int& h);
	int readWhile(bool state);
#endif
