#include <oled-exp.h>

// Send command byte to OLED Expansion
int _oledSendCommand (int command)
{
	int controlAddr = 0x40;
	i2c_writeBytes(OLED_EXP_DEVICE_NUM, OLED_EXP_ADDR, controlAddr, command, 1);
}

// Initialize the OLED Expansion
int oledDriverSetup ()
{
	// Initialize the screen for 128x64
	_oledSendCommand(OLED_EXP_DISPLAY_OFF);
	_oledSendCommand(OLED_EXP_SET_DISPLAY_CLOCK_DIV);
	_oledSendCommand(0x80); // The suggested ratio 0x80
	_oledSendCommand(OLED_EXP_SET_MULTIPLEX);
	_oledSendCommand(0x3F);
	_oledSendCommand(OLED_EXP_SET_DISPLAY_OFFSET);
}
