#include <pwm-exp.h>

void usage(const char* progName) {
	printf("\n");
	printf("Usage: pwm-exp [-qv] CHANNEL DUTY [DELAY]\n");
	printf("\n");
	printf("CHANNEL is the specified PWM channel on the Expansion\n");
	printf("DUTY is the signal duty cycle, expressed 1-100\n");
	printf("DELAY is the delay before signal asserts, optional\n");
	printf("\n");
	printf("OPTIONS:\n");
	printf(" -q 		quiet: no output\n");
	printf(" -v 		verbose: lots of output\n");
	printf(" -h 		help: show this prompt\n");
	printf("\n");
}

int main(int argc, char** argv)
{
	const char *progname;
	int status;
	int verbose 	= 1;
	int ch;

	int channel, duty, delay;

	progname = argv[0];

	// parse the option arguments
	while ((ch = getopt(argc, argv, "vqh")) != -1) {
		switch (ch) {
		case 'v':
			// verbose output
			verbose = 2;
			break;
		case 'q':
			// quiet output
			verbose = 0;
			break;
		default:
			usage(progname);
			return 0;
		}
	}

	// advance past the option arguments
	argc 	-= optind;
	argv	+= optind;

	// ensure correct number of arguments
	if ( argc != 2 && argc != 3 ) {
		usage(progname);
		return 0;
	}

	//// parse the arguments
	channel 	= (int)strtol(argv[0], NULL, 10);
	duty 		= (int)strtol(argv[1], NULL, 10);
	delay 		= 0;	// default value

	if (argc == 3) {
		delay 	= (int)strtol(argv[2], NULL, 10);
	}


	// setup the driver
	pwmSetupDriver(channel, duty, delay);


	return 0;
}