/*
// Derived from DHT Temperature & Humidity Sensor library for Arduino by Rob Tillaart
// http://arduino.cc/playground/Main/DHTLib

// reference: http://wahiduddin.net/calc/density_algorithms.htm 
/*
double dewPoint(double celsius, double humidity)
{
        double RATIO = 373.15 / (273.15 + celsius);  // RATIO was originally named A0, possibly confusing in Arduino context
        double SUM = -7.90298 * (RATIO - 1);
        SUM += 5.02808 * log10(RATIO);
        SUM += -1.3816e-7 * (pow(10, (11.344 * (1 - 1/RATIO ))) - 1) ;
        SUM += 8.1328e-3 * (pow(10, (-3.49149 * (RATIO - 1))) - 1) ;
        SUM += log10(1013.246);
        double VP = pow(10, SUM - 3) * humidity;
        double T = log(VP/0.61078);   // temp var
        return (241.88 * T) / (17.558 - T);
}
*/
int HA_SensorDHT::read(int& temperature, int& humidity)
{
	byte bits[5];

	// READ VALUES
	int rv = read(bits);
	if (rv != DHTLIB_OK)
	{
		humidity = DHTLIB_INVALID_VALUE; // invalid value, or is NaN prefered?
		temperature = DHTLIB_INVALID_VALUE; // invalid value
		return rv;
	}

	// CONVERT AND STORE
	long h = word(bits[0], bits[1]) << 7;  // bits[1] == 0;
	long t = word(bits[2] & 0x7F, bits[3]) << 7;  // bits[3] == 0;

	if (_type > 11)
	{
		h /= 10;
		t /= 10;
		if (bits[2] & 0x80) t = -t;
	}
	humidity = h; // invalid value, or is NaN prefered?
	temperature = t; // invalid value
	// TEST CHECKSUM
	uint8_t sum = bits[0] + bits[1] + bits[2] + bits[3];
	if (bits[4] != sum) return DHTLIB_ERROR_CHECKSUM;

	return DHTLIB_OK;
}

			//								4892.32623 *
			//								( 1/(273.15 + _tempNorm) - 1/(273.15 + _dht.temperature) )
			//							);
			//_humidityNorm.input(hn);
// uint16_t for UNO, higher CPU speeds => exceed MAXINT.
// works for DUE
// 16 MHz => 10000
// 84 MHz => 52500
// 100MHz => 62500
#define TIMEOUT (F_CPU/1600)

{

	// INIT BUFFERVAR TO RECEIVE DATA
	uint8_t mask = 128;
	uint8_t idx = 0;

	// EMPTY BUFFER
	memset(bits, 0, 5);

	// REQUEST SAMPLE
	pinMode(_pin, OUTPUT);
	digitalWrite(_pin, LOW);
	_delay_ms(20);
	digitalWrite(_pin, HIGH);
	_delay_us(40);
	pinMode(_pin, INPUT);

	// GET ACKNOWLEDGE or TIMEOUT
	if (readWhile(LOW) == DHTLIB_ERROR_TIMEOUT) return DHTLIB_ERROR_TIMEOUT;

	if (readWhile(HIGH) == DHTLIB_ERROR_TIMEOUT) return DHTLIB_ERROR_TIMEOUT;

	// READ THE OUTPUT - 40 BITS => 5 BYTES
	while (idx<5)
	{
		if (readWhile(LOW) == DHTLIB_ERROR_TIMEOUT) return DHTLIB_ERROR_TIMEOUT;

		unsigned long t = micros();

		if (readWhile(HIGH) == DHTLIB_ERROR_TIMEOUT) return DHTLIB_ERROR_TIMEOUT;

		if ((micros() - t) > 40) bits[idx] |= mask;
		mask >>= 1;
		if (!mask)   // next byte?
		{
			mask = 128;
			idx++;
		}
	}

	return DHTLIB_OK;
}