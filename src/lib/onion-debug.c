#include <onion-debug.h>


void onionSetVerbosity (int verbosityLevel)
{
	onionVerbosityLevel 	= verbosityLevel;
}

int onionGetVerbosity ()
{
	return (onionVerbosityLevel);
}

void onionPrint (int severity, const char* msg, ...)
{
	va_list 	argptr;	

	if (onionVerbosityLevel >= severity) {
		va_start(argptr, msg);
		vprintf(msg, argptr);
		va_end(argptr);
	}
}
