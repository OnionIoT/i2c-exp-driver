#include <pwm-exp.h>

typedef enum e_PwmExpMode {
	MAIN_PWM_EXP_DUTY_MODE 		= 0,
	MAIN_PWM_EXP_PERIOD_MODE 	= 1,
	MAIN_PWM_EXP_SLEEP_MODE		= 2
}	eProgramMode;

void usage(const char* progName) 
{
	onionPrint(ONION_SEVERITY_FATAL, "\n");
	onionPrint(ONION_SEVERITY_FATAL, "Usage: pwm-exp -i\n");
	onionPrint(ONION_SEVERITY_FATAL, "\n");
	onionPrint(ONION_SEVERITY_FATAL, "FUNCTIONALITY:\n");
	onionPrint(ONION_SEVERITY_FATAL, "\tOnly initialize the pwm chip\n");
	onionPrint(ONION_SEVERITY_FATAL, "\n\n");
	onionPrint(ONION_SEVERITY_FATAL, "Usage: pwm-exp [-qvi] [-f <freq>] CHANNEL DUTY [DELAY]\n");
	onionPrint(ONION_SEVERITY_FATAL, "\n");
	onionPrint(ONION_SEVERITY_FATAL, "CHANNEL is the specified PWM channel on the Expansion\n");
	onionPrint(ONION_SEVERITY_FATAL, "\tcan be: 0-15  to control a single channel\n");
	onionPrint(ONION_SEVERITY_FATAL, "\tcan be: 'all' to control all channels simultaneously\n");
	onionPrint(ONION_SEVERITY_FATAL, "DUTY is the signal duty cycle, expressed 1-100\n");
	onionPrint(ONION_SEVERITY_FATAL, "DELAY is the delay before signal asserts, optional\n");
	onionPrint(ONION_SEVERITY_FATAL, "\n");
	onionPrint(ONION_SEVERITY_FATAL, "FUNCTIONALITY:\n");
	onionPrint(ONION_SEVERITY_FATAL, "\tProgram the CHANNEL to the specified duty cycle\n");
	onionPrint(ONION_SEVERITY_FATAL, "\n");
	onionPrint(ONION_SEVERITY_FATAL, "OPTIONS:\n");
	onionPrint(ONION_SEVERITY_FATAL, " -q 		quiet: no output\n");
	onionPrint(ONION_SEVERITY_FATAL, " -v 		verbose: lots of output\n");
	onionPrint(ONION_SEVERITY_FATAL, " -h 		help: show this prompt\n");
	onionPrint(ONION_SEVERITY_FATAL, " -i 		initialize the pwm chip and oscillator\n");
	onionPrint(ONION_SEVERITY_FATAL, " -s 		put oscillator into sleep mode, disabling all pwm signals\n");
	onionPrint(ONION_SEVERITY_FATAL, " -f <freq>	set pwm signal frequency\n");
	onionPrint(ONION_SEVERITY_FATAL, "\n\n");
	onionPrint(ONION_SEVERITY_FATAL, "Usage: pwm-exp -p CHANNEL PULSE_WIDTH TOTAL_PERIOD\n");
	onionPrint(ONION_SEVERITY_FATAL, "\n");
	onionPrint(ONION_SEVERITY_FATAL, "CHANNEL is the specified PWM channel on the Expansion\n");
	onionPrint(ONION_SEVERITY_FATAL, "PULSE_WIDTH is the time in ms while the pulse is high\n");
	onionPrint(ONION_SEVERITY_FATAL, "TOTAL_PERIOD is the total period in ms\n");
	
	onionPrint(ONION_SEVERITY_FATAL, "\n");
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
		onionPrint(ONION_SEVERITY_FATAL, "ERROR: invalid CHANNEL selection\n");
		onionPrint(ONION_SEVERITY_FATAL, "Accepted values are:\n");
		onionPrint(ONION_SEVERITY_FATAL, "\t0-15\n");
		onionPrint(ONION_SEVERITY_FATAL, "\tall\n");
		onionPrint(ONION_SEVERITY_FATAL, "\n");

		status = EXIT_FAILURE;
	}

	return status;
}

int validateArgumentsDutyMode(int channel, float duty, float delay) 
{
	int status = EXIT_SUCCESS;

	status = validateChannelArgument(channel);

	if (duty < 0.0f || duty > 100.0f) {
		onionPrint(ONION_SEVERITY_FATAL, "ERROR: invalid DUTY selection\n");
		onionPrint(ONION_SEVERITY_FATAL, "Accepted values are:\n");
		onionPrint(ONION_SEVERITY_FATAL, "\t0 - 100\n");
		onionPrint(ONION_SEVERITY_FATAL, "\n");

		status = EXIT_FAILURE;
	}

	if (delay < 0.0f || delay > 100.0f) {
		onionPrint(ONION_SEVERITY_FATAL, "ERROR: invalid DELAY selection\n");
		onionPrint(ONION_SEVERITY_FATAL, "Accepted values are:\n");
		onionPrint(ONION_SEVERITY_FATAL, "\t0 - 100\n");
		onionPrint(ONION_SEVERITY_FATAL, "\n");

		status = EXIT_FAILURE;
	}

	return status;
}

int validateArgumentsPeriodMode(int channel, float periodOn, float periodTotal, float frequency) 
{
	int status = EXIT_SUCCESS;

	status = validateChannelArgument(channel);

	if (periodOn < 0.0f || periodOn > periodTotal) {
		onionPrint(ONION_SEVERITY_FATAL, "ERROR: invalid PERIOD_ON selection\n");
		onionPrint(ONION_SEVERITY_FATAL, "Must fulfill the following conditions:\n");
		onionPrint(ONION_SEVERITY_FATAL, "\tbe greater than 0\n");
		onionPrint(ONION_SEVERITY_FATAL, "\tbe less than or equal to the total period\n");
		onionPrint(ONION_SEVERITY_FATAL, "\n");

		status = EXIT_FAILURE;
	}

	if (periodTotal < 0.0f) {
		onionPrint(ONION_SEVERITY_FATAL, "ERROR: invalid PERIOD_TOTAL selection\n");
		onionPrint(ONION_SEVERITY_FATAL, "Must be greater than 0\n");
		onionPrint(ONION_SEVERITY_FATAL, "\n");

		status = EXIT_FAILURE;
	}

	if (frequency < PWM_FREQUENCY_MIN || frequency > PWM_FREQUENCY_MAX) {
		onionPrint(ONION_SEVERITY_FATAL, "ERROR: invalid PERIOD_TOTAL selection\n");
		onionPrint(ONION_SEVERITY_FATAL, "Period corresponds to frequency of %0.2f Hz\n", frequency);
		onionPrint(ONION_SEVERITY_FATAL, "Period must correspond to frequency between:\n");
		onionPrint(ONION_SEVERITY_FATAL, "\t%d Hz - %d Hz", (int)PWM_FREQUENCY_MIN, (int)PWM_FREQUENCY_MAX);
		onionPrint(ONION_SEVERITY_FATAL, "\n");

		status = EXIT_FAILURE;
	}

	return status;
}

int main(int argc, char** argv)
{
	const char 	*progname;
	int 		status;
	int 		mode;
	int 		verbose;
	int 		init;
	int 		ch;

	int 		bInitialized;
	int 		channel;
	float 		duty, delay, frequency;
	float 		periodOn, periodTotal;

	// set the defaults
	init 		= 0;
	verbose 	= ONION_VERBOSITY_NORMAL;
	mode 		= MAIN_PWM_EXP_DUTY_MODE;

	frequency 	= PWM_FREQUENCY_DEFAULT;
	delay 		= 0.0f;	// default value

	// save the program name
	progname = argv[0];	

	//// parse the option arguments
	while ((ch = getopt(argc, argv, "vqhipsf:")) != -1) {
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
		case 'p':
			// enable period mode
			mode 	= MAIN_PWM_EXP_PERIOD_MODE;
			break;
		case 'f':
			// specify the pwm frequency
			frequency = atof(optarg);
			break;
		case 's':
			// enable sleep mode
			mode  	= MAIN_PWM_EXP_SLEEP_MODE;
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
		status = pwmDriverInit();
		if (status == EXIT_FAILURE) {
			onionPrint(ONION_SEVERITY_FATAL, "main-pwm-exp:: pwm init failed!\n");
		}
		return 0;
	}

	// check if setting sleep mode
	if (mode == MAIN_PWM_EXP_SLEEP_MODE) {
		status = pwmDisableChip();
		if (status == EXIT_FAILURE) {
			onionPrint(ONION_SEVERITY_FATAL, "main-pwm-exp:: pwm chip disable failed!\n");
		}
		return 0;
	}



	//// parse the real arguments
	if (mode == MAIN_PWM_EXP_DUTY_MODE)
	{
		// ensure correct number of arguments
		if ( argc != 2 && argc != 3 ) {
			usage(progname);
			onionPrint(ONION_SEVERITY_FATAL, "ERROR: invalid amount of arguments!\n");
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
			onionPrint(ONION_SEVERITY_FATAL, "ERROR: invalid amount of arguments!\n");
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



	//// PWM PROGRAMMING
	// check if initialized
	status 	= pwmCheckInit(&bInitialized);

	// exit the app if i2c reads fail
	if (status == EXIT_FAILURE) {
		onionPrint(ONION_SEVERITY_FATAL, "> ERROR: PWM Expansion not found!\n");
		return 0;
	}


	// perform initialization
	if (init == 1 || bInitialized == 0) {
		status = pwmDriverInit();
		if (status == EXIT_FAILURE) {
			onionPrint(ONION_SEVERITY_FATAL, "main-pwm-exp:: pwm init failed!\n");
		}
	}

	// setup the frequency
	status = pwmSetFrequency(frequency);
	if (status == EXIT_FAILURE) {
		onionPrint(ONION_SEVERITY_FATAL, "main-pwm-exp:: pwm set frequency failed!\n");
	}

	// setup the driver
	status = pwmSetupDriver(channel, duty, delay);
	if (status == EXIT_FAILURE) {
		onionPrint(ONION_SEVERITY_FATAL, "main-pwm-exp:: driver setup failed!\n");
	}


	return 0;
}