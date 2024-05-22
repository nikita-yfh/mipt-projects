import RPi.GPIO as gpio
import time

gpio.setmode(gpio.BCM)

leds = [2, 3, 4, 17, 27, 22, 10, 9]
gpio.setup(leds, gpio.OUT)

for j in range(0, 3):
    for i in range(-1,8):
        if i >= 0:
            gpio.output(leds[i], 0)
        if i < 7:
            gpio.output(leds[i + 1], 1)
        time.sleep(0.2)
