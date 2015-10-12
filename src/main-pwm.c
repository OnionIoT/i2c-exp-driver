#include <errno.h>
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

	int channel, duty, delay, frequency;

	// set the default frequency
	frequency = PWM_DEFAULT_FREQUENCY;

	// save the program name
	progname = argv[0];	

	// parse the option arguments
	while ((ch = getopt(argc, argv, "vqhf:")) != -1) {
		switch (ch) {
		case 'v':
			// verbose output
			verbose = 2;
			break;
		case 'q':
			// quiet output
			verbose = 0;
			break;
		case 'f':
			// specify the pwm frequency
			frequency = atoi(optarg);
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

	//// PWM
	// setup the frequency
	status = pwmSetFrequency(frequency);
	if (status == EXIT_FAILURE) {
		printf("main:: pwm set frequency failed!\n");
	}

	// setup the driver
	status = pwmSetupDriver(channel, duty, delay);
	if (status == EXIT_FAILURE) {
		printf("main:: driver setup failed!\n");
	}


	return 0;
}