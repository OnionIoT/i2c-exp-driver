#ifndef _ONION_I2C_H_
#define _ONION_I2C_H_

#include <stdlib.h>
#include <unistd.h>

#ifndef __APPLE__
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#endif


#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define I2C_DEV_PATH		"/dev/i2c-%d"


// for debugging
#ifndef __APPLE__
	#define I2C_ENABLED		1
#endif


//#define I2C_DEBUG
#ifdef I2C_DEBUG
	#define I2C_PRINT(a,...)		printf (a,##__VA_ARGS__)
#else
	#define I2C_PRINT(a,...)
#endif



int _i2c_getFd 				(int adapterNum, int *devHandle);
int _i2c_releaseFd			(int devHandle);

int _i2c_setDevice 			(int devHandle, int addr);
int _i2c_setDevice10bit 	(int devHandle, int addr);

int i2c_writeByte 			(int devNum, int devAddr, int addr, int val);
int i2c_readByte 			(int devNum, int devAddr, int addr, int *val);




#endif // _ONION_I2C_H_