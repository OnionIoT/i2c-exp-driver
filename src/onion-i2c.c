#include <onion-i2c.h>

// get a file handle to the device
int _i2c_getFd(int adapterNum, int &devHandle)
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
  	devHandle = open(pathname, O_RDWR);
#else
  	devHandle = 0;
#endif

  	// check the defvice handle
  	if (devHandle < 0) {
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

// write to the i2c bus
int i2c_writeByte(int devNum, int devAddr, int addr, int val)
{
	int 	status;
	int 	size;
	int 	fd;
	char 	buffer[32];

	printf("i2c:: Writing to device 0x%02x: addr = 0x%02x, data = 0x%02x\n", devAddr, addr, val);

	// open the file handle
	status 	= _i2c_getFd(devNum, fd);

	// set the device address
	if ( status == EXIT_SUCCESS ) {
		status 	= _i2c_setDevice(fd, devAddr);
	}

	// perform the write
	if ( status == EXIT_SUCCESS ) {
		//// buffer setup
		// clear the buffer
		memset( buffer, 0, sizeof(buffer) );
		// push the address and data values into the buffer
		buffer[0]	= (addr & 0xff);
		buffer[1]	= (val & 0xff);
		size 		= 2;

		// if value is more than 1-byte, add to the buffer
		if (val > 0xff) {
			buffer[2] = ((val >> 8) & 0xff);
			size++;
		}

#ifdef I2C_ENABLED
		// write to the i2c device
		status = write(fd, buffer, size);
#endif

		printf("i2c:: dbg: write returned status %d\n", status);
		// add error checking here!
 	}

 	// release the device file handle
 	status 	= _i2c_releaseFd(fd);

	return (status);
}

// read from the i2c bus
int i2c_readByte(int devNum, int devAddr, int addr, int &val)
{
	int 	status;
	int 	fd;
	char 	buffer[32];

	printf("i2c:: Reading from device 0x%02x: addr = 0x%02x\n", devAddr, addr);

	// open the device file handle
	status 	= _i2c_getFd(devNum, fd);

	// set the device address
	if ( status == EXIT_SUCCESS ) {
		status 	= _i2c_setDevice(fd, devAddr);
	}

	// perform the read
	if ( status == EXIT_SUCCESS ) {
		//// set addr
		// clear the buffer
		memset( buffer, 0, sizeof(buffer) );
		// push the address and data values into the buffer
		buffer[0]	= (addr & 0xff);

#ifdef I2C_ENABLED
		// write to the i2c device
		status = write(fd, buffer, 1);
#endif

		printf("i2c:: dbg: write returned status %d\n", status);
		// add error checking here!

		//// read data
		// clear the buffer
		memset( buffer, 0, sizeof(buffer) );

#ifdef I2C_ENABLED
		// read from the i2c device
		status = read(fd, buffer, 1);
#endif

		printf("i2c:: dbg: read returned status %d\n", status);
		// add error checking here!

		//// return the data
		val = buffer[0];
 	}

 	// release the device file handle
 	status 	= _i2c_releaseFd(fd);

	return (status);
}
