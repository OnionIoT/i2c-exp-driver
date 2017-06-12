#include <onion-i2c.h>


// get a file handle to the device 
int _i2c_getFd(int adapterNum, int *devHandle)
{
	int 	status;
	char 	pathname[255];

	// define the path to open
	status = snprintf(pathname, sizeof(pathname), I2C_DEV_PATH, adapterNum);

	// check the filename
	if (status < 0 || status >= sizeof(pathname)) {
		// add errno
        return EXIT_FAILURE;
    }

	// create a file descriptor for the I2C bus
#ifdef I2C_ENABLED
  	*devHandle = open(pathname, O_RDWR);
#else
  	*devHandle = 0;
#endif

  	// check the defvice handle
  	if (*devHandle < 0) {
  		// add errno
  		return EXIT_FAILURE;
  	}

  	return EXIT_SUCCESS;
}

// release the device file handle
int _i2c_releaseFd(int devHandle)
{
#ifdef I2C_ENABLED
	if ( close(devHandle) < 0 ) {
		return EXIT_FAILURE;
	}
#endif

	return EXIT_SUCCESS;
}

// set the device address
int _i2c_setDevice(int devHandle, int addr)
{
#ifdef I2C_ENABLED
	// set to 7-bit addr
	if ( ioctl(devHandle, I2C_TENBIT, 0) < 0 ) {
		return EXIT_FAILURE;
	}

	// set the address
	if ( ioctl(devHandle, I2C_SLAVE, addr) < 0 ) {
		return EXIT_FAILURE;
	}
#endif

	return EXIT_SUCCESS;
}

// set the 10bit device address
int _i2c_setDevice10bit(int devHandle, int addr)
{
#ifdef I2C_ENABLED
	// set to 10-bit addr
	if ( ioctl(devHandle, I2C_TENBIT, 1) < 0 ) {
		return EXIT_FAILURE;
	}

	// set the address
	if ( _i2c_setDevice(devHandle, addr) != EXIT_SUCCESS ) {
		return EXIT_FAILURE;
	}
#endif

	return EXIT_SUCCESS;
}

// generic function to write a buffer to the i2c bus
int _i2c_writeBuffer(int devNum, int devAddr, uint8_t *buffer, int size)
{
	int 	status;
	int 	fd, index;

	// open the file handle
	status 	= _i2c_getFd(devNum, &fd);

	// set the device address
	if ( status == EXIT_SUCCESS ) {
		status 	= _i2c_setDevice(fd, devAddr);
	}

	onionPrint(ONION_SEVERITY_DEBUG_EXTRA, "%s writing buffer:\n", I2C_PRINT_BANNER);
	for (index = 0; index < size; index++) {
		onionPrint(ONION_SEVERITY_DEBUG_EXTRA, "\tbuffer[%d]: 0x%02x\n", index, buffer[index]);
	}

	// perform the write
	if ( status == EXIT_SUCCESS ) {
#ifdef I2C_ENABLED
		// write to the i2c device
		status = write(fd, buffer, size);
		if (status != size) {
			onionPrint(ONION_SEVERITY_FATAL, "%s write issue for register 0x%02x, errno is %d: %s\n", I2C_PRINT_BANNER, buffer[0], errno, strerror(errno) );
			status 	= EXIT_FAILURE;
		}
		else {
			status	= EXIT_SUCCESS;
		}
#endif
 	}

 	// release the device file handle
 	status 	|= _i2c_releaseFd(fd);

	return (status);
}

// generic function to write a buffer to the i2c bus
int i2c_writeBuffer(int devNum, int devAddr, int addr, uint8_t *buffer, int size)
{
	int 	status;
	uint8_t *bufferNew;

	// allocate the new buffer
	size++;		// adding addr to buffer
	bufferNew 	= malloc( size * sizeof *bufferNew );

	// add the address to the data buffer
	bufferNew[0]	= addr;
	memcpy( &bufferNew[1], &buffer[0], size * sizeof *buffer );

 	// perform the write
 	status 	= _i2c_writeBuffer(devNum, devAddr, bufferNew, size);

 	// free the allocated memory
 	free(bufferNew);

	return (status);
}

// write n bytes to the i2c bus
int i2c_write(int devNum, int devAddr, int addr, int val)
{
	int 	status;
	int 	size, tmp, index;
	uint8_t	buffer[32]; // TODO: either this should be set to I2C_BUFFER_SIZE
    // or the memset below should be 32, else this pattern will cause buffer overrun
    // if one mistakenly sets a smaller buffer

	//// buffer setup
	// clear the buffer
	memset( buffer, 0, I2C_BUFFER_SIZE );
	// push the address and data values into the buffer
	buffer[0]	= (addr & 0xff);
	buffer[1]	= (val & 0xff);
	size 		= 2;

	// if value is more than 1-byte, add to the buffer
	tmp 	= (val >> 8);	// start with byte 1
	index	= 2;
	while (tmp > 0x00) {
		buffer[index] = (uint8_t)(tmp & 0xff);

		tmp	= tmp >> 8; // advance the tmp data by a byte
		index++; 		// increment the index

		size++;			// increase the size
	}

	onionPrint(ONION_SEVERITY_DEBUG, "%s Writing to device 0x%02x: addr = 0x%02x, data = 0x%02x (data size: %d)\n", I2C_PRINT_BANNER, devAddr, addr, val, (size-1) );

	// write the buffer
 	status = _i2c_writeBuffer(devNum, devAddr, buffer, size);

	return (status);
}

// write a specified number of bytes to the i2c bus
int i2c_writeBytes(int devNum, int devAddr, int addr, int val, int numBytes)
{
	int 	status;
	int 	size, index;
	uint8_t	buffer[I2C_BUFFER_SIZE];

	//// buffer setup
	// clear the buffer
	memset( buffer, 0, sizeof(buffer) );
	// push the address and data values into the buffer
	buffer[0]	= (addr & 0xff);
	size 		= 1;

	// add all data bytes to buffer
	index	= 1;
	for (index = 0; index < numBytes; index++) {
		buffer[index+1] = (uint8_t)( (val >> (8*index)) & 0xff );

		size++;			// increase the size
	}

	onionPrint(ONION_SEVERITY_DEBUG, "%s Writing to device 0x%02x: addr = 0x%02x, data = 0x%02x (data size: %d)\n", I2C_PRINT_BANNER, devAddr, addr, val, (size-1) );

	// write the buffer
	status 	= _i2c_writeBuffer(devNum, devAddr, buffer, size);

	return (status);
}

// read a byte from the i2c bus
int i2c_read(int devNum, int devAddr, int addr, uint8_t *buffer, int numBytes)
{
	int 	status, size, index, data, tmp;
	int 	fd;

	onionPrint(ONION_SEVERITY_DEBUG, "%s Reading %d byte%s from device 0x%02x: addr = 0x%02x", I2C_PRINT_BANNER, numBytes, (numBytes > 1 ? "s": ""), devAddr, addr);

	// open the device file handle
	status 	= _i2c_getFd(devNum, &fd);

	// set the device address
	if ( status == EXIT_SUCCESS ) {
		status 	= _i2c_setDevice(fd, devAddr);
	}

	// perform the read 	
	if ( status == EXIT_SUCCESS ) {
		//// set addr
		// clear the buffer
		memset( buffer, 0, numBytes );
		// push the address and data values into the buffer
		buffer[0]	= (addr & 0xff);
		size 		= 1;

#ifdef I2C_ENABLED
		// write to the i2c device
		status = write(fd, buffer, size);
		if (status != size) {
			onionPrint(ONION_SEVERITY_FATAL, "%s write issue for register 0x%02x, errno is %d: %s\n", I2C_PRINT_BANNER, addr, errno, strerror(errno) );
		}
#endif

		//// read data
		// clear the buffer
		memset( buffer, 0, numBytes );

#ifdef I2C_ENABLED
		// read from the i2c device
		size 	= numBytes;
		status 	= read(fd, buffer, size);
		if (status != size) {
			onionPrint(ONION_SEVERITY_FATAL, "%s read issue for register 0x%02x, errno is %d: %s\n", I2C_PRINT_BANNER, addr, errno, strerror(errno) );
			status 	= EXIT_FAILURE;
		}
		else {
			status 	= EXIT_SUCCESS;
		}
#else
		buffer[0]	= 0x0;
		size 		= 1;
		/*
		// for debug
		printf("Setting buffer... it has length of %d\n", strlen(buffer) );
		buffer[0] 	= 0x34;
		buffer[1] 	= 0x12;
		size = 2;
		printf("Done setting buffer... it has length of %d\n", strlen(buffer) );
		printf("size is %d\n", size);*/
#endif		

		//// print the data
		onionPrint(ONION_SEVERITY_DEBUG, "\tread %d byte%s, value: 0x", size, (size > 1 ? "s" : "") );

		for (index = (size-1); index >= 0; index--) {
			onionPrint(ONION_SEVERITY_DEBUG, "%02x", (buffer[index] & 0xff) );
		}
		onionPrint(ONION_SEVERITY_DEBUG, "\n");
 	}

 	// release the device file handle
 	status 	|= _i2c_releaseFd(fd);

	return (status);
}

// read a single byte from the i2c bus
int i2c_readByte(int devNum, int devAddr, int addr, int *val)
{
	int 	status;
	uint8_t	buffer[I2C_BUFFER_SIZE]; // TODO: is allocating an entire buffer needed?

	status	= i2c_read	(	devNum, 
							devAddr, 
							addr, 
							buffer,
							1
						);

	*val 	= (int)(buffer[0]);

	return (status);
}

