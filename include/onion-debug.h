#ifndef _ONION_DEBUG_H_
#define _ONION_DEBUG_H_

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define ONION_SEVERITY_FATAL			(-1)
#define ONION_SEVERITY_INFO				(0)
#define ONION_SEVERITY_DEBUG			(1)
#define ONION_SEVERITY_DEBUG_EXTRA		(2)

#define ONION_VERBOSITY_NONE			(ONION_SEVERITY_FATAL)
#define ONION_VERBOSITY_NORMAL			(ONION_SEVERITY_INFO)
#define ONION_VERBOSITY_VERBOSE			(ONION_SEVERITY_DEBUG)
#define ONION_VERBOSITY_EXTRA_VERBOSE	(ONION_SEVERITY_DEBUG_EXTRA)


#ifdef __cplusplus
extern "C" {
#endif 

#ifdef __cplusplus
extern 
#endif 
int 	onionVerbosityLevel;

// debug functions
void 	onionSetVerbosity		(int verbosityLevel);
int 	onionGetVerbosity 		();

void 	onionPrint 				(int severity, const char* msg, ...);

#ifdef __cplusplus
}
#endif

#endif // _ONION_DEBUG_H_ 
