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
#include <stdarg.h>
#include <errno.h>

#define I2C_DEV_PATH		"/dev/i2c-%d"
#define I2C_PRINT_BANNER	"onion-i2c::"


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

#ifdef __cplusplus
extern "C"{
#endif 

int 	i2cVerbosityLevel;

// helper functions
void 	_i2c_print 				(const char* msg, ...);

int 	_i2c_getFd 				(int adapterNum, int *devHandle);
int 	_i2c_releaseFd			(int devHandle);

int 	_i2c_setDevice 			(int devHandle, int addr);
int 	_i2c_setDevice10bit 	(int devHandle, int addr);

int 	_i2c_writebuffer		(int devNum, int devAddr, int addr, uint8_t *buffer, int size);


// i2c functions
void 	i2c_setVerbosity 		(int level);

int 	i2c_write	 			(int devNum, int devAddr, int addr, int val);
int 	i2c_writeBytes 			(int devNum, int devAddr, int addr, int val, int numBytes);

int 	i2c_read 				(int devNum, int devAddr, int addr, uint8_t *buffer, int numBytes);
int 	i2c_readByte 			(int devNum, int devAddr, int addr, int *val);

#ifdef __cplusplus
}
#endif 
#endif // _ONION_I2C_H_ 