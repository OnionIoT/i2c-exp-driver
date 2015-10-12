#!/bin/sh

LOW=11
HIGH=90

# initialize
pwm-exp -i 0 25
sleep 5

# testing loop
while [ 1 ]
do
        # set duty cycle to low setting
        pwm-exp all $LOW
        sleep 2

        # set duty cycle to high setting
        pwm-exp all $HIGH
        sleep 2
done

