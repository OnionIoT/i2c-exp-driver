#include <oled-exp.h>

// Send command byte to OLED Expansion
int _oledSendCommand (int command)
{
	int status;

	status = i2c_write(OLED_EXP_DEVICE_NUM, OLED_EXP_ADDR, OLED_EXP_REG_COMMAND, (uint8_t)command);

	return status;
}

// Send data byte to OLED Expansion
int _oledSendData (int data)
{
	int status;

	status = i2c_write(OLED_EXP_DEVICE_NUM, OLED_EXP_ADDR, OLED_EXP_REG_DATA, (uint8_t)data);

	return status;
}

// Initialize the OLED Expansion
int oledDriverInit ()
{
	int 	status;

	onionPrint(ONION_SEVERITY_INFO, "> Initializing display\n");

	memset(_buffer, 0, sizeof(_buffer));	// Initialize the buffer
	_cursor 		= 0;					// Initialize the cursor
	_cursorInRow	= 0;					// Initialize the row cursor

	// set defaults
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
	status 	|= _oledSendCommand(OLED_EXP_MEM_HORIZONTAL_ADDR_MODE);
	status 	|= _oledSendCommand(OLED_EXP_SEG_REMAP | 0x01);
	status 	|= _oledSendCommand(OLED_EXP_COM_SCAN_DEC);
	status 	|= _oledSendCommand(OLED_EXP_SET_COM_PINS);
	status 	|= _oledSendCommand(0x12);
	status 	|= _oledSendCommand(OLED_EXP_SET_CONTRAST);
	if (_vccState == OLED_EXP_EXTERNAL_VCC) {
		status 	|= _oledSendCommand(OLED_EXP_DEF_CONTRAST_EXTERNAL_VCC);
	} else {
		status 	|= _oledSendCommand(OLED_EXP_DEF_CONTRAST_SWITCH_CAP_VCC);
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
	status 	|= oledSetDisplayMode(0);	// set normal display
	status	|= oledClear();


	return EXIT_SUCCESS;
}

// Clear the OLED screen
int oledClear()
{
	int 	status;
	int 	charRow, pixelCol;

	onionPrint(ONION_SEVERITY_DEBUG, "> Clearing display\n");
	// display off
	status 	= _oledSendCommand(OLED_EXP_DISPLAY_OFF);

	// write a blank space to each character
	for (charRow = 0; charRow < OLED_EXP_CHAR_ROWS; charRow++) {
		oledSetCursor(charRow, 0);

		for (pixelCol = 0; pixelCol < OLED_EXP_WIDTH; pixelCol++) {
			status 	= _oledSendData(0x00);
		}
	}

	// display on
	status 	|= _oledSendCommand(OLED_EXP_DISPLAY_ON);
	// reset the cursor to (0, 0)
	status	|= oledSetCursor(0, 0);

	return status;
}

// set the display to on or off
int oledSetDisplayPower(int bPowerOn)
{
	int 	status;
	uint8_t	cmd;

	onionPrint(ONION_SEVERITY_INFO, "> Setting display to %s\n", (bPowerOn == 1 ? "ON" : "OFF") );

	// set the command code
	if (bPowerOn == 1) {
		cmd 	= OLED_EXP_DISPLAY_ON;
	}
	else if (bPowerOn == 0) {
		cmd 	= OLED_EXP_DISPLAY_OFF;
	}

	// send the command code
	status 	= _oledSendCommand(cmd);

	return status;
}

// set the display to normal or inverted mode
int oledSetDisplayMode(int bInvert)
{
	int 	status;
	uint8_t	cmd;

	onionPrint(	(bInvert == 1 ? ONION_SEVERITY_INFO : ONION_SEVERITY_DEBUG), 
				"> Setting display mode to %s\n", 
				(bInvert == 1 ? "inverted" : "normal") 
			);

	// set the command code
	if (bInvert == 0) {
		cmd 	= OLED_EXP_NORMAL_DISPLAY;
	}
	else if (bInvert == 1) {
		cmd 	= OLED_EXP_INVERT_DISPLAY;
	}

	// send the command code
	status 	= _oledSendCommand(cmd);

	return status;
}

// set the display's contrast
int oledSetContrast(int contrast)
{
	int 	status;
	int 	contrastAdj;

	// clamp the contrast to the lower and upper limits
	contrastAdj 		= contrast;
	if (contrast < OLED_EXP_CONTRAST_MIN) {
		contrastAdj 	= OLED_EXP_CONTRAST_MIN;
	}
	if (contrast > OLED_EXP_CONTRAST_MAX) {
		contrastAdj		= OLED_EXP_CONTRAST_MAX;
	}

	// send the command
	onionPrint(ONION_SEVERITY_DEBUG, "> Setting display contrast to %d/%d\n", contrastAdj, OLED_EXP_CONTRAST_MAX);
	status 	=  _oledSendCommand(OLED_EXP_SET_CONTRAST);
	status 	|= _oledSendCommand(contrastAdj);

	return status;
}

// set the screen to normal brightness or dimmed brightness
int oledSetDim(int bDim)
{
	int 	status;
	int 	contrast;
	
	// set the contrast based on the dimness setting
	if (bDim == 1) {
		// dim 
		contrast 	= OLED_EXP_CONTRAST_MIN;
		onionPrint(ONION_SEVERITY_INFO, "> Dimming display\n");
	}
	else if (bDim == 0) {
		// normal
		contrast	= OLED_EXP_DEF_CONTRAST_SWITCH_CAP_VCC;
		if (_vccState == OLED_EXP_EXTERNAL_VCC) {
			contrast 	= OLED_EXP_DEF_CONTRAST_EXTERNAL_VCC;
		}
		onionPrint(ONION_SEVERITY_INFO, "> Setting normal display brightness\n");
	}

	// send the command
	status 	=  oledSetContrast(contrast);

	return status;
}

// set the display's memory mode
int oledSetMemoryMode(int mode)
{
	int 	status;
	int 	contrast;
	
	// check the input
	if 	(	mode != OLED_EXP_MEM_HORIZONTAL_ADDR_MODE &&
			mode != OLED_EXP_MEM_VERTICAL_ADDR_MODE &&
			mode != OLED_EXP_MEM_PAGE_ADDR_MODE
		)
	{
		onionPrint(ONION_SEVERITY_FATAL, "ERROR: Attempting to set invalid memory mode (0x%02x)\n", mode);
		return EXIT_FAILURE;
	}

	// send the command
	status 	=  _oledSendCommand(OLED_EXP_MEMORY_MODE);
	status 	=  _oledSendCommand(mode);

	// store the memory mode
	_memoryMode	= mode;

	return status;
}

// set the OLED's cursor
int oledSetCursor(int row, int column)
{
	int 	status;

	onionPrint(ONION_SEVERITY_DEBUG, "> Setting cursor to (%d, %d)\n", row, column);

	// check the inputs
	if (row < 0 || row >= OLED_EXP_CHAR_ROWS) {
		onionPrint(ONION_SEVERITY_FATAL, "ERROR: Attempting to set cursor to invalid row '%d'\n", row);
		return EXIT_FAILURE;
	}
	if (column < 0 || column >= OLED_EXP_CHAR_COLUMNS) {
		onionPrint(ONION_SEVERITY_FATAL, "ERROR: Attempting to set cursor to invalid column '%d'\n", column);
		return EXIT_FAILURE;
	}

	//// set the cursor
	// set page address
	status	= _oledSendCommand(OLED_EXP_ADDR_BASE_PAGE_START + row); 

	// set column lower address
	status	|= _oledSendCommand(OLED_EXP_SET_LOW_COLUMN + (8 * column & 0x0F) ); 

	// set column higher address
    status	|= _oledSendCommand(OLED_EXP_SET_HIGH_COLUMN + ((8 * column >> 4) & 0x0F) );

    return status;
}


//// writing functions ////
// Write a character directly to the OLED display (at the OLED cursor's current position)
int oledWriteChar(char c)
{
	int 	status;
	int 	idx;
	int 	charIndex = (int) c - 32;

	// ensure character is in the table
	if (charIndex >= 0 && charIndex < sizeof(asciiTable) / sizeof(asciiTable[0])) {
		// check where the cursor is in the current row
		if (_cursorInRow == OLED_EXP_CHAR_COLUMNS - 1) {
			// last character is cut off, write two pixels of nothing to advance to new line
			status 	= _oledSendData(0x00);
			status 	= _oledSendData(0x00);

			_cursorInRow 	= 0;
		}

		// write the data for the character
		for (idx = 0; idx < OLED_EXP_CHAR_LENGTH; idx++) {
	        status 	= _oledSendData(asciiTable[charIndex][idx]);
	    }
	    onionPrint(ONION_SEVERITY_DEBUG_EXTRA, "\twriting '%c' to column %d\n", c, _cursorInRow);

	    // increment row cursor
	    _cursorInRow++;
	}

	return status;
}

// Write a string message directly to the displayy
int oledWrite (char *msg)
{
	int 	status;
	int 	idx, i;

	onionPrint(ONION_SEVERITY_INFO, "> Writing '%s' to display\n", msg);

	// set addressing mode to page
	//oledSetMemoryMode(OLED_EXP_MEM_PAGE_ADDR_MODE);	// want automatic newlines enabled

	// write each character
	for (idx = 0; idx < strlen(msg); idx++) {
		// check for newline character
		if (msg[idx] == '\\' && msg[idx+1] == 'n' && msg[idx-1] != '\\' ) {
			// move the cursor to the next row
			for(i = _cursorInRow; i < OLED_EXP_CHAR_COLUMNS - 1; i++) {
				status 	= oledWriteChar(' ');
			}

			// increment past this newline character (skip next index)
			idx++;
		}
		else {
			status 	= oledWriteChar(msg[idx]);
		}
	}

	return status;
}

// function to reverse the bytes in a byte
uint8_t _reverseByte (uint8_t input) {
	int 	i;
	uint8_t rev;

	for (i = 0; i < 8; i++) {
		rev = ((input >> i) & 0x01) << (7 - i);
	}

	return rev;
}

// Write a buffer directly to the display
int oledDraw (uint8_t *buffer, int bytes) 
{
	int 	status;
	int 	idx;
	int 	swap;

	onionPrint(ONION_SEVERITY_INFO, "> Writing buffer data to display\n");

	// set addressing mode to horizontal (automatic newline at the end of each line)
	oledSetMemoryMode(OLED_EXP_MEM_HORIZONTAL_ADDR_MODE);

	// write each byte
	for (idx = 0; idx < bytes; idx++) {
		swap = _reverseByte(buffer[idx]);
		onionPrint(ONION_SEVERITY_DEBUG, ">> writing byte %d 0x%02x\n", idx, swap );
		status 	= _oledSendData(swap);
	}

	return status;
}


//// scrolling functions ////
// horizontal scrolling
//	direction 	scrolling
//	0 			left
//	1 			right
int oledScroll (int direction, int scrollSpeed, int startPage, int stopPage)
{
	int 	status;
	int 	scrollMode;

	onionPrint(ONION_SEVERITY_INFO, "> Enabling horizontal scrolling to the %s\n", (direction == 1 ? "right" : "left") );

	// read the direction
	if (direction == 1) {
		scrollMode 	= OLED_EXP_RIGHT_HORIZONTAL_SCROLL;
	}
	else if (direction == 0) {
		scrollMode 	= OLED_EXP_LEFT_HORIZONTAL_SCROLL;
	}

	// send the commands
	status 	=  _oledSendCommand(scrollMode);
	status 	|= _oledSendCommand(0x00);			// dummy byte
	status 	|= _oledSendCommand(startPage);		// start page addr (0 - 7)
	status 	|= _oledSendCommand(scrollSpeed);	// time interval between frames
	status 	|= _oledSendCommand(stopPage);		// end page addr (must be greater than start)
	status 	|= _oledSendCommand(0x00);			// dummy byte (must be 0x00)
	status 	|= _oledSendCommand(0xff);			// dummy byte (must be 0xff)

	status 	|= _oledSendCommand(OLED_EXP_ACTIVATE_SCROLL);

    return status;
}

// diagonal scrolling
//	direction 	scrolling
//	0 			left
//	1 			right
int oledScrollDiagonal (int direction, int scrollSpeed, int fixedRows, int scrollRows, int verticalOffset, int startPage, int stopPage)
{
	int 	status;
	int 	scrollMode;

	onionPrint(ONION_SEVERITY_INFO, "> Enabling diagonal scrolling to the %s\n", (direction == 1 ? "right" : "left") );

	// read the direction
	if (direction == 1) {
		scrollMode 	= OLED_EXP_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL;
	}
	else if (direction == 0) {
		scrollMode 	= OLED_EXP_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL;
	}

	//// send the commands
	// setup the vertical scrolling
	status 	=  _oledSendCommand(OLED_EXP_SET_VERTICAL_SCROLL_AREA);
	status 	|= _oledSendCommand(fixedRows);		// number of fixed rows
	status 	|= _oledSendCommand(scrollRows);	// number of rows in scroll area

	// setup the horizontal scrolling
	status 	|= _oledSendCommand(scrollMode);
	status 	|= _oledSendCommand(0x00);			// dummy byte
	status 	|= _oledSendCommand(startPage);		// start page addr (0 - 7)
	status 	|= _oledSendCommand(scrollSpeed);	// time interval between frames
	status 	|= _oledSendCommand(stopPage);		// end page addr (must be greater than start)
	status 	|= _oledSendCommand(verticalOffset);// number of rows to scroll by

	status 	|= _oledSendCommand(OLED_EXP_ACTIVATE_SCROLL);

    return status;
}

int oledScrollStop ()
{
	int 	status;

	onionPrint(ONION_SEVERITY_INFO, "> Disabling scrolling\n");

	// send the command
	status 	|= _oledSendCommand(OLED_EXP_DEACTIVATE_SCROLL);

    return status;
}

//// buffer functions ////
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

// Write a character to the buffer
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

