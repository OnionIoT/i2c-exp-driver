from OmegaExpansion import pwmExp


__version__ = "0.1"


SERVO_FREQUENCY 		= 50
SERVO_MIN_PULSE			= 600		# 600us 	= 0.6ms 
SERVO_MAX_PULSE			= 2400		# 2400us 	= 2.4ms

H_BRIDGE_MOTOR_FORWARD 	= 0
H_BRIDGE_MOTOR_REVERSE 	= 1

class OmegaPwm:
	"""Base class for PWM signal"""

	def __init__(self, channel):
		self.channel 	= channel
		self.frequency 	= SERVO_FREQUENCY

		# check that pwm-exp has been initialized
		bInit 	= pwmExp.checkInit()

		if (bInit == 0):
			# initialize the expansion
			ret 	= pwmExp.driverInit()
			if (ret != 0):
				print 'ERROR: pwm-exp init not successful!'

			# set to default frequency
			self._setFrequency(self.frequency)


	def _setFrequency(self, freq):
		"""Set frequency of pwm-exp oscillator"""
		self.frequency 	= freq
		ret 	= pwmExp.setFrequency(freq);
		if (ret != 0):
			print 'ERROR: pwm-exp setFrequency not successful!'

		return ret

	def getFrequency(self):
		"""Get frequency of pwm-exp oscillator"""
		return self.frequency

	def setDutyCycle(self, duty):
		"""Set duty cycle for pwm channel"""
		ret 	= pwmExp.setupDriver(self.channel, duty, 0)
		if (ret != 0):
			print 'ERROR: pwm-exp setupDriver not successful!'

		return ret


class DigitalPin:
	"""Class that uses PWM signals as 0 or 1"""

	def __init__(self, channel):
		# initialize a pwm channel
		self.channel 	= channel
		self.pwmDriver 	= OmegaPwm(self.channel)

	def set(self, value):
		"""Set the PWM signal to either OFF (0%) or ON (100%)"""
		if (value == 0):
			duty 	= 0
		else:
			duty 	= 100

		ret 	= self.pwmDriver.setDutyCycle(duty)
		return ret

	def turnOff(self):
		"""Set the PWM signal to OFF (0%)"""
		ret 	= self.set(0)
		return ret

	def turnOn(self):
		"""Set the PWM signal to ON (100%)"""
		ret 	= self.set(100)
		return ret


class Servo:
	"""Class that uses PWM signals to drive a servo"""

	def __init__(self, channel, minPulse, maxPulse):
		# initialize a pwm channel
		self.channel 	= channel
		self.pwmDriver 	= OmegaPwm(self.channel)

		# note the min and max pulses (in microseconds)
		self.minPulse 	= minPulse
		self.maxPulse 	= maxPulse

		# calculate the total range
		self.range 		= self.maxPulse - self.minPulse

		# calculate the us / degree
		self.step 		= self.range / float(180)

		# calculate the period (in us)
		self.period 	= (1000000 / self.pwmDriver.getFrequency()) 

		# initialize the min and max angles
		self.minAngle 	= 0
		self.maxAngle 	= 180

	def getSettings(self):
		"""Return an object with all settings"""
		settings 	= {}
		settings['channel']		= self.channel
		settings['minPulse']	= self.minPulse
		settings['maxPulse']	= self.maxPulse

		settings['range']		= self.range
		settings['step']		= self.step
		settings['period']		= self.period

		settings['minAngle']	= self.minAngle
		settings['maxAngle']	= self.maxAngle

		return settings

	def setupMinAngle(self, angle):
		"""Program a minimum angle"""
		self.minAngle 	= angle

	def setupMaxAngle(self, angle):
		"""Program a maximum angle"""
		self.maxAngle 	= angle

	def setAngle(self, angle):
		"""Move the servo to the specified angle"""
		# check against the minimum and maximium angles
		if angle < self.minAngle:
			angle 	= self.minAngle
		elif angle > self.maxAngle:
			angle 	= self.maxAngle

		# calculate pulse width for this angle
		pulseWidth 	= angle * self.step + self.minPulse

		# find the duty cycle percentage of the pulse width
		duty 		= (pulseWidth * 100) / float(self.period)

		# program the duty cycle
		ret = self.pwmDriver.setDutyCycle(duty)
		return ret

class hBridgeMotor:
	"""Class that two digital signals and a pwm signal to control an h-bridge"""

	def __init__(self, pwmChannel, fwdChannel, revChannel):
		# note the channels
		self.pwmChannel 	= pwmChannel
		self.fwdChannel		= fwdChannel
		self.revChannel 	= revChannel

		# setup the objects
		self.pwmDriver 		= OmegaPwm(self.pwmChannel)
		self.pwmDriver.setDutyCycle(0)
		self.fwdDriver 		= DigitalPin(self.fwdChannel)
		self.fwdDriver.turnOff()
		self.revDriver 		= DigitalPin(self.revChannel)
		self.revDriver.turnOff()

		# setup the limitations
		self.minDuty 		= 0
		self.maxDuty		= 100

	def setupMinDuty(self, duty):
		"""Set the minimum allowed duty cycle for pwm"""
		self.minDuty 		= duty

	def setupMaxDuty(self, duty):
		"""Set the maximum allowed duty cycle for pwm"""
		self.maxDuty 		= duty

	def reset(self):
		"""Set the PWM to 0%, disable both h-bridge controls"""
		ret 	=  self.pwmDriver.setDutyCycle(0)
		ret 	|= self.fwdDriver.turnOff()
		ret 	|= self.revDriver.turnOff()

		return ret

	def spin(self, direction, duty):
		"""Set the PWM to the specified duty, and in the specified direction"""
		ret 	= 0

		# 0 - forward, 1 - reverse
		if (direction == H_BRIDGE_MOTOR_FORWARD):
			self.revDriver.turnOff()
			self.fwdDriver.turnOn()
		elif (direction == H_BRIDGE_MOTOR_REVERSE):
			self.fwdDriver.turnOff()
			self.revDriver.turnOn()
		else:
			ret 	= -1

		if (ret == 0):
			# check against the minimum and maximium pwm
			if duty < self.minDuty:
				duty 	= self.minDuty
			elif duty > self.maxDuty:
				duty 	= self.maxDuty
		
			# program the duty cycle
			ret 	= self.pwmDriver.setDutyCycle(duty)
		return ret

	def spinForward(self, duty):
		ret 	= self.drive(H_BRIDGE_MOTOR_FORWARD, duty)
		return ret

	def spinReverse(self, duty):
		ret 	= self.drive(H_BRIDGE_MOTOR_REVERSE, duty)
		return ret


