/*****************************************************************************
CORE XPL OPTIONS
*****************************************************************************/
// Allow device to be configured (about 3kB flash)
//#define XPL_CONFIG
//#define XPL_GROUPS
//#define XPL_FILTERS
//interval to send config.basic messages
#define XPL_CONFIG_INTERVAL 1
//interval to send hbeat.basic messages
#define XPL_HBEAT_INTERVAL 5
//Allow interrupts usage
//#define XPL_INTERRUPTS
// xPL device configurable (1534 octets)
#define XPL_CONFIGURE


/*****************************************************************************
ETHERNET OPTIONS
*****************************************************************************/

#define ENC28J60_PIN 10
//#define XPL_IP
#define XPL_DHCP 
// xPL default port, do not change
#define XPL_PORT 3865

#define PS2_CLK_PIN 6
#define PS2_DATA_PIN 5

