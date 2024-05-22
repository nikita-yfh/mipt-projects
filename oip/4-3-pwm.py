import RPi.GPIO as gpio
import time

pwmPin=2

gpio.setmode(gpio.BCM)
gpio.setup(pwmPin, gpio.OUT)
pwm = gpio.PWM(pwmPin, 1000)
pwm.start(0)

try:
    while True:
        value = int(input("Input duty cycle: "))
        pwm.ChangeDutyCycle(value)
except Exception as error:
    print(error)
finally:
    gpio.cleanup()