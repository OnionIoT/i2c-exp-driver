#include <oled-exp.h>

// function prototypes:
void 	Usage 				(const char* progName);
int 	oledCommand 		(char *command, char *param);

// print the usage info 
void usage(const char* progName) 
{
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
	onionPrint(ONION_SEVERITY_FATAL, "  scroll <direction>              Enable scrolling of screen content\n");
	onionPrint(ONION_SEVERITY_FATAL, "         available directions:    left, right, diagonal-left, diagonal-right\n");
	onionPrint(ONION_SEVERITY_FATAL, "         to stop scrolling:       stop\n");
	onionPrint(ONION_SEVERITY_FATAL, "  draw <lcd file>                 Draw the contents of an lcd file to the display\n");
	onionPrint(ONION_SEVERITY_FATAL, "\n");
	onionPrint(ONION_SEVERITY_FATAL, "COMMANDs can be cascaded one after another, they will execute in order.\n");
	onionPrint(ONION_SEVERITY_FATAL, "\n");

	onionPrint(ONION_SEVERITY_FATAL, "OPTIONS:\n");
	onionPrint(ONION_SEVERITY_FATAL, "  -i 		initialize display\n");
	onionPrint(ONION_SEVERITY_FATAL, "  -c 		clear the display\n");
	onionPrint(ONION_SEVERITY_FATAL, "  -q 		quiet: no output\n");
	onionPrint(ONION_SEVERITY_FATAL, "  -v 		verbose: lots of output\n");
	onionPrint(ONION_SEVERITY_FATAL, "  -h 		help: show this prompt\n");
	

	onionPrint(ONION_SEVERITY_FATAL, "\n");
}

// execute a specified command
int oledCommand(char *command, char *param)
{
	int 	status;
	int 	val0, val1;
	uint8_t	*buffer;

	// perform the specified command
	onionPrint(ONION_SEVERITY_DEBUG_EXTRA, "command = '%s', param = '%s'\n", command, param);
	if (strcmp(command, "write") == 0 ) {	
		status	= oledWrite(param);
	}
	else if (strcmp(command, "brightness") == 0 ) {
		status	= oledSetBrightness( atoi(param) );
	}
	else if (strcmp(command, "invert") == 0 ) {
		// interpret the parameter
		val0 	= 0;	// off by default
		if (strcmp(param, "on") == 0 ) {
			val0 = 1;
		}
		status	= oledSetDisplayMode( val0 );
	}
	else if (strcmp(command, "power") == 0 ) {
		// interpret the parameter
		val0 	= 0;	// off by default
		if (strcmp(param, "on") == 0 ) {
			val0 = 1;
		}
		status	= oledSetDisplayPower(val0);
	}
	else if (strcmp(command, "dim") == 0 ) {
		// interpret the parameter
		val0 	= 0;	// off by default
		if (strcmp(param, "on") == 0 ) {
			val0 = 1;
		}
		status	= oledSetDim(val0);
	}
	else if (strcmp(command, "cursor") == 0 ) {
		// interpret the parameter
		sscanf(param, "%d, %d", &val0, &val1);
		onionPrint(ONION_SEVERITY_INFO, "> Setting cursor to (%d, %d)\n", val0, val1);
		status 	= oledSetTextColumns();
		status	= oledSetCursor(val0, val1);
	}
	else if (strcmp(command, "draw") == 0 ) {
		// allocate memory for the buffer
		buffer 	= malloc(OLED_EXP_WIDTH*OLED_EXP_HEIGHT/8 * sizeof *buffer);

		// read the parameter
		if ( strncmp(param, OLED_EXP_READ_LCD_DATA_IDENTIFIER, strlen(OLED_EXP_READ_LCD_DATA_IDENTIFIER) ) == 0 ) {
			onionPrint(ONION_SEVERITY_INFO, "> Reading data from argument\n");

			onionPrint(ONION_SEVERITY_DEBUG_EXTRA, "  param length is %d\n", strlen(param) );
			// remove the data identifier from the string
			memmove	(	param, 
						param + strlen(OLED_EXP_READ_LCD_DATA_IDENTIFIER), 
						strlen(param) 
					);
			onionPrint(ONION_SEVERITY_DEBUG_EXTRA, "  after move: param length is %d\n", strlen(param) );

			// read the data into a buffer
			status 	= oledReadLcdData(param, buffer);
		}
		else {
			// read data from a file
			onionPrint(ONION_SEVERITY_INFO, "> Reading data from file '%s'\n", param);
			status 	= oledReadLcdFile(param, buffer);
		}

		if (status == EXIT_SUCCESS) {
			status	= oledDraw(buffer, OLED_EXP_WIDTH*OLED_EXP_HEIGHT/8);
		}
		else {
			onionPrint(ONION_SEVERITY_FATAL, "ERROR: Cannot draw invalid data\n");
		}

		// deallocate memory for the buffer
		if (buffer != NULL) {
			onionPrint(ONION_SEVERITY_DEBUG_EXTRA, "> Deallocating buffer array\n");
			free(buffer);
		}
	}
	else if (strcmp(command, "scroll") == 0 ) {
		// interpret the parameters
		val0 		= -1;
		val1 		= -1;
		if (strcmp(param, "left") == 0) {
			val0 	= 0;	// horizontal scrolling
			val1	= 0;	// scrolling left
		}
		else if (strcmp(param, "right") == 0) {
			val0 	= 0;	// horizontal scrolling
			val1	= 1;	// scrolling right
		}
		else if (strcmp(param, "diagonal-left") == 0) {
			val0 	= 1;	// vertical scrolling
			val1	= 0;	// scrolling up
		}
		else if (strcmp(param, "diagonal-right") == 0) {
			val0 	= 1;	// vertical scrolling
			val1	= 1;	// scrolling down
		}

		if (val0 == -1) {
			status 	= oledScrollStop();
		}
		else if (val0 == 0) {
			// horizontal scrolling
			status 	= oledScroll(val1, OLED_EXP_SCROLL_SPEED_5_FRAMES, 0, OLED_EXP_CHAR_ROWS-1);
		}
		else if (val0 == 1) {
			// diagonal scrolling
			status 	= oledScrollDiagonal (	val1, 								// direction
											OLED_EXP_SCROLL_SPEED_5_FRAMES, 	// scroll speed
											0, 									// # fixed rows
											OLED_EXP_HEIGHT, 					// # scrolling rows
											1, 									// rows to scroll by
											0, 									// horizontal start page
											OLED_EXP_CHAR_ROWS-1				// horizontal end page	
										);
		}
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
	while ((ch = getopt(argc, argv, "vqhic")) != -1) {
		switch (ch) {
		case 'v':
			// verbose output
			verbose++;
			break;
		case 'q':
			// quiet output
			verbose = ONION_VERBOSITY_NONE;
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