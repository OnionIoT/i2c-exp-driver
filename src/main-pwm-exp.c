#include <pwm-exp.h>

void usage(const char* progName) 
{
	printf("\n");
	printf("Usage: pwm-exp [-qvi] CHANNEL DUTY [DELAY]\n");
	printf("\n");
	printf("CHANNEL is the specified PWM channel on the Expansion\n");
	printf("\tcan be: 0-15  to control a single channel\n");
	printf("\tcan be: 'all' to control all channels simultaneously\n");
	printf("DUTY is the signal duty cycle, expressed 1-100\n");
	printf("DELAY is the delay before signal asserts, optional\n");
	printf("\n");
	printf("FUNCTIONALITY:\n");
	printf("\tProgram the CHANNEL to the specified duty cycle\n");
	printf("\n");
	printf("OPTIONS:\n");
	printf(" -q 		quiet: no output\n");
	printf(" -v 		verbose: lots of output\n");
	printf(" -h 		help: show this prompt\n");
	printf(" -i 		initialize the pwm chip (must be done after power-up)\n");
	printf("\n\n");
	printf("Usage: pwm-exp -i\n");
	printf("\n");
	printf("FUNCTIONALITY:\n");
	printf("\tOnly initialize the pwm chip\n");
	printf("\n");
	printf("\n");
}

int validateArguments(int channel, int duty, int delay) 
{
	int status = EXIT_SUCCESS;

	if (channel < -1 || channel > 15) {
		printf("ERROR: invalid CHANNEL selection\n");
		printf("Accepted values are:\n");
		printf("\t0-15\n");
		printf("\tall\n");
		printf("\n");

		status = EXIT_FAILURE;
	}

	if (duty < 0 || duty > 100) {
		printf("ERROR: invalid DUTY selection\n");
		printf("Accepted values are:\n");
		printf("\t0 - 100\n");
		printf("\n");

		status = EXIT_FAILURE;
	}

	if (delay < 0 || delay > 100) {
		printf("ERROR: invalid DELAY selection\n");
		printf("Accepted values are:\n");
		printf("\t0 - 100\n");
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
	float duty, delay, frequency;

	// set the default frequency
	frequency = PWM_DEFAULT_FREQUENCY;

	// save the program name
	progname = argv[0];	

	//// parse the option arguments
	while ((ch = getopt(argc, argv, "vqhif:")) != -1) {
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
			// perform PWM init
			init 	= 1;
			break;
		case 'f':
			// specify the pwm frequency
			frequency = atof(optarg);
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
	if ( argc == 0 && init == 1 ) {
		status = pwmDriverInit();
		if (status == EXIT_FAILURE) {
			printf("main:: pwm init failed!\n");
		}
		return 0;
	}
	else if ( argc != 2 && argc != 3 ) {
		usage(progname);
		return 0;
	}


	//// parse the arguments
	// first arg - channel
	if (strcmp(argv[0], "all") == 0 ) {
		channel = -1;	// all drivers
	}
	else {
		channel 	= (int)strtol(argv[0], NULL, 10);
	}

	// second arg - duty cycle
	duty 		= strtof(argv[1], NULL);

	// third arg, optional - delay value 
	delay 		= 0.0f;	// default value
	if (argc == 3) {
		delay 	= strtof(argv[2], NULL);
	}

	// validate the arguments
	status 	= validateArguments(channel, duty, delay);
	if (status == EXIT_FAILURE) {
		return 0;
	}



	//// PWM
	// perform initialization
	if (init == 1) {
		status = pwmDriverInit();
		if (status == EXIT_FAILURE) {
			printf("main:: pwm init failed!\n");
		}
	}

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