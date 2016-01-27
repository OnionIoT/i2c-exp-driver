from OmegaExpansion import pwmExp
import time
import sys

# check the arguments
channel = 0
if len(sys.argv) == 2:
	channel = int(sys.argv[1])

print '>> Using channel ', channel

pwmExp.setVerbosity(0)

# check initialization
#	should return 0 if the PWM Expansion has just been plugged in
ret 	= pwmExp.checkInit()
print "checking if initialized: ", ret

# initialize the pwm-exp
ret 	= pwmExp.driverInit()
print "driverInit return: ", ret
if (ret != 0):
	exit()
ret 	= pwmExp.setFrequency(50)	
print "setFrequency return: ", ret
if (ret != 0):
	exit()

# check initialization
#	should return 1 since the Expansion was initialized above
ret 	= pwmExp.checkInit()
print "checking if initialized: ", ret
time.sleep(1)


# set channel 0
ret 	= pwmExp.setupDriver(channel, 9, 0)
print "setupDriver return: ", ret
if (ret != 0):
	exit()
time.sleep(2)

# set channel 0
ret 	= pwmExp.setupDriver(channel, 5, 0)
print "setupDriver return: ", ret
if (ret != 0):
	exit()
time.sleep(2)

# change the frequency
ret 	= pwmExp.setFrequency(60)
print "setFrequency return: ", ret
if (ret != 0):
	exit()
time.sleep(2)

# set channel 0
ret 	= pwmExp.setupDriver(channel, 6.55, 0)
print "setupDriver return: ", ret
if (ret != 0):
	exit()
time.sleep(2)


print "Done"
