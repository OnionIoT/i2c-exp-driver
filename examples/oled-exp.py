from OmegaExpansion import oledExp
import time


print('Starting to use oled-exp functions...')

oledExp.setVerbosity(0)


# initialize
ret 	= oledExp.driverInit()
print("driverInit return: %s"%ret)
if (ret != 0):
	exit()

# write a character
ret 	= oledExp.writeChar('x')
print("writeChar return: %s"%ret)
if (ret != 0):
	exit()
time.sleep(2)

# invert the colours
ret 	= oledExp.setDisplayMode(1)
print("setDisplayMode return: %s"%ret)
if (ret != 0):
	exit()
time.sleep(2)

# dim
ret 	= oledExp.setDim(1)
print("setDim return: %s"%ret)
if (ret != 0):
	exit()
time.sleep(2)

# clear the display
ret 	= oledExp.clear()
print("clear return: %s"%ret)
if (ret != 0):
	exit()
time.sleep(2)

# write a message
ret 	= oledExp.write("Welcome to the Omega")
print("write return: %s"%ret)
if (ret != 0):
	exit()
time.sleep(2)

# set the cursor
ret 	= oledExp.setCursor(3,8)
print("write return: %s"%ret)
if (ret != 0):
	exit()

# write a message
ret 	= oledExp.write("Invent the future")
print("write return: %s"%ret)
if (ret != 0):
	exit()
time.sleep(2)

# invert the colours to normal
ret 	= oledExp.setDisplayMode(0)
print("setDisplayMode return: %s"%ret)
if (ret != 0):
	exit()
time.sleep(2)

# set the highest brightness
ret 	= oledExp.setBrightness(255)
print("setBrightness return: %s"%ret)
if (ret != 0):
	exit()
time.sleep(2)


# clear the display
ret 	= oledExp.clear()
print("clear return: %s"%ret)
if (ret != 0):
	exit()

# set the cursor by pixel
ret 	= oledExp.setCursorByPixel(1,64)
print("setCursorByPixel return: %s"%ret)
if (ret != 0):
	exit()

# draw a few bytes
ret 	= oledExp.writeByte(0x0f)
print("writeByte return: %s"%ret)
ret 	= oledExp.writeByte(0xf0)
print("writeByte return: %s"%ret)
ret 	= oledExp.writeByte(0x0f)
print("writeByte return: %s"%ret)
ret 	= oledExp.writeByte(0xf0)
print("writeByte return: %s"%ret)
time.sleep(2)

ret 	= oledExp.setCursorByPixel(1,127)
print("setCursorByPixel return: %s"%ret)
if (ret != 0):
	exit()

# draw a few bytes
ret 	= oledExp.writeByte(0x0f)
print("writeByte return: %s"%ret)
ret 	= oledExp.writeByte(0xf0)
print("writeByte return: %s"%ret)
ret 	= oledExp.writeByte(0x0f)
print("writeByte return: %s"%ret)
ret 	= oledExp.writeByte(0xf0)
print("writeByte return: %s"%ret)


# clear the display
#ret 	= oledExp.clear()
#print("clear return: %s"%ret)
#if (ret != 0):
#	exit()
#time.sleep(2)

# draw an image
#ret 	= oledExp.drawFromFile("lcd/onion.lcd")
#print("drawFromFile return: %s"%ret)
#if (ret != 0):
#	exit()
#time.sleep(2)



print("Done")
