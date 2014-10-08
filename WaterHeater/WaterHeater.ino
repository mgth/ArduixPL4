#include "ArduHA.h"
#include "ha_DS18x20.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Encoder.h>


OneWire oneWire(7);
HA_DS18x20_Wire sensors(oneWire);
Adafruit_SSD1306 lcd(-1); 

size_t printTempTo(Print& p, StringRom s, temperature_t value)
{

	lcd.print(s);

#ifdef HA_FLOAT
	lcd.print(value, 2);
#else
	lcd.print(value / 100);
	lcd.print(".");
	lcd.print((value /10 ) % 10 );
	lcd.print(value % 10);
#endif
	lcd.print("\x0F7""C");
}



class Heater :public Filter<temperature_t>
{
	temperature_t _setpoint;
	temperature_t _temperature;

	float _duration;
	float _v40;

	byte _pin;
	bool _heating;

	const temperature_t _coldWaterTemp = 15.0;// °C
	const temperature_t _ambiantTemp = 20.0;  // °C
	const temperature_t _mixWaterTemp = 40.0; // °C

	const float _volume = 200.0; // Litres

	const float _power = 3000.0; // Watts
	const float _mixWaterFlow = 12.0 / 60; // L/min -> L/s

	const float _qpr = 1.7;

	void setHeating(bool value)
	{
		_heating = value;
		digitalWrite(_pin, _heating);
		display();
	}

	static inline temperature_t K(temperature_t T) { return T + 273.15; }
	static inline temperature_t C(temperature_t T) { return T - 273.15; }

	void calc()
	{
		temperature_t hotLayerTemp = _temperature;

		/**/
		temperature_t coldLayerTemp = _temperature;
		float hotLayerVolume = _volume;
		float coldLayerVolume = 0;

		_duration = 0;
		_v40 = 0;

		float sample = 1; // sample duration in seconds

		while (hotWaterTemp > _mixWaterTemp)
		{
			// hot volume to get one sample mix
			float hotWaterVolume = (_mixWaterFlow *sample)* (_mixWaterTemp - _coldWaterTemp) / (hotWaterTemp - _coldWaterTemp);

			//temp resulting from replacing one sample hot with one sample cold
			hotWaterTemp = C(((_volume - hotWaterVolume) * K(hotWaterTemp) + hotWaterVolume * K(_coldWaterTemp)) / _volume);

			// heating
			hotWaterTemp += (_power * sample) / (_volume *4185.5);

			// heat loss
			hotWaterTemp -= ((_temperature - _ambiantTemp)*((_qpr * 1000 / 24) / (65 - 20)) * sample) / (_volume *4185.5);

			_duration+=sample;
			_v40 += _mixWaterFlow *sample;
		}
	}

	void display()
	{
		lcd.clearDisplay();
		lcd.setTextSize(2);
		lcd.setTextColor(WHITE);
		lcd.setCursor(0, 0);
		// go home
		printTempTo(lcd, F("S:"), _setpoint);
		lcd.println();
		printTempTo(lcd, F("T:"), _temperature);
		lcd.println();
		calc();
		lcd.setTextSize(1);
		lcd.print("V40:"); lcd.println(_v40);
		lcd.print("tps:"); lcd.print((int)_duration / 60); lcd.print(':'); if ((int)_duration % 60 < 10) lcd.print('0');  lcd.println((int)_duration % 60);
		lcd.setTextSize(2);
		if (_heating) lcd.println("H"); else lcd.println("_");
		lcd.display();

	}

public:
	virtual void input(temperature_t value)
	{
		_temperature = value*2.2;
		temperature_t delta = _setpoint - _temperature;

		setHeating(delta > 0);

	};

	// in case of error on temperature reading heater should stop for security 
	virtual void error() { setHeating(false); }

public:
	void setSetpoint(temperature_t setpoint) { _setpoint = setpoint; }
	temperature_t setpoint() { return _setpoint; }
	Heater(byte pin) :_pin(pin) {}

} heater(8);

Encoder knob(5, 6);

void setup()
{
	Serial.begin(9600);

	lcd.begin(SSD1306_SWITCHCAPVCC, 0x3C);

	sensors.discover();
	foreach(HA_DS18x20, s)
	{
		s->temperature
		//	.link(new Calibration2ndOrder_Shifted(-7539, 192, 19))->out
			.link(&heater)
			;
		s->startConversion(1000);
	}


	lcd.clearDisplay();
	lcd.setTextSize(2);
	lcd.setTextColor(WHITE);
	lcd.setCursor(0, 0);
	lcd.print("count:");
	lcd.print(HA_DS18x20::AutoList<HA_DS18x20>::count());
	lcd.display();

	heater.setSetpoint((temperature_t)65);

	//heater.input(2000);
}

void loop()
{
	Task::loop(false);
	//int i = knob.read();
	//if (i) heater.setSetpoint(heater.setpoint() + i);
}

