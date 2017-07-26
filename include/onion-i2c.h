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
#include <stdint.h>
#include <errno.h>

#include <onion-debug.h>


#define I2C_DEV_PATH		"/dev/i2c-%d"
#define I2C_PRINT_BANNER	"onion-i2c::"

#define I2C_BUFFER_SIZE		32

#define I2C_DEFAULT_ADAPTER	0


// for debugging
#ifndef __APPLE__
	#define I2C_ENABLED		1
#endif


#ifdef __cplusplus
extern "C" {
#endif 



// helper functions
int 	_i2c_getFd 				(int adapterNum, int *devHandle);
int 	_i2c_releaseFd			(int devHandle);

int 	_i2c_setDevice 			(int devHandle, int addr);
int 	_i2c_setDevice10bit 	(int devHandle, int addr);

int 	_i2c_writeBuffer		(int devNum, int devAddr, uint8_t *buffer, int size);


// i2c functions
int 	i2c_writeBuffer			(int devNum, int devAddr, int addr, uint8_t *buffer, int size);
int 	i2c_writeBufferRaw		(int devNum, int devAddr, uint8_t *buffer, int size);
int 	i2c_write	 			(int devNum, int devAddr, int addr, int val);
int 	i2c_writeBytes 			(int devNum, int devAddr, int addr, int val, int numBytes);

int 	i2c_read 				(int devNum, int devAddr, int addr, uint8_t *buffer, int numBytes);
int 	i2c_readRaw				(int devNum, int devAddr, uint8_t *buffer, int numBytes);
int 	i2c_readByte 			(int devNum, int devAddr, int addr, int *val);

#ifdef __cplusplus
}
#endif 
#endif // _ONION_I2C_H_ 
