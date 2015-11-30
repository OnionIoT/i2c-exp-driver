#include <oled-exp.h>

void usage(const char* progName) 
{
	// to do: actually populate this

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

int oledCommand(char *command, char *param)
{
	int 	status;
	int 	val;

	// perform the specified command
	onionPrint(ONION_SEVERITY_DEBUG, "command = '%s', param = '%s'\n", command, param);
	if (strcmp(command, "write") == 0 ) {
		oledWrite(param);
	}
	else if (strcmp(command, "contrast") == 0 ) {
		oledSetContrast( atoi(param) );
	}
	else if (strcmp(command, "invert") == 0 ) {
		oledSetDisplayMode( atoi(param) );
	}
	else if (strcmp(command, "power") == 0 ) {
		// interpret the parameter
		val 	= 0;	// off by default
		if (strcmp(param, "on") == 0 ) {
			val = 1;
		}
		oledSetDisplayPower(val);
	}
	else if (strcmp(command, "dim") == 0 ) {
		// interpret the parameter
		val 	= 0;	// off by default
		if (strcmp(param, "on") == 0 ) {
			val = 1;
		}
		oledSetDim(val);
	}
	else {
		onionPrint(ONION_SEVERITY_FATAL, "> Unrecognized command '%s'\n", command );
	}

	return status;
}


int main(int argc, char** argv)
{
	const char *progname;
	char 	*command;
	char 	*param;
	
	int 	status;
	int 	verbose;
	int 	init;
	int 	ch;


	// set the defaults
	init 		= 0;
	verbose 	= ONION_VERBOSITY_NORMAL;

	command 	= malloc(255 * sizeof *command);
	param 		= malloc(255 * sizeof *param);

	// save the program name
	progname 	= argv[0];	


	//// parse the option arguments
	while ((ch = getopt(argc, argv, "vqxhi")) != -1) {
		switch (ch) {
		case 'v':
			// verbose output
			verbose = ONION_VERBOSITY_VERBOSE;
			break;
		case 'q':
			// quiet output
			verbose = ONION_VERBOSITY_NONE;
			break;
		case 'x':
			// extra verbose output
			verbose = ONION_SEVERITY_DEBUG_EXTRA;
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

	// set the verbosity
	onionSetVerbosity(verbose);


	// advance past the option arguments
	argc 	-= optind;
	argv	+= optind;


	// check if just initialization
	if ( argc == 0 && init == 1 ) {
		status = oledDriverInit();
		if (status == EXIT_FAILURE) {
			onionPrint(ONION_SEVERITY_FATAL, "main-oled-exp:: oled init failed!\n");
		}
		return 0;
	}

	// ensure correct number of arguments
	if ( argc < 1) {
		usage(progname);
		onionPrint(ONION_SEVERITY_FATAL, "ERROR: invalid amount of arguments!\n");
		return 0;
	}

	//// parse the arguments
	// first arg - command
	strcpy(command, argv[0]);

	// second arg - parameter (optional)
	if ( argc > 1 ) {
		strcpy(param, argv[1]);
	}


	//// OLED PROGRAMMING
	// perform initialization
	if (init == 1) {
		status 	= oledDriverInit();
		if (status == EXIT_FAILURE) {
			onionPrint(ONION_SEVERITY_FATAL, "main-oled-exp:: relay init failed!\n");
		}
	}

	// perform the specified command
	status 	= oledCommand(command, param);
	if (status != EXIT_SUCCESS) {
		onionPrint(ONION_SEVERITY_FATAL, "ERROR: command '%s' failed!\n", command);
	}

	return 0;
}