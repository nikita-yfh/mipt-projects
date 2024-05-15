import RPi.GPIO as gpio
import time

gpio.setmode(gpio.BCM)
gpio.setup(24, gpio.OUT)
pwm = gpio.PWM(24, 1000)
pwm.start(0)

try:
    while True:
        value = int(input("Input dute cycle: "))
        pwm.ChangeDutyCycle(value)
except Exception as error:
    print(error)
finally:
    gpio.cleanup()