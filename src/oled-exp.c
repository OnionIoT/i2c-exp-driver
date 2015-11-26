#include <oled-exp.h>

// Send command byte to OLED Expansion
int _oledSendCommand (int command)
{
	int controlRegister = 0x40;
	i2c_write(OLED_EXP_DEVICE_NUM, OLED_EXP_ADDR, controlRegister, command);
}

// Write display buffer to OLED
int oledDisplay ()
{
	_oledSendCommand(OLED_EXP_COLUMN_ADDR);
	_oledSendCommand(0x00);								// Column start address (0x00 = reset)
	_oledSendCommand(OLED_EXP_WIDTH - 1);				// Column end address
	_oledSendCommand(OLED_EXP_PAGE_ADDR);
	_oledSendCommand(0x00);								// Page start address (0x00 = reset)
	_oledSendCommand(OLED_EXP_PAGES - 1);				// Page end address
	for (int i = 0; i < sizeof(_buffer) / sizeof(_buffer[0]); i += I2C_BUFFER_SIZE) {
		int controlRegister = 0x40;
		i2c_writeBuffer(OLED_EXP_DEVICE_NUM, OLED_EXP_ADDR, controlRegister, _buffer + i, I2C_BUFFER_SIZE);
	}

	return EXIT_SUCCESS;
}

int oledPrintChar(char c)
{
	int charIndex = (int) c - 32;

	if (c == "\n") {
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

		int buff[] = asciiTable[charIndex];
		memcpy(_buffer + _cursor, buff, OLED_EXP_CHAR_LENGTH);
		_cursor += OLED_EXP_CHAR_LENGTH;
	}
}

int oledLineScroll()
{
	memmove(_buffer, _buffer + (OLED_EXP_WIDTH * OLED_EXP_CHAR_LENGTH), OLED_EXP_WIDTH * (OLED_EXP_PAGES - 1) * OLED_EXP_CHAR_LENGTH);
	_cursor = OLED_EXP_WIDTH * (OLED_EXP_PAGES - 1);

	return EXIT_SUCCESS;
}

int oledNewLine();
int oledPrintLine();
int oledInvert();
int oledClear();
int oledSetContrast(int contrast);
int oledSetDim(bool dim);

// Initialize the OLED Expansion
int oledDriverInit ()
{
	memset(_buffer, 0, sizeof(_buffer));				// Reinitialize the buffer
	_cursor = 0;										// Reinitialize the cursor

	_vccState = OLED_EXP_SWITCH_CAP_VCC;

	// Initialize the screen for 128x64
	_oledSendCommand(OLED_EXP_DISPLAY_OFF);
	_oledSendCommand(OLED_EXP_SET_DISPLAY_CLOCK_DIV);
	_oledSendCommand(0x80); 							// The suggested ratio 0x80
	_oledSendCommand(OLED_EXP_SET_MULTIPLEX);
	_oledSendCommand(0x3F);
	_oledSendCommand(OLED_EXP_SET_DISPLAY_OFFSET);
	_oledSendCommand(0x00);
	_oledSendCommand(OLED_EXP_SET_START_LINE | 0x00); 	// Set start line to line 0
	_oledSendCommand(OLED_EXP_CHARGE_PUMP);
	if (_vccState == OLED_EXP_EXTERNAL_VCC) {
		_oledSendCommand(0x10);
	} else {
		_oledSendCommand(0x14);
	}
	_oledSendCommand(OLED_EXP_MEMORY_MODE);
	_oledSendCommand(0x00);
	_oledSendCommand(OLED_EXP_SEG_REMAP | 0x01);
	_oledSendCommand(OLED_EXP_COM_SCAN_DEC);
	_oledSendCommand(OLED_EXP_SET_COM_PINS);
	_oledSendCommand(0x12);
	_oledSendCommand(OLED_EXP_SET_CONTRAST);
	if (_vccState == OLED_EXP_EXTERNAL_VCC) {
		_oledSendCommand(0x9F);
	} else {
		_oledSendCommand(0xCF);
	}
	_oledSendCommand(OLED_EXP_SET_PRECHARGE);
	if (_vccState == OLED_EXP_EXTERNAL_VCC) {
		_oledSendCommand(0x22);
	} else {
		_oledSendCommand(0xF1);
	}
	_oledSendCommand(OLED_EXP_SET_VCOM_DETECT);
	_oledSendCommand(0x40);
	_oledSendCommand(OLED_EXP_DISPLAY_ALL_ON_RESUME);
	_oledSendCommand(OLED_EXP_NORMAL_DISPLAY);
	_oledSendCommand(OLED_EXP_SEG_REMAP);
	_oledSendCommand(OLED_EXP_COM_SCAN_INC);

	_oledSendCommand(OLED_EXP_DISPLAY_ON);

	return EXIT_SUCCESS;
}

