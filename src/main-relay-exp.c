#include <relay-exp.h>

void usage(const char* progName) 
{
	printf("\n");
	printf("Usage: relay-exp -i\n");
	printf("\n");
	printf("FUNCTIONALITY:\n");
	printf("\tJust initialize the Relay chip\n");
	printf("\n\n");
	printf("Usage: relay-exp [-qvi] CHANNEL STATE\n");
	printf("\n");
	printf("CHANNEL is the specified Relay channel on the Expansion\n");
	printf("\tcan be: 0-1  to control a single channel\n");
	printf("\tcan be: 'all' to control all channels simultaneously\n");
	printf("STATE is the desired operational state of the relay\n");
	printf("\tcan be: 0  relay switch is OFF\n");
	printf("\tcan be: 1  relay switch is ON\n");
	printf("\n");
	printf("FUNCTIONALITY:\n");
	printf("\tProgram the CHANNEL to the specified relay state\n");
	printf("\n");
	printf("OPTIONS:\n");
	printf(" -q 		quiet: no output\n");
	printf(" -v 		verbose: lots of output\n");
	printf(" -h 		help: show this prompt\n");
	printf(" -i 		initialize the relay chip (must be done after power-up)\n");
	
	printf("\n");
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
		printf("ERROR: invalid CHANNEL selection\n");
		printf("Accepted values are:\n");
		printf("\t0-1\n");
		printf("\tall\n");
		printf("\n");

		status = EXIT_FAILURE;
	}

	if (state != 0 && state != 1) {
		printf("ERROR: invalid STATE selection\n");
		printf("Accepted values are:\n");
		printf("\t0 or 1\n");
		printf("\n");

		status = EXIT_FAILURE;
	}

	return status;
}


int main(int argc, char** argv)
{
	const char *progname;
	int status;
	int verbose 	= 1;
	int init 		= 0;
	int ch;

	int channel;
	int relayState;

	int bInitialized;


	// save the program name
	progname = argv[0];	

	//// parse the option arguments
	while ((ch = getopt(argc, argv, "vqhi")) != -1) {
		switch (ch) {
		case 'v':
			// verbose output
			verbose = 2;
			break;
		case 'q':
			// quiet output
			verbose = 0;
			break;
		case 'i':
			// perform init sequence
			init 	= 1;
			break;
		default:
			usage(progname);
			return 0;
		}
	}

	// advance past the option arguments
	argc 	-= optind;
	argv	+= optind;

	// check if just initialization
	if ( argc == 0 && init == 1 ) {
		status = relayDriverInit();
		if (status == EXIT_FAILURE) {
			printf("main-relay-exp:: relay init failed!\n");
		}
		return 0;
	}

	// ensure correct number of arguments
	if ( argc != 2) {
		usage(progname);
		printf("ERROR: invalid amount of arguments!\n");
		return 0;
	}


	//// parse the arguments
	// first arg - channel
	channel 	= readChannelArgument(argv[0]);

	// second arg - relay state (on or off)
	relayState 	= (int)strtol(argv[1], NULL, 10);

	// validate the arguments
	status 	= validateArguments(channel, relayState);
	if (status == EXIT_FAILURE) {
		return 0;
	}


	//// RELAY PROGRAMMING
	// check if initialized
	status = relayCheckInit(&bInitialized);

	// perform initialization
	if (init == 1 || bInitialized == 0) {
		status 	= relayDriverInit();
		if (status == EXIT_FAILURE) {
			printf("main-relay-exp:: relay init failed!\n");
		}
	}

	// set the relay state
	if (channel < 0) {
		// program both relays at once
		status 	= relaySetAllChannels(relayState);
		if (status == EXIT_FAILURE) {
			printf("main-pwm-exp:: all relay setup failed!\n");
		}
	}
	else {
		// program just one relay
		status 	= relaySetChannel(channel, relayState);
		if (status == EXIT_FAILURE) {
			printf("main-pwm-exp:: relay %d setup failed!\n", channel);
		}
	}


	return 0;
}