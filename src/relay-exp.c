#include <relay-exp.h>

// perform basic initialization of GPIO chip
int relayDriverInit (int addr)
{
	int status; 

	onionPrint(ONION_SEVERITY_INFO, "> Initializing Relay Expansion chip\n");

	// set all GPIOs to output
	status 	= mcp_setAllDirection(addr, 0);
	if (status == EXIT_FAILURE) {
		onionPrint(ONION_SEVERITY_FATAL, "relay-exp:init:: Setting output direction failed\n");
		return status;
	}

	// disable all pullup resistors
	status 	= mcp_setAllPullup(addr, 0);
	if (status == EXIT_FAILURE) {
		onionPrint(ONION_SEVERITY_FATAL, "relay-exp:init:: Disabling pullup resistors failed\n");
		return status;
	}

	// set all GPIOs to 0
	status 	= mcp_setAllGpio(addr, 0);
	if (status == EXIT_FAILURE) {
		onionPrint(ONION_SEVERITY_FATAL, "relay-exp:init:: Reseting GPIOs failed\n");
		return status;
	}

	return status;
}

// check if GPIO0 and GPIO1 are properly initialized
int relayCheckInit (int addr, int *bInitialized)
{
	int status;
	int dirGpio0, dirGpio1;
	int puGpio0, puGpio1;

	// set default to not initialized
	*bInitialized	= 0;

	// find GPIO0 direction
	status 	= mcp_getDirection(addr, RELAY_EXP_CHANNEL0, &dirGpio0);

	// exit with failure if read fails
	if (status != EXIT_SUCCESS) {
		return status;
	}

	// find GPIO1 direction
	status 	|= mcp_getDirection(addr, RELAY_EXP_CHANNEL1, &dirGpio1);


	// find status of GPIO0's pull-up resistor
	status 	|= mcp_getInputPolarity(addr, RELAY_EXP_CHANNEL0, &puGpio0);

	// find status of GPIO1's pull-up resistor
	status 	|= mcp_getInputPolarity(addr, RELAY_EXP_CHANNEL1, &puGpio1);


	// check for any initialization
	if 	( 	status == EXIT_SUCCESS &&
			dirGpio0 	== 0 && 
			dirGpio1 	== 0 && 
			puGpio0 	== 0 && 
			puGpio1 	== 0
		)
	{
		*bInitialized = 1;
	}

	return status;
}

// read GPIO calue, ie read the relay state
int relayReadState (int addr, int channel, int* state)
{
	int status;

	// read the relay state
	status 	= mcp_getGpio(addr, channel, state);
	onionPrint(ONION_SEVERITY_INFO, "> Reading RELAY%d state: %s\n", channel, ( *state == 1 ? "ON" : "OFF") );

	return status;
}

// set GPIO value - change the relay state
int relaySetChannel (int addr, int channel, int state)
{
	int status;

	// set the relay channel to the specified state
	onionPrint(ONION_SEVERITY_INFO, "> Setting RELAY%d to %s\n", channel, ( state == 1 ? "ON" : "OFF") );
	status = mcp_setGpio(addr, channel, state);

	return status;
}

// set value to both GPIO0 and GPIO1 (both relay states)
int relaySetAllChannels (int addr, int state)
{
	int status, val;

	onionPrint(ONION_SEVERITY_INFO, "> Setting both RELAYS to %s\n", ( state == 1 ? "ON" : "OFF") );

	// translate state to register value
	if (state == 0) {
		val = 0x0;
	}
	else {
		val = 0x3;	// enable GPIO0 and GPIO1
	}

	// set the all relay channels to the specified state
	status = mcp_setAllGpio(addr, val);

	return status;
}



