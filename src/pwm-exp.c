#include <pwm-exp.h>


// initialize the pwm info struct
void _initPwmSetup(pwmSetup *obj)
{
	obj->driverNum 		= -1;
	obj->regOffset 		= 0;

	obj->timeStart		= 0;
	obj->timeEnd 		= 0;

	obj->prescale		= 0;
}

// return register offset for specified driver/channel
int _getDriverRegisterOffset (int driverNum, int &addr)
{
	int len; 

	// define array of register offsets
	int pwmDriverAddr[17] = {
		PWM_EXP_REG_ADDR_DRIVER0,
		PWM_EXP_REG_ADDR_DRIVER1,
		PWM_EXP_REG_ADDR_DRIVER2,
		PWM_EXP_REG_ADDR_DRIVER3,
		PWM_EXP_REG_ADDR_DRIVER4,
		PWM_EXP_REG_ADDR_DRIVER5,
		PWM_EXP_REG_ADDR_DRIVER6,
		PWM_EXP_REG_ADDR_DRIVER7,
		PWM_EXP_REG_ADDR_DRIVER8,
		PWM_EXP_REG_ADDR_DRIVER9,
		PWM_EXP_REG_ADDR_DRIVER10,
		PWM_EXP_REG_ADDR_DRIVER11,
		PWM_EXP_REG_ADDR_DRIVER12,
		PWM_EXP_REG_ADDR_DRIVER13,
		PWM_EXP_REG_ADDR_DRIVER14,
		PWM_EXP_REG_ADDR_DRIVER15,
		PWM_EXP_REG_ADDR_DRIVER_ALL
	};

	// check the input
	len = sizeof(pwmDriverAddr) / sizeof(pwmDriverAddr[0]);

	// find the address
	if (driverNum < 0) {
		addr = PWM_EXP_REG_ADDR_DRIVER_ALL;
	}
	if (driverNum < len) {
		addr = pwmDriverAddr[driverNum];
	}
	else {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

// find time count based on duty (1-100)
int _dutyToCount (int duty)
{
	int 	count;
	float 	dutyF 	= (float)duty / 100.0f;

	// convert duty to count
	count 	= (int)round(dutyF * (float)PULSE_TOTAL_COUNT);

	// check for negatives
	if (count < 0) {
		count = 0;
	}

	return (count);
}

// split value into two bytes, and write to the L and H registers (via I2C)
int _writeValue(int addr, int value)
{
	int status, byte, wrAddr;

	// write first byte to L
	wrAddr 	= addr + REG_OFFSET_BYTE0;
	byte 	= (value & 0xff);

	printf("Writing to addr: 0x%02x, data: 0x%02x\n", wrAddr, byte);
	status 	= i2c_writeByte(I2C_DEVICE_NUM, I2C_DEVICE_ADDR, wrAddr, byte);

	// write second byte to H
	wrAddr 	= addr + REG_OFFSET_BYTE1;
	byte 	= ((value >> 8) & 0xff);
	
	printf("Writing to addr: 0x%02x, data: 0x%02x\n", wrAddr, byte);
	status 	|= i2c_writeByte(I2C_DEVICE_NUM, I2C_DEVICE_ADDR, wrAddr, byte);

	return status;
}

// write ON and OFF time values
int _pwmSetTime(pwmSetup *setup)
{
	int status;

	// set the ON time
	status = _writeValue(setup->regOffset + REG_OFFSET_ON_BYTES, setup->timeStart);

	// set the OFF time
	status |= _writeValue(setup->regOffset + REG_OFFSET_OFF_BYTES, setup->timeEnd);

	return (status);
}

// calculate the ON and OFF time values
void _pwmCalculate(int duty, int delay, pwmSetup *setup)
{
	// find duty and delay in terms of numbers
	int 	countOn 	= _dutyToCount(duty);	
	int 	countDelay 	= _dutyToCount(delay);

	// calculate the time to assert and deassert the signal
	setup->timeStart	= countDelay;
	if ( setup->timeStart > 0 ) {
		setup->timeStart -= 1;
	}

	setup->timeEnd		= setup->timeStart + countOn;

	// take care of the case where delay + duty are more than 100
	if (setup->timeEnd > PULSE_TOTAL_COUNT) {
		setup->timeEnd 	-= PULSE_TOTAL_COUNT;
	}
}

// program the prescale value for desired pwm frequency
int pwmSetFrequency(int freq, pwmSetup *setup)
{
	int status;

	// prescale = round( osc_clk / pulse_count x update_rate ) - 1
	setup->prescale 	= (int)round( OSCILLATOR_CLOCK/(PULSE_TOTAL_COUNT * freq) ) - 1;

	// clamp the value
	if (setup->prescale < PRESCALE_MIN_VALUE) {
		setup->prescale = PRESCALE_MIN_VALUE;
	}
	else if (setup->prescale > PRESCALE_MAX_VALUE) {
		setup->prescale = PRESCALE_MAX_VALUE;
	}

	printf("DBG: freq: %d, prescale: 0x%02x\n", freq, setup->prescale);

	status = i2c_writeByte	(	I2C_DEVICE_NUM, 
								I2C_DEVICE_ADDR, 
								PWM_EXP_REG_ADDR_PRESCALE, 
								setup->prescale
							);
	

	return status;
}

// perform PWM driver setup based on duty and delay
int pwmSetupDriver(int driverNum, int duty, int delay)
{
	int 		status;
	pwmSetup 	setup;

	// initialize the setup structure
	_initPwmSetup(&setup);

	// find driver number and then register offset
	setup.driverNum 	= driverNum;
	status 	= _getDriverRegisterOffset(driverNum, setup.regOffset);

	// find on and off times
	_pwmCalculate(duty, delay, &setup);

	printf("DBG: duty: %d, delay: %d\n", duty, delay);
	printf("DBG: start: %d (0x%04x), stop: %d (0x%04x)\n\n", setup.timeStart, setup.timeStart, setup.timeEnd, setup.timeEnd);

	// write on and off times via i2c
	if (status == EXIT_SUCCESS) {
		status 	= _pwmSetTime(&setup);
	}


	return status;
}


