import RPi.GPIO as gpio
import time
dac = [8, 11, 7, 1, 0, 5, 12, 6]

gpio.setmode(gpio.BCM)
gpio.setup(dac, gpio.OUT)

number = [0, 1, 1, 1, 1, 1, 1, 1]

gpio.output(dac,number)

time.sleep(10)

gpio.output(dac, 0)
gpio.cleanup()



