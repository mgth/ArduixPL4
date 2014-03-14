/*
  ArduHA - ArduixPL - xPL library for Arduino(tm)
  Copyright (c) 2012/2014 Mathieu GRENET.  All right reserved.

  This file is part of ArduHA / ArduixPL.

    ArduixPL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ArduixPL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ArduixPL.  If not, see <http://www.gnu.org/licenses/>.

	  Modified 2014-3-14 by Mathieu GRENET 
	  mailto:mathieu@mgth.fr
	  http://www.mgth.fr
*/

#include "xPL_ENC28J60.h"

//ERXFCON 0b01110000
//EMPOH:EPMOL = 0
//24h=0F:19
//2Ah=78:70:2B:6C
//32h=0A:7B:0A

//00:00:00:00:0C:3B:38
//<<388F

 /*******************************************************************

 *******************************************************************/
static int gNextPacketPtr = -1;
static byte Enc28j60Bank;

OptionMacAddress _mac;

int _writePos = -1;
int _available = -1;
long _checksum;
long _headerSum;
long _udpSum;
// ENC28J60 Control Registers
// Control register definitions are a combination of address,
// bank number, and Ethernet/MAC/PHY indicator bits.
// - Register address        (bits 0-4)
// - Bank number        (bits 5-6)
// - MAC/PHY indicator        (bit 7)
#define ADDR_MASK        0x1F
#define BANK_MASK        0x60
#define SPRD_MASK        0x80
// All-bank registers
#define EIE              0x1B
#define EIR              0x1C
#define ESTAT            0x1D
#define ECON2            0x1E
#define ECON1            0x1F
// Bank 0 registers
#define ERDPT           (0x00|0x00)
#define EWRPT           (0x02|0x00)
#define ETXST           (0x04|0x00)
#define ETXND           (0x06|0x00)
#define ERXST           (0x08|0x00)
#define ERXND           (0x0A|0x00)
#define ERXRDPT         (0x0C|0x00)
// #define ERXWRPT         (0x0E|0x00)
#define EDMAST          (0x10|0x00)
#define EDMAND          (0x12|0x00)
// #define EDMADST         (0x14|0x00)
#define EDMACS          (0x16|0x00)
// Bank 1 registers
#define EHT0             (0x00|0x20)
#define EHT1             (0x01|0x20)
#define EHT2             (0x02|0x20)
#define EHT3             (0x03|0x20)
#define EHT4             (0x04|0x20)
#define EHT5             (0x05|0x20)
#define EHT6             (0x06|0x20)
#define EHT7             (0x07|0x20)
#define EPMM0            (0x08|0x20)
#define EPMM1            (0x09|0x20)
#define EPMM2            (0x0A|0x20)
#define EPMM3            (0x0B|0x20)
#define EPMM4            (0x0C|0x20)
#define EPMM5            (0x0D|0x20)
#define EPMM6            (0x0E|0x20)
#define EPMM7            (0x0F|0x20)
#define EPMCS           (0x10|0x20)
#define EPMO            (0x14|0x20)
#define EWOLIE           (0x16|0x20)
#define EWOLIR           (0x17|0x20)
#define ERXFCON          (0x18|0x20)
#define EPKTCNT          (0x19|0x20)
// Bank 2 registers
#define MACON1           (0x00|0x40|0x80)
#define MACON2           (0x01|0x40|0x80)
#define MACON3           (0x02|0x40|0x80)
#define MACON4           (0x03|0x40|0x80)
#define MABBIPG          (0x04|0x40|0x80)
#define MAIPG           (0x06|0x40|0x80)
#define MACLCON1         (0x08|0x40|0x80)
#define MACLCON2         (0x09|0x40|0x80)
#define MAMXFL          (0x0A|0x40|0x80)
#define MAPHSUP          (0x0D|0x40|0x80)
#define MICON            (0x11|0x40|0x80)
#define MICMD            (0x12|0x40|0x80)
#define MIREGADR         (0x14|0x40|0x80)
#define MIWR            (0x16|0x40|0x80)
#define MIRD            (0x18|0x40|0x80)
// Bank 3 registers
#define MAADR1           (0x00|0x60|0x80)
#define MAADR0           (0x01|0x60|0x80)
#define MAADR3           (0x02|0x60|0x80)
#define MAADR2           (0x03|0x60|0x80)
#define MAADR5           (0x04|0x60|0x80)
#define MAADR4           (0x05|0x60|0x80)
#define EBSTSD           (0x06|0x60)
#define EBSTCON          (0x07|0x60)
#define EBSTCS          (0x08|0x60)
#define MISTAT           (0x0A|0x60|0x80)
#define EREVID           (0x12|0x60)
#define ECOCON           (0x15|0x60)
#define EFLOCON          (0x17|0x60)
#define EPAUS           (0x18|0x60)

// ENC28J60 ERXFCON Register Bit Definitions
#define ERXFCON_UCEN     0x80
#define ERXFCON_ANDOR    0x40
#define ERXFCON_CRCEN    0x20
#define ERXFCON_PMEN     0x10
#define ERXFCON_MPEN     0x08
#define ERXFCON_HTEN     0x04
#define ERXFCON_MCEN     0x02
#define ERXFCON_BCEN     0x01
// ENC28J60 EIE Register Bit Definitions
#define EIE_INTIE        0x80
#define EIE_PKTIE        0x40
#define EIE_DMAIE        0x20
#define EIE_LINKIE       0x10
#define EIE_TXIE         0x08
#define EIE_WOLIE        0x04
#define EIE_TXERIE       0x02
#define EIE_RXERIE       0x01
// ENC28J60 EIR Register Bit Definitions
#define EIR_PKTIF        0x40
#define EIR_DMAIF        0x20
#define EIR_LINKIF       0x10
#define EIR_TXIF         0x08
#define EIR_WOLIF        0x04
#define EIR_TXERIF       0x02
#define EIR_RXERIF       0x01
// ENC28J60 ESTAT Register Bit Definitions
#define ESTAT_INT        0x80
#define ESTAT_LATECOL    0x10
#define ESTAT_RXBUSY     0x04
#define ESTAT_TXABRT     0x02
#define ESTAT_CLKRDY     0x01
// ENC28J60 ECON2 Register Bit Definitions
#define ECON2_AUTOINC    0x80
#define ECON2_PKTDEC     0x40
#define ECON2_PWRSV      0x20
#define ECON2_VRPS       0x08
// ENC28J60 ECON1 Register Bit Definitions
#define ECON1_TXRST      0x80
#define ECON1_RXRST      0x40
#define ECON1_DMAST      0x20
#define ECON1_CSUMEN     0x10
#define ECON1_TXRTS      0x08
#define ECON1_RXEN       0x04
#define ECON1_BSEL1      0x02
#define ECON1_BSEL0      0x01
// ENC28J60 MACON1 Register Bit Definitions
#define MACON1_LOOPBK    0x10
#define MACON1_TXPAUS    0x08
#define MACON1_RXPAUS    0x04
#define MACON1_PASSALL   0x02
#define MACON1_MARXEN    0x01
// ENC28J60 MACON2 Register Bit Definitions
#define MACON2_MARST     0x80
#define MACON2_RNDRST    0x40
#define MACON2_MARXRST   0x08
#define MACON2_RFUNRST   0x04
#define MACON2_MATXRST   0x02
#define MACON2_TFUNRST   0x01
// ENC28J60 MACON3 Register Bit Definitions
#define MACON3_PADCFG2   0x80
#define MACON3_PADCFG1   0x40
#define MACON3_PADCFG0   0x20
#define MACON3_TXCRCEN   0x10
#define MACON3_PHDRLEN   0x08
#define MACON3_HFRMLEN   0x04
#define MACON3_FRMLNEN   0x02
#define MACON3_FULDPX    0x01
// ENC28J60 MICMD Register Bit Definitions
#define MICMD_MIISCAN    0x02
#define MICMD_MIIRD      0x01
// ENC28J60 MISTAT Register Bit Definitions
#define MISTAT_NVALID    0x04
#define MISTAT_SCAN      0x02
#define MISTAT_BUSY      0x01

// ENC28J60 EBSTCON Register Bit Definitions
#define EBSTCON_PSV2     0x80
#define EBSTCON_PSV1     0x40
#define EBSTCON_PSV0     0x20
#define EBSTCON_PSEL     0x10
#define EBSTCON_TMSEL1   0x08
#define EBSTCON_TMSEL0   0x04
#define EBSTCON_TME      0x02
#define EBSTCON_BISTST    0x01

// PHY registers
#define PHCON1           0x00
#define PHSTAT1          0x01
#define PHHID1           0x02
#define PHHID2           0x03
#define PHCON2           0x10
#define PHSTAT2          0x11
#define PHIE             0x12
#define PHIR             0x13
#define PHLCON           0x14

// ENC28J60 PHY PHCON1 Register Bit Definitions
#define PHCON1_PRST      0x8000
#define PHCON1_PLOOPBK   0x4000
#define PHCON1_PPWRSV    0x0800
#define PHCON1_PDPXMD    0x0100
// ENC28J60 PHY PHSTAT1 Register Bit Definitions
#define PHSTAT1_PFDPX    0x1000
#define PHSTAT1_PHDPX    0x0800
#define PHSTAT1_LLSTAT   0x0004
#define PHSTAT1_JBSTAT   0x0002
// ENC28J60 PHY PHCON2 Register Bit Definitions
#define PHCON2_FRCLINK   0x4000
#define PHCON2_TXDIS     0x2000
#define PHCON2_JABBER    0x0400
#define PHCON2_HDLDIS    0x0100

// ENC28J60 Packet Control Byte Bit Definitions
#define PKTCTRL_PHUGEEN  0x08
#define PKTCTRL_PPADEN   0x04
#define PKTCTRL_PCRCEN   0x02
#define PKTCTRL_POVERRIDE 0x01

// SPI operation codes
#define ENC28J60_READ_CTRL_REG       0x00
#define ENC28J60_READ_BUF_MEM        0x3A
#define ENC28J60_WRITE_CTRL_REG      0x40
#define ENC28J60_WRITE_BUF_MEM       0x7A
#define ENC28J60_BIT_FIELD_SET       0x80
#define ENC28J60_BIT_FIELD_CLR       0xA0
#define ENC28J60_SOFT_RESET          0xFF

// The RXSTART_INIT must be zero. See Rev. B4 Silicon Errata point 5.
// Buffer boundaries applied to internal 8K ram
// the entire available packet buffer space is allocated

#define RXSTART_INIT        0x0000  // start of RX buffer, room for 2 packets
#define RXSTOP_INIT         0x0BFF  // end of RX buffer
                            
#define TXSTART_INIT        0x0C00  // start of TX buffer, room for 1 packet
#define TXSTOP_INIT         0x11FF  // end of TX buffer
                            
#define SCRATCH_START       0x1200  // start of scratch area
#define SCRATCH_LIMIT       0x2000  // past end of area, i.e. 3.5 Kb 
#define SCRATCH_PAGE_SHIFT  6       // addressing is in pages of 64 bytes
#define SCRATCH_PAGE_SIZE   (1 << SCRATCH_PAGE_SHIFT)

// max frame length which the conroller will accept:
// (note: maximum ethernet frame length would be 1518)
#define MAX_FRAMELEN      1500        

#define FULL_SPEED  1   // switch to full-speed SPI for bulk transfers

static void xferSPI (byte data) {
    SPDR = data;
    while (!(SPSR&(1<<SPIF)))
        ;
}

void enableChip(byte data) {
    cli();
    digitalWrite(ENC28J60_PIN, LOW);
	xferSPI(data);
}

static void disableChip () {
    digitalWrite(ENC28J60_PIN, HIGH);
    sei();
}


// Opérations

static byte readOp (byte op, byte address) {
    enableChip(op | (address & ADDR_MASK));
    xferSPI(0x00);
    if (address & 0x80)
        xferSPI(0x00);
    byte result = SPDR;
    disableChip();
    return result;
}

static void writeOp(byte op, byte address, byte data) {
	enableChip(op | (address & ADDR_MASK));
	xferSPI(data);
	disableChip();
}

// Read buffer

void startReadBuf() {
	enableChip(ENC28J60_READ_BUF_MEM);
}

byte readByte() {
	_available--;
	xferSPI(0x00);
	return SPDR;
}

static void readBuf(word len, byte* data) {
	while (len--) {
		*data++ = readByte();
    }
}


static word readWordLE() {
	word w = (word)readByte();
	w &= (word)readByte() << 8;
	return w;
}

static word readWordBE() {
	word w;
	readBuf(sizeof(w),(byte*)&w);
	return w;
}

static void drop(word len) {
	while (len--) {
		readByte();
    }
}

// Write Buffer


void startWriteBuf()
{
    enableChip(ENC28J60_WRITE_BUF_MEM);
}

static void writeBuf(word len, const byte* data) {
	startWriteBuf();
	while (len--)
        xferSPI(*data++);
    disableChip();
}

static void SetBank (byte address) {
    if ((address & BANK_MASK) != Enc28j60Bank) {
        writeOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_BSEL1|ECON1_BSEL0);
        Enc28j60Bank = address & BANK_MASK;
        writeOp(ENC28J60_BIT_FIELD_SET, ECON1, Enc28j60Bank>>5);
    }
}

static byte readRegByte (byte address) {
    SetBank(address);
    return readOp(ENC28J60_READ_CTRL_REG, address);
}

static word readReg(byte address) {
	return readRegByte(address) + (readRegByte(address+1) << 8);
}

static void writeRegByte (byte address, byte data) {
    SetBank(address);
    writeOp(ENC28J60_WRITE_CTRL_REG, address, data);
}

static void writeReg(byte address, word data) {
    writeRegByte(address, data);
    writeRegByte(address + 1, data >> 8);
}

static word readPhyByte (byte address) {
    writeRegByte(MIREGADR, address);
    writeRegByte(MICMD, MICMD_MIIRD);
    while (readRegByte(MISTAT) & MISTAT_BUSY)
        ;
    writeRegByte(MICMD, 0x00);
    return readRegByte(MIRD+1);
}

static void writePhy (byte address, word data) {
    writeRegByte(MIREGADR, address);
    writeReg(MIWR, data);
    while (readRegByte(MISTAT) & MISTAT_BUSY)
        ;
}

void initSPI() {
	pinMode(SS, OUTPUT);
	digitalWrite(SS, HIGH);
	pinMode(MOSI, OUTPUT);
	pinMode(SCK, OUTPUT);
	pinMode(MISO, INPUT);

	digitalWrite(MOSI, HIGH);
	digitalWrite(MOSI, LOW);
	digitalWrite(SCK, LOW);

	SPCR = bit(SPE) | bit(MSTR); // 8 MHz @ 16
	bitSet(SPSR, SPI2X);
}

void filterXpl() {
	writeRegByte(ERXFCON, ERXFCON_ANDOR | ERXFCON_CRCEN | ERXFCON_PMEN);
	//	writeReg(EPMO, 0x0000);
	writeReg(EPMM0, 0x3000); // 0C = 08:00 = IP
	writeReg(EPMM2, 0x0080); // 17 = 11    = UDP
	writeReg(EPMM4, 0x3C30); // 24 = 0F:19       = Port xPL 
	// 2A = 78:70:6C:2D = "xpl-"
	writeReg(EPMM6, 0x001C); // 32 = 0A:7B:0A    = "/n{/n"*/

	writeReg(EPMCS, 0xB4F7);
}

void filterXplCmnd() {
	writeRegByte(ERXFCON, ERXFCON_ANDOR | ERXFCON_CRCEN | ERXFCON_PMEN);
	//	writeReg(EPMO, 0x0000);
	writeReg(EPMM0, 0x3000); // 0C = 08:00 = IP
	writeReg(EPMM2, 0x0080); // 17 = 11    = UDP
	writeReg(EPMM4, 0xFC30); // 24 = 0F:19       = Port xPL 
	// 2A = 78:70:6C:2D:63:6D = "xpl-cm"
	writeReg(EPMM6, 0x001F); // 32 = 6E:64:0A:7B:0A    = "nd/n{/n"*/

	writeReg(EPMCS, 0xE325);
}

void startWrite(int pos=0)
{
	disableChip();
	writeReg(EWRPT, pos + TXSTART_INIT + 1); // TODO : dont know why i need +1
	_writePos = pos;
	enableChip(ENC28J60_WRITE_BUF_MEM);
}

void printByte(byte b)
{
	if (_writePos % 2) _checksum += b;
	else _checksum += ((long)b) << 8;

	xferSPI(b);
	_writePos++;
}

word getChecksum(long sum)
{
	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	return ~sum;
}

void printBytes(byte b, byte count)
{
	for (byte i = 0; i<count; i++) printByte(b);
}

void printWord(word w)
{ 
	printByte((char)(w>>8));
	printByte((char)w);
}

void writeHeader()
{

	startWrite();

// Ethernet
	printBytes('\xFF',6);       // broadcast mac
	for (byte i = 0; i<6; i++) printByte(_mac[i].get());
	printWord(0x800);

// IP
	_checksum = 0;
	printWord(0x4500); // 14
	printBytes(0, 4);
//	printWord(0);      // 16 : IP Lenght
//	printWord(0);      // 18
	printWord(0x4000); // 20
	printWord(0x4011); // 22

	printWord(0); // 24 : ip checksum

#ifdef XPL_IP
	for (byte i = 0; i<4; i++) len += p.print(_ip.bin[i]);
#else
	printBytes(0, 4);
//	printWord(0); // 26
//	printWord(0); //28
#endif
	printBytes('\xFF', 4);
//	for (byte i = 0; i<4; i++) printByte('\xFF');//30    // Ip source and dest set to all ones.

	_headerSum = _checksum;
	
	_checksum = 0x11 + 0xFFFF + 0xFFFF + 2; // TODO : don't know why I have to add 2

// UDP
	printWord(XPL_PORT); // 34 - Source port
	printWord(XPL_PORT); // 36 - Dest port
	printBytes(0, 4);
//	printWord(0);        // 38 : UDP length
//	printWord(0);        // 40 : checksum

	_udpSum = _checksum;

	disableChip();
}

void initialize() {
	_writePos = -1;

	if (bitRead(SPCR, SPE) == 0) initSPI();

	pinMode(ENC28J60_PIN, OUTPUT);
	disableChip();

	writeOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
	delay(2); // errata B7/2
	while (!readOp(ENC28J60_READ_CTRL_REG, ESTAT) & ESTAT_CLKRDY)
		;

	gNextPacketPtr = RXSTART_INIT;
	writeReg(ERXST, RXSTART_INIT);
	writeReg(ERXRDPT, RXSTART_INIT);
	writeReg(ERXND, RXSTOP_INIT);
	writeReg(ETXST, TXSTART_INIT);
	writeReg(ETXND, TXSTOP_INIT);
	//enableBroadcast(); // change to add ERXFCON_BCEN recommended by epam

	filterXpl();


	//    writeReg(EPMM0, 0x303f);
	//    writeReg(EPMCS, 0xf7f9);
	writeRegByte(MACON1, MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);
	writeRegByte(MACON2, 0x00);
	writeOp(ENC28J60_BIT_FIELD_SET, MACON3,
		MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN);
	writeReg(MAIPG, 0x0C12);
	writeRegByte(MABBIPG, 0x12);
	writeReg(MAMXFL, MAX_FRAMELEN);

	writeRegByte(MAADR5, _mac[0].get());
	writeRegByte(MAADR4, _mac[1].get());
	writeRegByte(MAADR3, _mac[2].get());
	writeRegByte(MAADR2, _mac[3].get());
	writeRegByte(MAADR1, _mac[4].get());
	writeRegByte(MAADR0, _mac[5].get());
	writePhy(PHCON2, PHCON2_HDLDIS);
	SetBank(ECON1);
	writeOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE | EIE_PKTIE);
	writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);

	while (readOp(ENC28J60_READ_CTRL_REG, ECON1) & ECON1_TXRTS)
	if (readRegByte(EIR) & EIR_TXERIF) {
		writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRST);
		writeOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRST);
	}
	writeOp(ENC28J60_WRITE_BUF_MEM, 0, 0x00);

	writeHeader();

}

void xPL_ENC28J60Adapter::begin()
{
	static bool init = false;

	if (!init)
	{
		DBG(F("<begin ENC28J60>"));
		initialize();
		init = true;
	}

}

bool xPL_ENC28J60Adapter::start() {
	begin();

	writeReg(ETXND, TXSTOP_INIT);
	_checksum = _udpSum;
	startWrite(42);

	return true;
}

bool xPL_ENC28J60Adapter::send() {
	
	word len = _writePos;
	long udpSum = _checksum;

	startWrite(16);
	printWord(len - 14); // IP lenght

	startWrite(24);
	printWord(getChecksum(_headerSum + len - 14));

	startWrite(38);
	printWord(len - 34);  // UDP lenght
	printWord(getChecksum(udpSum + 2*(len - 34)));

	disableChip();

	writeReg(ETXND, TXSTART_INIT + len);

	writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);

	return true;
}


size_t xPL_ENC28J60Adapter::write(uint8_t val) {
	printByte(val);
	return 1;
}

/////////////////////////////////////////////////////////////////////
// Reading
/////////////////////////////////////////////////////////////////////
void setReadPos(int pos) { writeReg(ERDPT, pos); }

void packetRelease() {
	if (gNextPacketPtr - 1 > RXSTOP_INIT)
		writeReg(ERXRDPT, RXSTOP_INIT);
	else
		writeReg(ERXRDPT, gNextPacketPtr - 1);
	writeOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);

	_available = -1;
}

word packetReceive() {
	word len = 0;
	_available = -1;

	if (readRegByte(EPKTCNT) > 0)
	{
		writeReg(ERDPT, gNextPacketPtr);

		enableChip(ENC28J60_READ_BUF_MEM);

		gNextPacketPtr = readWordBE();
		len = readWordBE() -4; //byteCount remove the CRC count

		if ((readWordBE() & 0x80) == 0) // status
		{
			len = 0;
			packetRelease();
		}
		else
			_available = len;

		disableChip();
	}
	return len;
}

int xPL_ENC28J60Adapter::available()
{
	if (_available < 0)
	{
		begin();
		word len = packetReceive();
		if (len)
		{
			if (_available>0)
			{
				enableChip(ENC28J60_READ_BUF_MEM);
				drop(42);
				disableChip();
			}
			else
			{
				packetRelease();
				return 0;
			}
		}
		else return 0;
	}
	return _available;
}

int xPL_ENC28J60Adapter::read()
{
	if (available())
	{
		enableChip(ENC28J60_READ_BUF_MEM);
		byte b = readByte();
		disableChip();
		return b;
	}
	return -1;
}

int xPL_ENC28J60Adapter::peek() 
{
	if (available())
	{
		writeOp(ENC28J60_BIT_FIELD_CLR, ECON2, ECON2_AUTOINC);
		enableChip(ENC28J60_READ_BUF_MEM);
		xferSPI(0x00);
		byte b = SPDR;
		disableChip();
		writeOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_AUTOINC);
		return b;
	}
	return -1;
}

void xPL_ENC28J60Adapter::flush()
{
	packetRelease();
}

