from OmegaExpansion import oledExp
import time


print 'Starting to use oled-exp functions...'

oledExp.setVerbosity(0)


# initialize
ret 	= oledExp.driverInit()
print "driverInit return: ", ret
if (ret != 0):
	exit()

# write a character
ret 	= oledExp.writeChar('x')
print "writeChar return: ", ret
if (ret != 0):
	exit()
time.sleep(2)

# invert the colours
ret 	= oledExp.setDisplayMode(1)
print "setDisplayMode return: ", ret
if (ret != 0):
	exit()
time.sleep(2)

# dim
ret 	= oledExp.setDim(1)
print "setDim return: ", ret
if (ret != 0):
	exit()
time.sleep(2)

# clear the display
ret 	= oledExp.clear()
print "clear return: ", ret
if (ret != 0):
	exit()
time.sleep(2)

# write a message
ret 	= oledExp.write("Welcome to the Omega")
print "write return: ", ret
if (ret != 0):
	exit()
time.sleep(2)

# set the cursor
ret 	= oledExp.setCursor(3,8)
print "write return: ", ret
if (ret != 0):
	exit()

# write a message
ret 	= oledExp.write("Invent the future")
print "write return: ", ret
if (ret != 0):
	exit()
time.sleep(2)

# invert the colours to normal
ret 	= oledExp.setDisplayMode(0)
print "setDisplayMode return: ", ret
if (ret != 0):
	exit()
time.sleep(2)

# set the highest brightness
ret 	= oledExp.setBrightness(255)
print "setBrightness return: ", ret
if (ret != 0):
	exit()
time.sleep(2)


# clear the display
ret 	= oledExp.clear()
print "clear return: ", ret
if (ret != 0):
	exit()

# set the cursor by pixel
ret 	= oledExp.setCursorByPixel(1,64)
print "setCursorByPixel return: ", ret
if (ret != 0):
	exit()

# draw a few bytes 
ret 	= oledExp.writeByte(0x0f)
print "writeByte return: ", ret
ret 	= oledExp.writeByte(0xf0)
print "writeByte return: ", ret
ret 	= oledExp.writeByte(0x0f)
print "writeByte return: ", ret
ret 	= oledExp.writeByte(0xf0)
print "writeByte return: ", ret
time.sleep(2)

ret 	= oledExp.setCursorByPixel(1,127)
print "setCursorByPixel return: ", ret
if (ret != 0):
	exit()

# draw a few bytes 
ret 	= oledExp.writeByte(0x0f)
print "writeByte return: ", ret
ret 	= oledExp.writeByte(0xf0)
print "writeByte return: ", ret
ret 	= oledExp.writeByte(0x0f)
print "writeByte return: ", ret
ret 	= oledExp.writeByte(0xf0)
print "writeByte return: ", ret


# clear the display
#ret 	= oledExp.clear()
#print "clear return: ", ret
#if (ret != 0):
#	exit()
#time.sleep(2)

# draw an image
#ret 	= oledExp.drawFromFile("lcd/onion.lcd")
#print "drawFromFile return: ", ret
#if (ret != 0):
#	exit()
#time.sleep(2)



print "Done"