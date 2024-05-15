import RPi.GPIO as gpio
import time
dac = [8, 11, 7, 1, 0, 5, 12, 6]

gpio.setmode(gpio.BCM)
gpio.setup(dac, gpio.OUT)

comp = 14
troyka = 13

gpio.setup(comp, gpio.IN)
gpio.setup(troyka, gpio.OUT, initial=1)

def decimal2binary(value):
    return [int(element) for element in bin(value)[2:].zfill(8)]

def adc():
    for i in range(0, 256):
        gpio.output(dac, decimal2binary(i))
        time.sleep(0.001)
        if gpio.input(comp) == 1:
            return float(i) / 255.0 * 3.3
    return 3.3

try:
    while True:
        print(adc())
except Exception as error:
    print(error)
finally:
    gpio.output(dac, 0)
    gpio.cleanup()