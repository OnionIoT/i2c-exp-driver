#! /usr/bin/python
#Sierpenski Triangle Example for ssd1306 OLED board (and Omega2 of course!)
#by Casten Riepling
#Thanks to greenbreakfast for the nice i2c-exp-driver library + examples!

from OmegaExpansion import oledExp as oled
import random
import time

#since we can only write a byte at a time, we need to keep
#the previous state in memory to OR it with
state = [[0 for x in range(16)] for y in range(128)]


#init all the 1306 driver stuff
oled.driverInit()
oled.setImageColumns()
oled.setDisplayMode(0)

#init our positional state and boundary vertices
currX = currY = 0
vertices = [{'x':0,'y':0}, {'x':63,'y':63}, {'x':0,'y':127}]

#To make it a little more fun, we'll use a diminishing wait so one can
#get an idea of the algorithm at work
sleepTime = 1.0

#The algorithm works as follows.  Start with 3 vertices of a triangle and begin at
#one of them.  Randomly choose a vertex.  Go half way to that vertex and draw a point.
#Repeat until you are happy.

while True:
        #pick a vertex
        i = random.randint(0,2)
        vert = vertices[i]

        #find the point halfway between
        currX = int((vert['x']+currX) / 2)
        currY = int((vert['y']+currY) / 2)

        #convert to columns and bit patterns
        col = int(currX/8)
        bitX = 1<<(currX%8)

        #OR the info into the existing data
        state[currY][col] |= int(state[currY][col] | bitX)

        #write the data to the display
        oled.setCursorByPixel(col,currY)
        oled.writeByte(state[currY][col])

        #and do some sleeping for dramatic effect
        time.sleep(sleepTime)
        sleepTime = sleepTime - (sleepTime/20)
