from OmegaExpansion import relayExp
import time
import sys

# check the arguments
if len(sys.argv) != 2:
	print('ERROR: expected addr offset:')
	print('%s <addr offset>'%sys.argv[0])
	exit()

addr = int(sys.argv[1])

print('Starting to use relay-exp functions on addr offset %d'%addr)

relayExp.setVerbosity(0)

# check initialization
#	should return 0 if the Expansion has just been plugged in
ret 	= relayExp.checkInit(addr)
print("checking if initialized: %s"%ret)

# initialize the relay-exp
ret 	= relayExp.driverInit(addr)
print("Result from relayDriverInit: %s"%ret)
if (ret != 0):
	exit()

# check initialization
#	should return 1 since the Expansion was initialized above
ret 	= relayExp.checkInit(addr)
print("checking if initialized: %s"%ret)
time.sleep(1)


# set channel 0 to on
ret 	= relayExp.setChannel(addr, 0, 1)
print("Result from relaySetChannel: %s"%ret)
if (ret != 0):
	exit()
time.sleep(2)

# read channel 0 value
value 	= relayExp.readChannel(addr, 0)
print("Channel 0 value: %d"%value)
time.sleep(2)

# set both channels to on
ret 	= relayExp.setAllChannels(addr, 1)
print("Result from relaySetAllChannels: %s"%ret)
if (ret != 0):
	exit()
time.sleep(2)

# set channel 0 to off
ret 	= relayExp.setChannel(addr, 0, 0)
print("Result from relaySetChannel: %s"%ret)
if (ret != 0):
	exit()
time.sleep(2)

# read channel 0 value
value 	= relayExp.readChannel(addr, 0)
print("Channel 0 value: %d"%value)
time.sleep(2)


print("Done")
