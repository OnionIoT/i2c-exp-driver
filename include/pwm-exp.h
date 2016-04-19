#ifndef _PWM_EXP_H_
#define _PWM_EXP_H_

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

#include <onion-i2c.h>




#define PWM_I2C_DEVICE_NUM			(I2C_DEFAULT_ADAPTER)
#define PWM_I2C_DEVICE_ADDR			0x5a



#define LED_FULL_VAL			0x1000

#define PWM_FREQUENCY_DEFAULT	50.0f
#define PWM_FREQUENCY_MIN		24.0f
#define PWM_FREQUENCY_MAX		1526.0f

#define OSCILLATOR_CLOCK		25000000.0f
#define	PULSE_TOTAL_COUNT		4096

#define PRESCALE_MIN_VALUE		0x03
#define PRESCALE_MAX_VALUE		0xff

#define PWM_EXP_REG_MODE1			0x00
#define PWM_EXP_REG_MODE2			0x01
#define PWM_EXP_REG_ADDR_PRESCALE	0xfe

#define PWM_EXP_REG_MODE1_RESET		0x80
#define PWM_EXP_REG_MODE1_SLEEP		0x10
#define PWM_EXP_REG_MODE1_ALLCALL	0x01
#define PWM_EXP_REG_MODE2_OUTDRV	0x04


#define REG_OFFSET_BYTE0		0x0
#define REG_OFFSET_BYTE1		0x1

#define REG_OFFSET_ON_BYTES		0x0
#define REG_OFFSET_OFF_BYTES	0x2

#define PWM_EXP_NUM_CHANNELS	16

// type definitions
typedef enum e_PwmDriverAddr {
	PWM_EXP_REG_ADDR_DRIVER0 		= 0x06,
	PWM_EXP_REG_ADDR_DRIVER1 		= 0x0a,
	PWM_EXP_REG_ADDR_DRIVER2 		= 0x0e,
	PWM_EXP_REG_ADDR_DRIVER3 		= 0x12,
	PWM_EXP_REG_ADDR_DRIVER4 		= 0x16,
	PWM_EXP_REG_ADDR_DRIVER5 		= 0x1a,
	PWM_EXP_REG_ADDR_DRIVER6 		= 0x1e,
	PWM_EXP_REG_ADDR_DRIVER7 		= 0x22,
	PWM_EXP_REG_ADDR_DRIVER8 		= 0x26,
	PWM_EXP_REG_ADDR_DRIVER9 		= 0x2a,
	PWM_EXP_REG_ADDR_DRIVER10 		= 0x2e,
	PWM_EXP_REG_ADDR_DRIVER11 		= 0x32,
	PWM_EXP_REG_ADDR_DRIVER12 		= 0x36,
	PWM_EXP_REG_ADDR_DRIVER13 		= 0x3a,
	PWM_EXP_REG_ADDR_DRIVER14 		= 0x3e,
	PWM_EXP_REG_ADDR_DRIVER15 		= 0x42,
	PWM_EXP_REG_ADDR_DRIVER_ALL		= 0xfa
} ePwmDriverAddr;

struct pwmSetup {
	int 	driverNum;
	int 	regOffset;
	
	int 	timeStart;
	int 	timeEnd;

	int 	prescale;
};


#ifdef __cplusplus
extern "C" {
#endif

// helper functions
void 	_initPwmSetup		(struct pwmSetup *obj);
int 	_dutyToCount 		(float duty);

int 	_getDriverRegisterOffset (int driverNum, int *addr);
int 	_writeValue			(int addr, int value);
int 	_pwmSetTime			(struct pwmSetup *setup);

int 	_pwmSetReset 		(void);

int 	_pwmGetSleepMode 	(int *bSleepMode);
int 	_pwmSetSleepMode 	(int bSleepMode);


void 	_pwmCalculate		(float duty, float delay, struct pwmSetup *setup);


// programming functions
int 	pwmCheckInit 		(int *bInitialized);
int 	pwmDriverInit 		();

int 	pwmDisableChip 		();

int 	pwmSetFrequency		(float freq);
int 	pwmSetupDriver		(int driverNum, float duty, float delay);

#ifdef __cplusplus
}
#endif 
#endif // _PWM_EXP_H_