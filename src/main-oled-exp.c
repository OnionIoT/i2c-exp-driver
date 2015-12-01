#include <oled-exp.h>

void usage(const char* progName) 
{
	// to do: actually populate this

	onionPrint(ONION_SEVERITY_FATAL, "\n");
	onionPrint(ONION_SEVERITY_FATAL, "Usage: oled-exp -i\n");
	onionPrint(ONION_SEVERITY_FATAL, "\n");
	onionPrint(ONION_SEVERITY_FATAL, "FUNCTIONALITY:\n");
	onionPrint(ONION_SEVERITY_FATAL, "  Initialize the OLED Display\n");
	onionPrint(ONION_SEVERITY_FATAL, "\n\n");

	onionPrint(ONION_SEVERITY_FATAL, "Usage: oled-exp -c\n");
	onionPrint(ONION_SEVERITY_FATAL, "\n");
	onionPrint(ONION_SEVERITY_FATAL, "FUNCTIONALITY:\n");
	onionPrint(ONION_SEVERITY_FATAL, "  Clear the OLED Display\n");
	onionPrint(ONION_SEVERITY_FATAL, "\n\n");

	onionPrint(ONION_SEVERITY_FATAL, "Usage: oled-exp [-icqv] COMMAND PARAMETER\n");
	onionPrint(ONION_SEVERITY_FATAL, "\n");
	onionPrint(ONION_SEVERITY_FATAL, "The following COMMANDs are available:\n");
	onionPrint(ONION_SEVERITY_FATAL, "  power <on|off>                  Turn the display on or off\n");
	onionPrint(ONION_SEVERITY_FATAL, "  write <message>                 Write the input string on the display\n");
	onionPrint(ONION_SEVERITY_FATAL, "  dim <on|off>                    Adjust the screen brightness\n");
	onionPrint(ONION_SEVERITY_FATAL, "  invert <on|off>                 Invert the colors on the display\n");
	onionPrint(ONION_SEVERITY_FATAL, "  cursor <row>,<column>           Set the cursor to the specified row and column\n");
	onionPrint(ONION_SEVERITY_FATAL, "\n");
	onionPrint(ONION_SEVERITY_FATAL, "OPTIONS:\n");
	onionPrint(ONION_SEVERITY_FATAL, "  -i 		initialize display\n");
	onionPrint(ONION_SEVERITY_FATAL, "  -c 		clear the display\n");
	onionPrint(ONION_SEVERITY_FATAL, "  -q 		quiet: no output\n");
	onionPrint(ONION_SEVERITY_FATAL, "  -v 		verbose: lots of output\n");
	onionPrint(ONION_SEVERITY_FATAL, "  -h 		help: show this prompt\n");
	

	onionPrint(ONION_SEVERITY_FATAL, "\n");
}

int oledCommand(char *command, char *param)
{
	int 	status;
	int 	val0, val1;

	// perform the specified command
	onionPrint(ONION_SEVERITY_DEBUG_EXTRA, "command = '%s', param = '%s'\n", command, param);
	if (strcmp(command, "write") == 0 ) {
		oledWrite(param);
	}
	else if (strcmp(command, "contrast") == 0 ) {
		oledSetContrast( atoi(param) );
	}
	else if (strcmp(command, "invert") == 0 ) {
		// interpret the parameter
		val0 	= 0;	// off by default
		if (strcmp(param, "on") == 0 ) {
			val0 = 1;
		}
		oledSetDisplayMode( val0 );
	}
	else if (strcmp(command, "power") == 0 ) {
		// interpret the parameter
		val0 	= 0;	// off by default
		if (strcmp(param, "on") == 0 ) {
			val0 = 1;
		}
		oledSetDisplayPower(val0);
	}
	else if (strcmp(command, "dim") == 0 ) {
		// interpret the parameter
		val0 	= 0;	// off by default
		if (strcmp(param, "on") == 0 ) {
			val0 = 1;
		}
		oledSetDim(val0);
	}
	else if (strcmp(command, "cursor") == 0 ) {
		// interpret the parameter
		sscanf(param, "%d, %d", &val0, &val1);
		onionPrint(ONION_SEVERITY_INFO, "> Setting cursor to (%d, %d)\n", val0, val1);
		oledSetCursor(val0, val1);
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
	int 	clear;
	int 	ch;


	// set the defaults
	init 		= 0;
	clear 		= 0;
	verbose 	= ONION_VERBOSITY_NORMAL;

	command 	= malloc(255 * sizeof *command);
	param 		= malloc(255 * sizeof *param);

	// save the program name
	progname 	= argv[0];	


	//// parse the option arguments
	while ((ch = getopt(argc, argv, "vqxhic")) != -1) {
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
		case 'c':
			// perform clear sequence
			clear 	= 1;
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


	//// OLED PROGRAMMING
	// initialize display
	if ( init == 1 ) {
		status 	= oledDriverInit();
		if (status == EXIT_FAILURE) {
			onionPrint(ONION_SEVERITY_FATAL, "main-oled-exp:: display init failed!\n");
		}
	}

	// clear screen
	if ( clear == 1 ) {
		onionPrint(ONION_SEVERITY_INFO, "> Clearing display\n");
		status 	= oledClear();
		if (status == EXIT_FAILURE) {
			onionPrint(ONION_SEVERITY_FATAL, "main-oled-exp:: display clear failed!\n");
		}
	}


	// check if just option command
	if ( argc == 0 ) {
		// check if usage needs to be printed
		if ( init == 0 && clear == 0) {
			usage(progname);
		}
		return 0;
	}


	//// parse the command arguments
	while ( argc > 0 ) {
		// first arg - command
		strcpy(command, argv[0]);

		// second arg - parameter (optional)
		if ( argc > 1 ) {
			strcpy(param, argv[1]);
		}

		// perform the specified command
		status 	= oledCommand(command, param);
		if (status != EXIT_SUCCESS) {
			onionPrint(ONION_SEVERITY_FATAL, "ERROR: command '%s' failed!\n", command);
		}

		// decrement the number of arguments left
		argc	-= 2;
		argv	+= 2;

		onionPrint(ONION_SEVERITY_DEBUG, "> arguments remaining: %d\n", argc);
	}


	return 0;
}