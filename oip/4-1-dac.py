import RPi.GPIO as gpio
dac = [8, 11, 7, 1, 0, 5, 12, 6]

gpio.setmode(gpio.BCM)
gpio.setup(dac, gpio.OUT)

def decimal2binary(value):
    return [int(element) for element in bin(value)[2:].zfill(8)]

try:
    while True:
        value = int(input("Enter a number: "))
        if value < 0:
            raise Exception("not positive")
        if value > 255:
            raise Exception("bigger then 255")
        gpio.output(dac, decimal2binary(value))
except Exception as error:
    print(error)
finally:
    gpio.output(dac, 0)
    gpio.cleanup()


