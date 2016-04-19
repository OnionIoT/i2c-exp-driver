#ifndef _RELAY_EXP_H_
#define _RELAY_EXP_H_

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

#include <onion-mcp23008-driver.h>

#define RELAY_EXP_ADDR_SWITCH_NUM			3
#define RELAY_EXP_ADDR_SWITCH_DEFAULT_VAL	"000"

// type definitions
typedef enum e_RelayDriverChannels {
	RELAY_EXP_CHANNEL0 		= 0,
	RELAY_EXP_CHANNEL1,
	RELAY_EXP_NUM_CHANNELS,
} eRelayDriverChannels;

#ifdef __cplusplus
extern "C" {
#endif

int 	relayDriverInit 		(int addr);
int 	relayCheckInit 			(int addr, int *bInitialized);

int 	relaySetChannel			(int addr, int channel, int state);
int 	relaySetAllChannels		(int addr, int state);

#ifdef __cplusplus
}
#endif 
#endif // _RELAY_EXP_H_