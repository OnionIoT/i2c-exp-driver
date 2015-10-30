#ifndef _ONION_MCP23008_DRIVER_
#define _ONION_MCP23008_DRIVER_

#include <stdlib.h>
#include <stdio.h>

#include <onion-i2c.h>


#define MCP23008_I2C_DEVICE_NUM		0
#define MCP23008_I2C_DEVICE_ADDR	0x27

#define MCP23008_NUM_GPIOS			8

// define register addresses
#define MCP23008_REG_IODIR			0x00
#define MCP23008_REG_IPOL			0x01
#define MCP23008_REG_GPINTEN		0x02
#define MCP23008_REG_DEFVAL			0x03
#define MCP23008_REG_INTCON			0x04
#define MCP23008_REG_IOCON			0x05
#define MCP23008_REG_GPPU			0x06
#define MCP23008_REG_INTF			0x07
#define MCP23008_REG_INTCAP			0x08
#define MCP23008_REG_GPIO			0x09
#define MCP23008_REG_OLAT			0x0A



// helper functions
void 		_SetBit			(int* regVal, int bitNum, int value);
int 		_GetBit			(int regVal, int bitNum);

int 		_SetReg			(int addr, int val);
int 		_ReadReg		(int addr, int* val);

int 		_SetRegBit 		(int addr, int bitNum, int bitVal);

int 		_ValidateGpio 	(int gpio);

int 		_SetAttribute 	(int gpio, int addr, int val);
int 		_GetAttribute	(int gpio, int addr, int* val);


// functions to set a single GPIO's attributes
int 		mcp_setDirection			(int gpio, int bInput);
int 		mcp_setInputPolarity		(int gpio, int bActiveLow);
int 		mcp_setPullup				(int gpio, int bPullUp);
int 		mcp_setGpio					(int gpio, int value);

// functions to set attributes for all GPIOs
//	expecting 8-bit values
int 		mcp_setAllDirection			(int bInput);
int 		mcp_setAllInputPolarity 	(int bActiveLow);
int 		mcp_setAllPullup			(int bPullUp);
int 		mcp_setAllGpio				(int value);

// functions to read a single GPIO's attributes
int 		mcp_getDirection			(int gpio, int* bInput);
int 		mcp_getInputPolarity		(int gpio, int* bActiveLow);
int 		mcp_getPullup				(int gpio, int* bPullUp);
int 		mcp_getGpio					(int gpio, int* value);




#endif // _ONION_MCP23008_DRIVER_