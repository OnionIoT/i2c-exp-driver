#include <pwm-exp.h>

typedef enum e_PwmExpMode {
	MAIN_PWM_EXP_DUTY_MODE = 0,
	MAIN_PWM_EXP_PERIOD_MODE = 1
}	eProgramMode;

void usage(const char* progName) 
{
	printf("\n");
	printf("Usage: pwm-exp -i\n");
	printf("\n");
	printf("FUNCTIONALITY:\n");
	printf("\tOnly initialize the pwm chip\n");
	printf("\n\n");
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
	printf("Usage: pwm-exp -p CHANNEL PULSE_WIDTH TOTAL_PERIOD\n");
	printf("\n");
	printf("CHANNEL is the specified PWM channel on the Expansion\n");
	printf("PULSE_WIDTH is the time in ms while the pulse is high\n");
	printf("TOTAL_PERIOD is the total period in ms\n");
	
	printf("\n");
}

int readChannelArgument (char* channelArgument) 
{
	int channel 	= PWM_EXP_NUM_CHANNELS;		// default value is invalid
	
	if (strcmp(channelArgument, "all") == 0 ) {
		channel 	= -1;	// all drivers
	}
	else {
		channel 	= (int)strtol(channelArgument, NULL, 10);
	}

	return channel;
}

int validateChannelArgument (int channel)
{
	int status = EXIT_SUCCESS;

	if (channel < -1 || channel >= PWM_EXP_NUM_CHANNELS) {
		printf("ERROR: invalid CHANNEL selection\n");
		printf("Accepted values are:\n");
		printf("\t0-15\n");
		printf("\tall\n");
		printf("\n");

		status = EXIT_FAILURE;
	}

	return status;
}

int validateArgumentsDutyMode(int channel, float duty, float delay) 
{
	int status = EXIT_SUCCESS;

	status = validateChannelArgument(channel);

	if (duty < 0.0f || duty > 100.0f) {
		printf("ERROR: invalid DUTY selection\n");
		printf("Accepted values are:\n");
		printf("\t0 - 100\n");
		printf("\n");

		status = EXIT_FAILURE;
	}

	if (delay < 0.0f || delay > 100.0f) {
		printf("ERROR: invalid DELAY selection\n");
		printf("Accepted values are:\n");
		printf("\t0 - 100\n");
		printf("\n");

		status = EXIT_FAILURE;
	}

	return status;
}

int validateArgumentsPeriodMode(int channel, float periodOn, float periodTotal, float frequency) 
{
	int status = EXIT_SUCCESS;

	status = validateChannelArgument(channel);

	if (periodOn < 0.0f || periodOn > periodTotal) {
		printf("ERROR: invalid PERIOD_ON selection\n");
		printf("Must fulfill the following conditions:\n");
		printf("\tbe greater than 0\n");
		printf("\tbe less than or equal to the total period\n");
		printf("\n");

		status = EXIT_FAILURE;
	}

	if (periodTotal < 0.0f) {
		printf("ERROR: invalid PERIOD_TOTAL selection\n");
		printf("Must be greater than 0\n");
		printf("\n");

		status = EXIT_FAILURE;
	}

	if (frequency < PWM_FREQUENCY_MIN || frequency > PWM_FREQUENCY_MAX) {
		printf("ERROR: invalid PERIOD_TOTAL selection\n");
		printf("Period corresponds to frequency of %0.2f Hz\n", frequency);
		printf("Period must correspond to frequency between:\n");
		printf("\t%d Hz - %d Hz", (int)PWM_FREQUENCY_MIN, (int)PWM_FREQUENCY_MAX);
		printf("\n");

		status = EXIT_FAILURE;
	}

	return status;
}

int main(int argc, char** argv)
{
	const char *progname;
	int status;
	int mode 		= MAIN_PWM_EXP_DUTY_MODE;
	int verbose 	= 1;
	int init 		= 0;
	int ch;

	int channel;
	float duty, delay, frequency;
	float periodOn, periodTotal;

	// set the defaults
	frequency 	= PWM_FREQUENCY_DEFAULT;
	delay 		= 0.0f;	// default value

	// save the program name
	progname = argv[0];	

	//// parse the option arguments
	while ((ch = getopt(argc, argv, "vqhipf:")) != -1) {
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
		case 'p':
			// enabled period mode
			mode 	= MAIN_PWM_EXP_PERIOD_MODE;
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

	// check if just initialization
	if ( argc == 0 && init == 1 ) {
		status = pwmDriverInit();
		if (status == EXIT_FAILURE) {
			printf("main-pwm-exp:: pwm init failed!\n");
		}
		return 0;
	}

	if (mode == MAIN_PWM_EXP_DUTY_MODE)
	{
		// ensure correct number of arguments
		if ( argc != 2 && argc != 3 ) {
			usage(progname);
			printf("ERROR: invalid amount of arguments!\n");
			return 0;
		}


		//// parse the arguments
		// first arg - channel
		channel 	= readChannelArgument(argv[0]);

		// second arg - duty cycle
		duty 		= strtof(argv[1], NULL);

		// third arg, optional - delay value 
		if (argc == 3) {
			delay 	= strtof(argv[2], NULL);
		}

		// validate the arguments
		status 	= validateArgumentsDutyMode(channel, duty, delay);
		if (status == EXIT_FAILURE) {
			return 0;
		}
	}
	else if (mode == MAIN_PWM_EXP_PERIOD_MODE)
	{
		// ensure correct number of arguments
		if ( argc != 3 ) {
			usage(progname);
			printf("ERROR: invalid amount of arguments!\n");
			return 0;
		}


		//// parse the arguments
		// first arg - channel
		channel 	= readChannelArgument(argv[0]);

		// second arg - pulse width
		periodOn	= strtof(argv[1], NULL);

		// third arg - total period
		periodTotal	= strtof(argv[2], NULL);

		// convert periods to duty
		duty 		= (periodOn / periodTotal) * 100.0f;

		// convert total period to frequency
		frequency 	= ( 1.0f / (periodTotal / 1000.0f) );

		// validate the arguments
		status 	= validateArgumentsPeriodMode(channel, periodOn, periodTotal, frequency);
		if (status == EXIT_FAILURE) {
			return 0;
		}
	}



	//// PWM
	// perform initialization
	if (init == 1) {
		status = pwmDriverInit();
		if (status == EXIT_FAILURE) {
			printf("main-pwm-exp:: pwm init failed!\n");
		}
	}

	// setup the frequency
	status = pwmSetFrequency(frequency);
	if (status == EXIT_FAILURE) {
		printf("main-pwm-exp:: pwm set frequency failed!\n");
	}

	// setup the driver
	status = pwmSetupDriver(channel, duty, delay);
	if (status == EXIT_FAILURE) {
		printf("main-pwm-exp:: driver setup failed!\n");
	}


	return 0;
}