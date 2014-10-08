#include "ha_spi.h"

HA_Spi* HA_Spi::_spi = NULL;
bool HA_Spi::ready = true;

bool HA_Spi::chipSelect() {
	if (!ready) return false;
	if (!_spi)
	{
		digitalWrite(SS, HIGH);
		pinMode(SS, OUTPUT);
		SPCR = _BV(SPE) | _BV(MSTR); // 8 MHz @ 16

		pinMode(SCK, OUTPUT);
		pinMode(MOSI, OUTPUT);
	}
	if (_spi != this)
	{		
		if (!_spi || (config.bitOrder != _spi->config.bitOrder))
		{
			if (config.bitOrder) bitClear(SPCR, DORD);
			else bitSet(SPCR, DORD);
		}
		
		if (!_spi || (config.dataMode != _spi->config.dataMode))
			SPCR = (SPCR & ~SPI_MODE_MASK) | (config.dataMode << 2);
		
		if (!_spi || (config.clockDivider != _spi->config.clockDivider))
		{
			SPCR = (SPCR & ~SPI_CLOCK_MASK) | (config.clockDivider & SPI_CLOCK_MASK);
			SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | ((config.clockDivider >> 2) & SPI_2XCLOCK_MASK);
		}
		_spi = this;
	}

	if (config.cli) cli();

	digitalWrite(config.cs, LOW);
	ready = false;
	return true;
}

bool HA_Spi::chipSelect(byte data)
{
	if (chipSelect())
	{
		return transfer(data);
	}
}

void HA_Spi::chipUnselect() {
	if (_spi == this)
	{
		digitalWrite(config.cs, HIGH);
		ready = true;
		if (config.cli) sei();
	}
}

int HA_Spi::transfer(byte data) {
	if (ready || _spi != this) return -1;

	SPDR = data;
	while (!(SPSR&(1 << SPIF)));
	return SPDR;
}

void HA_Spi::readBuffer(void* buffer, int len) {
	byte* buf = (byte*)buffer;

	while (len--) {
		byte r = read();
		if (buf) *buf++ = r;
	}
}

void HA_Spi::writeBuffer(const void* buffer, int len)
{
	byte* buf = (byte*)buffer;
	while (len--) { write(*buf++); }
}

word HA_Spi::readWordLE() {
	word w = (word)read();
	w &= (word)read() << 8;
	return w;
}

word HA_Spi::readWordBE() {
	word w;
	readBuffer((byte*)&w, sizeof(w));
	return w;
}

byte HA_Spi::readRegister(byte reg, void* buffer, byte len)
{
	byte status = chipSelect(reg);
	readBuffer(buffer, len);
	chipUnselect();
	return status;
}

byte HA_Spi::writeRegister(byte reg, const void* buffer, int len)
{
	byte status = chipSelect(reg);
	writeBuffer(buffer, len);
	chipUnselect();
	return status;
}
