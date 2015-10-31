#include <onion-mcp23008-driver.h>

// change the value of a single bit
void _SetBit(int* regVal, int bitNum, int value)
{
	(*regVal) ^= (-value ^ (*regVal)) & (1 << bitNum);
}

// find the value of a single bit
int _GetBit(int regVal, int bitNum)
{
	int value;

	// isolate the specific bit
	value = ((regVal >> bitNum) & 0x1);

	return (value);
}

// overwrite all values in register
int _SetReg (int addr, int val)
{
	int status;

	status 	= i2c_writeByte	(	MCP23008_I2C_DEVICE_NUM, 
								MCP23008_I2C_DEVICE_ADDR, 
								addr, 
								val
							);
	if (status == EXIT_FAILURE) {
		printf("mcp-driver:: writing value 0x%02x to addr 0x%02x failed\n", val, addr);
	}

	return status;
}

// read register value
int _ReadReg (int addr, int* val)
{
	int status;

	status 	= i2c_readByte	(	MCP23008_I2C_DEVICE_NUM, 
								MCP23008_I2C_DEVICE_ADDR, 
								addr, 
								val
							);
	if (status == EXIT_FAILURE) {
		printf("mcp-driver:: read from addr 0x%02x failed\n", addr);
	}

	return status;
}

// set only a single bit in a register
int _SetRegBit (int addr, int bitNum, int bitVal)
{
	int status, val;

	// read the value
	status	= _ReadReg(addr, &val);
	if (status == EXIT_FAILURE) {
		return status;
	}

	// inject the bit value
	_SetBit(&val, bitNum, bitVal);

	// write the value
	status 	= _SetReg(addr, val);

	return status;
}

// validate gpio number
int _ValidateGpio (int gpio)
{
	int status = EXIT_FAILURE;

	if (gpio >= 0 && gpio < MCP23008_NUM_GPIOS) {
		status = EXIT_SUCCESS;
	}

	return status;
}

// function to generalize attribute setting functions
int _SetAttribute(int gpio, int addr, int val)
{
	int status;

	// validate the gpio number
	status	= _ValidateGpio(gpio);
	if (status == EXIT_FAILURE) {
		return status;
	}

	// perform the register write to set the attribute
	status 	= _SetRegBit(addr, gpio, val & 0x01);

	return status;
}

// function to generalize attribute reading functions
int _GetAttribute(int gpio, int addr, int* val)
{
	int status, regVal;

	// validate the gpio number
	status	= _ValidateGpio(gpio);
	if (status == EXIT_FAILURE) {
		return status;
	}

	// read the register
	status 	= _ReadReg(addr, &regVal);
	if (status == EXIT_FAILURE) {
		return status;
	}

	// isolate the specific bit
	*val 	= _GetBit(regVal, gpio);

	return status;
}


// set attribute functions
int mcp_setDirection (int gpio, int bInput)
{
	int status; 

	status	= _SetAttribute(gpio, MCP23008_REG_IODIR, bInput);

	return status;
}

int mcp_setInputPolarity (int gpio, int bActiveLow)
{
	int status; 

	status	= _SetAttribute(gpio, MCP23008_REG_IPOL, bActiveLow);

	return status;
}

int mcp_setPullup (int gpio, int bPullUp)
{
	int status; 

	status	= _SetAttribute(gpio, MCP23008_REG_GPPU, bPullUp);

	return status;
}

int mcp_setGpio (int gpio, int value)
{
	int status; 

	status	= _SetAttribute(gpio, MCP23008_REG_GPIO, value);

	return status;
}


// functions to set attribute for all gpios
int mcp_setAllDirection (int bInput)
{
	int status; 

	status	= _SetReg(MCP23008_REG_IODIR, bInput);

	return status;
}

int mcp_setAllInputPolarity (int bActiveLow)
{
	int status; 

	status	= _SetReg(MCP23008_REG_IPOL, bActiveLow);

	return status;
}

int mcp_setAllPullup (int bPullUp)
{
	int status; 

	status	= _SetReg(MCP23008_REG_GPPU, bPullUp);

	return status;
}

int mcp_setAllGpio (int value)
{
	int status; 

	status	= _SetReg(MCP23008_REG_GPIO, value);

	return status;
}


// functions to read attribute for specific gpio
int mcp_getDirection (int gpio, int* bInput)
{
	int status, val;

	status	= _GetAttribute(gpio, MCP23008_REG_IODIR, bInput);

	return status;
}

int mcp_getInputPolarity (int gpio, int* bActiveLow)
{
	int status, val;

	status	= _GetAttribute(gpio, MCP23008_REG_IPOL, bActiveLow);

	return status;
}

int mcp_getPullup (int gpio, int* bPullUp)
{
	int status, val;

	status	= _GetAttribute(gpio, MCP23008_REG_GPPU, bPullUp);

	return status;
}

int mcp_getGpio (int gpio, int* value)
{
	int status, val;

	status	= _GetAttribute(gpio, MCP23008_REG_GPIO, value);

	return status;
}






