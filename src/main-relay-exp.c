#include <relay-exp.h>

void usage(const char* progName) 
{
	onionPrint(ONION_SEVERITY_FATAL, "\n");
	onionPrint(ONION_SEVERITY_FATAL, "Usage: relay-exp -i\n");
	onionPrint(ONION_SEVERITY_FATAL, "\n");
	onionPrint(ONION_SEVERITY_FATAL, "FUNCTIONALITY:\n");
	onionPrint(ONION_SEVERITY_FATAL, "\tJust initialize the Relay chip\n");
	onionPrint(ONION_SEVERITY_FATAL, "\n\n");
	onionPrint(ONION_SEVERITY_FATAL, "Usage: relay-exp [-qvi] [-s <bbb>] CHANNEL STATE\n");
	onionPrint(ONION_SEVERITY_FATAL, "\n");
	onionPrint(ONION_SEVERITY_FATAL, "CHANNEL is the specified Relay channel on the Expansion\n");
	onionPrint(ONION_SEVERITY_FATAL, "\tcan be: 0-1  to control a single channel\n");
	onionPrint(ONION_SEVERITY_FATAL, "\tcan be: 'all' to control all channels simultaneously\n");
	onionPrint(ONION_SEVERITY_FATAL, "STATE is the desired operational state of the relay\n");
	onionPrint(ONION_SEVERITY_FATAL, "\tcan be: 0  relay switch is OFF\n");
	onionPrint(ONION_SEVERITY_FATAL, "\tcan be: 1  relay switch is ON\n");
	onionPrint(ONION_SEVERITY_FATAL, "\n");
	onionPrint(ONION_SEVERITY_FATAL, "FUNCTIONALITY:\n");
	onionPrint(ONION_SEVERITY_FATAL, "\tProgram the CHANNEL to the specified relay state\n");
	onionPrint(ONION_SEVERITY_FATAL, "\n");
	onionPrint(ONION_SEVERITY_FATAL, "OPTIONS:\n");
	onionPrint(ONION_SEVERITY_FATAL, " -q 		quiet: no output\n");
	onionPrint(ONION_SEVERITY_FATAL, " -v 		verbose: lots of output\n");
	onionPrint(ONION_SEVERITY_FATAL, " -h 		help: show this prompt\n");
	onionPrint(ONION_SEVERITY_FATAL, " -i 		initialize the relay chip\n");
	onionPrint(ONION_SEVERITY_FATAL, " -s <bbb>	dip-switch configuration in binary, not required if 000\n");
	
	onionPrint(ONION_SEVERITY_FATAL, "\n");
}

// flip single char, check for valid input
int processSingleSwitch(char* out, char in)
{
	int status 	= EXIT_FAILURE;

	if (in == '0') {
		*out 	= '1';
		status 	= EXIT_SUCCESS;
	}
	else if (in == '1') {
		*out 	= '0';
		status 	= EXIT_SUCCESS;
	}

	return status;
}

// process switch input into addr to be used by MCP chip
//	flip the bits (bit0=bit2, bit1=bit1, bit2=bit0)
// 	invert the bits
// 	convert to int
int processSwitchAddr(char* addrIn, int* addrOut)
{
	char* 	output;
	int 	status;

	// check the input
	if (strlen(addrIn) != 3) {
		return EXIT_FAILURE;
	}

	// initialize the output
	output 	= (char*)malloc(sizeof(char) * 3);
	strcpy(output, RELAY_EXP_ADDR_SWITCH_DEFAULT_VAL);

	// perform the flip
	status 	= processSingleSwitch(&output[0], addrIn[2]);
	status 	|= processSingleSwitch(&output[1], addrIn[1]);
	status 	|= processSingleSwitch(&output[2], addrIn[0]);

	// convert the string to an integer
	*addrOut 	= (int)strtol(output, NULL, 2);

	// free the allocated memory
	free(output);

	return status;
}

int readChannelArgument (char* channelArgument) 
{
	int channel 	= RELAY_EXP_NUM_CHANNELS;		// default value is invalid
	
	if (strcmp(channelArgument, "all") == 0 ) {
		channel 	= -1;	// all drivers
	}
	else {
		channel 	= (int)strtol(channelArgument, NULL, 10);
	}

	return channel;
}

int validateArguments(int channel, int state)
{
	int status = EXIT_SUCCESS;

	if (channel < -1 || channel >= RELAY_EXP_NUM_CHANNELS) {
		onionPrint(ONION_SEVERITY_FATAL, "ERROR: invalid CHANNEL selection\n");
		onionPrint(ONION_SEVERITY_FATAL, "Accepted values are:\n");
		onionPrint(ONION_SEVERITY_FATAL, "\t0-1\n");
		onionPrint(ONION_SEVERITY_FATAL, "\tall\n");
		onionPrint(ONION_SEVERITY_FATAL, "\n");

		status = EXIT_FAILURE;
	}

	if (state != 0 && state != 1) {
		onionPrint(ONION_SEVERITY_FATAL, "ERROR: invalid STATE selection\n");
		onionPrint(ONION_SEVERITY_FATAL, "Accepted values are:\n");
		onionPrint(ONION_SEVERITY_FATAL, "\t0 or 1\n");
		onionPrint(ONION_SEVERITY_FATAL, "\n");

		status = EXIT_FAILURE;
	}

	return status;
}


int main(int argc, char** argv)
{
	const char *progname;
	char 	*switchAddr;
	
	int 	status;
	int 	verbose 	= ONION_SEVERITY_INFO;
	int 	init 		= 0;
	int 	ch;

	int 	channel;
	int 	relayState;

	int 	devAddr;
	int 	bInitialized;


	// save the program name
	progname 	= argv[0];	

	// set defaults
	switchAddr 	= malloc(RELAY_EXP_ADDR_SWITCH_NUM * sizeof *switchAddr);
	strcpy(switchAddr, RELAY_EXP_ADDR_SWITCH_DEFAULT_VAL);

	//// parse the option arguments
	while ((ch = getopt(argc, argv, "vqhis:")) != -1) {
		switch (ch) {
		case 'v':
			// verbose output
			verbose = ONION_SEVERITY_DEBUG;
			break;
		case 'q':
			// quiet output
			verbose = ONION_SEVERITY_FATAL;
			break;
		case 'i':
			// perform init sequence
			init 	= 1;
			break;
		case 's':
			// capture binary 
			strcpy (switchAddr, optarg);
			break;
		default:
			usage(progname);
			return 0;
		}
	}

	// set the verbosity
	onionSetVerbosity(verbose);


	// advance past the option arguments
	argc 	-= optind;
	argv	+= optind;


	// process the switch address
	status = processSwitchAddr(switchAddr, &devAddr);
	if (status == EXIT_FAILURE) {
		usage(progname);
		onionPrint(ONION_SEVERITY_FATAL, "ERROR: invalid switch address argument!\n");
		return 0;
	}
	if (strcmp(switchAddr, RELAY_EXP_ADDR_SWITCH_DEFAULT_VAL) != 0) {
		onionPrint(ONION_SEVERITY_INFO, "> Switch configuration: %s\n", switchAddr);
	}


	// check if just initialization
	if ( argc == 0 && init == 1 ) {
		status = relayDriverInit(devAddr);
		if (status == EXIT_FAILURE) {
			onionPrint(ONION_SEVERITY_FATAL, "main-relay-exp:: relay init failed!\n");
		}
		return 0;
	}

	// ensure correct number of arguments
	if ( argc != 2) {
		usage(progname);
		onionPrint(ONION_SEVERITY_FATAL, "ERROR: invalid amount of arguments!\n");
		return 0;
	}


	//// parse the arguments
	// first arg - channel
	channel 	= readChannelArgument(argv[0]);

	// second arg - relay state (on or off)
	if 	(	strcmp(argv[1], "off") == 0 ||
			strcmp(argv[1], "Off") == 0 ||
			strcmp(argv[1], "OFF") == 0
		) 
	{
		relayState 	= 0;
	}
	else if (	strcmp(argv[1], "on") == 0 ||
				strcmp(argv[1], "On") == 0 ||
				strcmp(argv[1], "ON") == 0
		) 
	{
		relayState 	= 1;
	}
	else {
		relayState 	= (int)strtol(argv[1], NULL, 10);
	}

	// validate the arguments
	status 	= validateArguments(channel, relayState);
	if (status == EXIT_FAILURE) {
		return 0;
	}


	//// RELAY PROGRAMMING
	// check if initialized
	status = relayCheckInit(devAddr, &bInitialized);

	// perform initialization
	if (init == 1 || bInitialized == 0) {
		status 	= relayDriverInit(devAddr);
		if (status == EXIT_FAILURE) {
			onionPrint(ONION_SEVERITY_FATAL, "main-relay-exp:: relay init failed!\n");
		}
	}

	// set the relay state
	if (channel < 0) {
		// program both relays at once
		status 	= relaySetAllChannels(devAddr, relayState);
		if (status == EXIT_FAILURE) {
			onionPrint(ONION_SEVERITY_FATAL, "main-relay-exp:: all relay setup failed!\n");
		}
	}
	else {
		// program just one relay
		status 	= relaySetChannel(devAddr, channel, relayState);
		if (status == EXIT_FAILURE) {
			onionPrint(ONION_SEVERITY_FATAL, "main-relay-exp:: relay %d setup failed!\n", channel);
		}
	}


	return 0;
}