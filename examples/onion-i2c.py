from OmegaExpansion import onionI2C
import time
import sys

print('Starting: onionI2C module testing...')

i2c 	= onionI2C.OnionI2C(0)

# set the verbosity
i2c.setVerbosity(1)

devAddr = 0x27


print("")
ret = raw_input('  Ready to read?')

# perform series of reads
size 	= 1
addr 	= 0x00
print('Reading from device 0x%02x, address: 0x%02x'%(devAddr, addr))
val 	= i2c.readBytes(devAddr, addr, size)
print('   Read returned: %s'%(val))

size 	= 1
addr 	= 0x01
print('Reading from device 0x%02x, address: 0x%02x'%(devAddr, addr))
val 	= i2c.readBytes(devAddr, addr, size)
print('   Read returned: %s'%(val))

size 	= 2
addr 	= 0x06
print('Reading from device 0x%02x, address: 0x%02x'%(devAddr, addr))
val 	= i2c.readBytes(devAddr, addr, size)
print('   Read returned: %s'%(val))



print("")
ret = raw_input('  Ready to write?')

# perform series of writes
size 	= 1
addr 	= 0x00
value	= [0x00]
print('Writing to device 0x%02x, address: 0x%02x'%(devAddr, addr))
val 	= i2c.writeBytes(devAddr, addr, value)
print('   writeBytes returned: %s'%(val))

size 	= 1
addr 	= 0x01
value 	= 0x00
print('Writing to device 0x%02x, address: 0x%02x'%(devAddr, addr))
# comment: confirmed that this causes a TypeError :)
# val 	= i2c.writeBytes(devAddr, addr, value)
# print('   writeBytes returned: ', val)
val 	= i2c.writeByte(devAddr, addr, value)
print('   writeByte returned: %s'%(val))

size 	= 2
value 	= [0x06, 0x00]
print('Writing to device 0x%02x, list is: %s'%(devAddr, value))
val 	= i2c.write(devAddr, value)
print('   write returned: %s'%(val))



print("")
ret = raw_input('  Ready to test writing? Part 1')

# perform write
size 	= 1
addr 	= 0x09
value	= [0x01]
print('Writing to device 0x%02x, address: 0x%02x, writing: 0x%02x'%(devAddr, addr, value[0]))
val 	= i2c.writeBytes(devAddr, addr, value)
print('   writeBytes returned: %s'%(val))

# read back the value
size 	= 1
addr 	= 0x09
print('Reading from device 0x%02x, address: 0x%02x'%(devAddr, addr))
val 	= i2c.readBytes(devAddr, addr, size)
print('   Read returned: %s'%(val))




print("")
ret = raw_input('  Ready to test writing? Part 2')

# perform write
size 	= 1
addr 	= 0x09
value 	= 0x02
print('Writing to device 0x%02x, address: 0x%02x, writing: 0x%02x'%(devAddr, addr, value))
val 	= i2c.writeByte(devAddr, addr, value)
print('   writeByte returned: %s'%(val))

# read back the value
size 	= 1
addr 	= 0x09
print('Reading from device 0x%02x, address: 0x%02x'%(devAddr, addr))
val 	= i2c.readBytes(devAddr, addr, size)
print('   Read returned: %s'%(val))




print("")
ret = raw_input('  Ready to test writing? Part 3')

# perform write
size 	= 2
value 	= [0x09, 0x03]
print('Writing to device 0x%02x, list is: %s'%(devAddr, value))
val 	= i2c.write(devAddr, value)
print('   write returned: %s'%(val))

# read back the value
size 	= 1
addr 	= 0x09
print('Reading from device 0x%02x, address: 0x%02x'%(devAddr, addr))
val 	= i2c.readBytes(devAddr, addr, size)
print('   Read returned: %s'%(val))



print("")
ret = raw_input('  Ready finish testing?')

# perform write
size 	= 1
addr 	= 0x09
value 	= 0x00
print('Writing to device 0x%02x, address: 0x%02x, writing: 0x%02x'%(devAddr, addr, value))
val 	= i2c.writeByte(devAddr, addr, value)
print('   writeByte returned: %s'%(val))




print('Done!')
