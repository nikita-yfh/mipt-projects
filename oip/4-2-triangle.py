import RPi.GPIO as gpio
import time
dac = [8, 11, 7, 1, 0, 5, 12, 6]

gpio.setmode(gpio.BCM)
gpio.setup(dac, gpio.OUT)

def decimal2binary(value):
    return [int(element) for element in bin(value)[2:].zfill(8)]

try:
    period = float(input())
    while True:
        cycle = time.time() % period / period;
        if cycle < 0.5:
            gpio.output(dac, decimal2binary(int(cycle * 2 * 255)))
            print(int(cycle * 2 * 255))
        else:
            gpio.output(dac, decimal2binary(int((1 - cycle) * 2 * 255)))
            print(int((1 - cycle) * 2 * 255))
        
except Exception as error:
    print(error)
finally:
    gpio.output(dac, 0)
    gpio.cleanup()