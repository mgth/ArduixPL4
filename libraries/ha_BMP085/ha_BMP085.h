/*

#ifndef HA_BMP085_h
#define HA_BMP085_h

#include <ArduHA.h>
#include "utility/task.h"
#include "utility/sensor.h"
#include "Wire.h"
//#include "utility/BMP085.h"
int _pin;
	uint8_t read8(uint8_t addr);
	uint16_t read16(uint8_t addr);
	void write8(uint8_t addr, uint8_t data);

	uint16_t readRawTemperature();

#endif
