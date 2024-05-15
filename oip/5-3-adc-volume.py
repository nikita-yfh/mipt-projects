import RPi.GPIO as gpio
import time
dac = [8, 11, 7, 1, 0, 5, 12, 6]
leds = [2, 3, 4, 17, 27, 22, 10, 9]

gpio.setmode(gpio.BCM)
gpio.setup(dac, gpio.OUT)
gpio.setup(leds, gpio.OUT)

comp = 14
troyka = 13

gpio.setup(comp, gpio.IN)
gpio.setup(troyka, gpio.OUT, initial=1)

def decimal2binary(value):
    return [int(element) for element in bin(value)[2:].zfill(8)]

def writeleds(value):
    for i in range(0, 8):
        value1 = (float(i) + 0.25) / 8 * 3.3
        if(value1 > value):
            print(0)
            gpio.output(leds[i], 0)
        else:
            print(1)
            gpio.output(leds[i], 1)
def adc():
    for i in range(0, 256):
        gpio.output(dac, decimal2binary(i))
        time.sleep(0.001)
        if gpio.input(comp) == 1:
            return float(i) / 255.0 * 3.3

    return 3.3


try:
    while True:
        writeleds(adc())
finally:
    gpio.output(dac, 0)
    gpio.cleanup()
