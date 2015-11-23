#ifndef _ONION_DEBUG_H_
#define _ONION_DEBUG_H_

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define ONION_SEVERITY_FATAL			(-1)
#define ONION_SEVERITY_INFO				(0)
#define ONION_SEVERITY_DEBUG			(1)
#define ONION_SEVERITY_DEBUG_EXTRA		(2)


#ifdef __cplusplus
extern "C"{
#endif 

int 	onionVerbosityLevel;

// debug functions
void 	onionSetVerbosity		(int verbosityLevel);
void 	onionPrint 				(int severity, const char* msg, ...);

#ifdef __cplusplus
}
#endif 
#endif // _ONION_DEBUG_H_ 