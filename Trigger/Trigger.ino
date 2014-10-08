#include "digitalWriteFast.h"


#define DELAIS_PIN  2// point millieu du potar de delais
#define THRESHOLD_PIN 3 // point millieu du potard threshold

#define SHUTTER_PIN 1 // Pin vers Moc3020 => flash/dslr
#define LED_SHUTTER_PIN 0 // une led visualise le declenchement ( sur un pins different )
#define LIGHTNING_TRIGGER_ANALOG_PIN 1 // entree capteur

#define DELAI_OFFSET 100

//#define BENCH

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

int lightningVal;
int oldLightningVal;

int valDelai = 0;
int triggerThreshold = 1024;

inline void readThreshold() {
	triggerThreshold = analogRead(THRESHOLD_PIN);//50; // lecture du potar 10K /50 pour limiter fluctuations et portée
}
inline void readDelai() {
	valDelai = analogRead(DELAIS_PIN) - DELAI_OFFSET; // lecture du potar 10K
}

void setup()
{
	sbi(ADCSRA, ADPS2);
	cbi(ADCSRA, ADPS1);
	cbi(ADCSRA, ADPS0);

	pinMode(SHUTTER_PIN, OUTPUT);
	pinMode(LED_SHUTTER_PIN, OUTPUT);
	digitalWrite(SHUTTER_PIN, LOW);

#ifdef BENCH
	Serial.begin(9600); // open serial
#endif

	lightningVal = analogRead(LIGHTNING_TRIGGER_ANALOG_PIN);

	readDelai();
	readThreshold();
}

int i = 0;

void loop()
{
#ifdef BENCH
	long start = micros();
	for (i = 0; i < 20000; i++)
#else
	i++;
#endif
	{
		oldLightningVal = lightningVal;
		lightningVal = analogRead(LIGHTNING_TRIGGER_ANALOG_PIN);

		if (abs(lightningVal - oldLightningVal) > triggerThreshold)
		{
			if (valDelai>0) delay(valDelai);
			digitalWriteFast(SHUTTER_PIN, HIGH);  // 5us
			digitalWriteFast(LED_SHUTTER_PIN, HIGH);
			delay(50); // May want to adjust this depending on shot type
			digitalWriteFast(SHUTTER_PIN, LOW);
			digitalWriteFast(LED_SHUTTER_PIN, LOW);

			lightningVal = analogRead(LIGHTNING_TRIGGER_ANALOG_PIN);
		}

		if (i & 1) readDelai();
		else readThreshold();
	}

#ifdef BENCH
	Serial.println(((float)micros() - start) / i);
#endif
}
