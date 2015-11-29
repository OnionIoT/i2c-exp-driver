#include <oled-exp.h>

// Send command byte to OLED Expansion
int _oledSendCommand (int command)
{
	int status;

	status = i2c_write(OLED_EXP_DEVICE_NUM, OLED_EXP_ADDR, OLED_EXP_REG_CONTROL, command);

	return status;
}

// Initialize the OLED Expansion
int oledDriverInit ()
{
	int 	status;

	memset(_buffer, 0, sizeof(_buffer));				// Reinitialize the buffer
	_cursor = 0;										// Reinitialize the cursor

	// set default
	_vccState = OLED_EXP_SWITCH_CAP_VCC;

	// Initialize the screen for 128x64
	status 	= _oledSendCommand(OLED_EXP_DISPLAY_OFF);
	usleep(4500);

	status 	|= _oledSendCommand(OLED_EXP_SET_DISPLAY_CLOCK_DIV);
	status 	|= _oledSendCommand(0x80); 								// The suggested ratio 0x80
	status 	|= _oledSendCommand(OLED_EXP_SET_MULTIPLEX);
	status 	|= _oledSendCommand(0x3F);
	status 	|= _oledSendCommand(OLED_EXP_SET_DISPLAY_OFFSET);
	status 	|= _oledSendCommand(0x00);								// no offset
	status 	|= _oledSendCommand(OLED_EXP_SET_START_LINE | 0x00); 	// Set start line to line #0
	status 	|= _oledSendCommand(OLED_EXP_CHARGE_PUMP);
	if (_vccState == OLED_EXP_EXTERNAL_VCC) {
		status 	|= _oledSendCommand(0x10);
	} else {
		status 	|= _oledSendCommand(0x14);
	}

	status 	|= _oledSendCommand(OLED_EXP_MEMORY_MODE);
	status 	|= _oledSendCommand(0x00);
	status 	|= _oledSendCommand(OLED_EXP_SEG_REMAP | 0x01);
	status 	|= _oledSendCommand(OLED_EXP_COM_SCAN_DEC);
	status 	|= _oledSendCommand(OLED_EXP_SET_COM_PINS);
	status 	|= _oledSendCommand(0x12);
	status 	|= _oledSendCommand(OLED_EXP_SET_CONTRAST);
	if (_vccState == OLED_EXP_EXTERNAL_VCC) {
		status 	|= _oledSendCommand(0x9F);
	} else {
		status 	|= _oledSendCommand(0xCF);
	}

	status 	|= _oledSendCommand(OLED_EXP_SET_PRECHARGE);
	if (_vccState == OLED_EXP_EXTERNAL_VCC) {
		status 	|= _oledSendCommand(0x22);
	} else {
		status 	|= _oledSendCommand(0xF1);
	}
	status 	|= _oledSendCommand(OLED_EXP_SET_VCOM_DETECT);
	status 	|= _oledSendCommand(0x40);
	status 	|= _oledSendCommand(OLED_EXP_DISPLAY_ALL_ON_RESUME);
	status 	|= _oledSendCommand(OLED_EXP_NORMAL_DISPLAY);

	status 	|= _oledSendCommand(OLED_EXP_SEG_REMAP);
	status 	|= _oledSendCommand(OLED_EXP_COM_SCAN_INC);

	status 	|= _oledSendCommand(OLED_EXP_DISPLAY_ON);
	usleep(4500);

	// clear the display
	status 	|= oledSetNormalDisplay();


	return EXIT_SUCCESS;
}

int oledSetCursor(int row, int column)
{
	int 	status;

	// set page address
	status	= _oledSendCommand(OLED_EXP_ADDR_BASE_PAGE_START + row); 

	// set column lower address
	status	|= _oledSendCommand(OLED_EXP_SET_LOW_COLUMN + (8 * column & 0x0F) ); 

	// set column higher address
    status	|= _oledSendCommand(OLED_EXP_SET_HIGH_COLUMN + ((8 * column >> 4) & 0x0F) );

    return status;
}

int oledSetNormalDisplay ()
{
	int 	status;

	status 	= _oledSendCommand(OLED_EXP_NORMAL_DISPLAY);

	return status;
}

int oledClear()
{
	int 	status;
	int 	col, row;

	// display off
	status 	= _oledSendCommand(OLED_EXP_DISPLAY_OFF);

	// write a blank space to each character
	for (row = 0; row < OLED_EXP_CHAR_ROWS; row++) {
		oledSetCursor(row, 0);

		for (col = 0; col < OLED_EXP_CHAR_COLUMNS; col++) {
			oledPrintChar(' ');
		}
	}

	// display on
	status 	|= _oledSendCommand(OLED_EXP_DISPLAY_ON);
	// reset the cursor to (0, 0)
	status	|= oledSetCursor(0, 0);

	return status;
}


int oledInvert();

int oledSetContrast(int contrast);
int oledSetDim(int dim);

// Write display buffer to OLED
int oledDisplay ()
{
	_oledSendCommand(OLED_EXP_COLUMN_ADDR);
	_oledSendCommand(0x00);								// Column start address (0x00 = reset)
	_oledSendCommand(OLED_EXP_WIDTH - 1);				// Column end address
	_oledSendCommand(OLED_EXP_PAGE_ADDR);
	_oledSendCommand(0x00);								// Page start address (0x00 = reset)
	_oledSendCommand(OLED_EXP_PAGES - 1);				// Page end address

	// to do: can write entire buffer at once
	/*
	for (int i = 0; i < sizeof(_buffer) / sizeof(_buffer[0]); i += I2C_BUFFER_SIZE) {
		int controlRegister = 0x40;
		i2c_writeBuffer(OLED_EXP_DEVICE_NUM, OLED_EXP_ADDR, controlRegister, _buffer + i, I2C_BUFFER_SIZE);
	}*/

	return EXIT_SUCCESS;
}

int oledPrintChar(char c)
{
	int 	status;
	int 	charIndex = (int) c - 32;

	if (c == '\n') {
		oledNewLine();
	} else if (charIndex >= 0 && charIndex < sizeof(asciiTable) / sizeof(asciiTable[0])) {
		// At the end of the screen
		if (_cursor >= OLED_EXP_WIDTH * OLED_EXP_PAGES - 2) {
			oledLineScroll();
		}
		_cursor++;
		if (_cursor % OLED_EXP_WIDTH >= 126) {
			_cursor += 2;
		}

		memcpy(_buffer + _cursor, asciiTable[charIndex], OLED_EXP_CHAR_LENGTH);
		_cursor += OLED_EXP_CHAR_LENGTH;
	}

	return status;
}

int oledLineScroll()
{
	memmove(_buffer, _buffer + (OLED_EXP_WIDTH * OLED_EXP_CHAR_LENGTH), OLED_EXP_WIDTH * (OLED_EXP_PAGES - 1) * OLED_EXP_CHAR_LENGTH);
	_cursor = OLED_EXP_WIDTH * (OLED_EXP_PAGES - 1);

	return EXIT_SUCCESS;
}

int oledNewLine()
{
	int 	status;

	return EXIT_SUCCESS;
}

int oledPrintLine()
{
	int 	status;

	return EXIT_SUCCESS;
}

