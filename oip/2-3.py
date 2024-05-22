import RPi.GPIO as gpio
import time

gpio.setmode(gpio.BCM)

leds = [2, 3, 4, 17, 27, 22, 10, 9]
gpio.setup(leds, gpio.OUT)

aux = [21,20,26,16,19,25,23,24]
gpio.setup(aux, gpio.IN, gpio.PUD_UP)

while True:
    for i in range(0, 8):
        gpio.output(leds[i], gpio.input(aux[i]))