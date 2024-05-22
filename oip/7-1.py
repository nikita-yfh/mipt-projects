import RPi.GPIO as gpio
import time
import matplotlib.pyplot as plt
dac = [8, 11, 7, 1, 0, 5, 12, 6]

print("Preparing to charging")

# pins setup
gpio.setmode(gpio.BCM)
gpio.setup(dac, gpio.OUT)

comp   = 14 # output of comparator 
troyka = 13 # power of troyka module

gpio.setup(comp, gpio.IN)
gpio.setup(troyka, gpio.OUT, initial=0)

def decimal2binary(value):
    return [int(element) for element in bin(value)[2:].zfill(8)]

def adc(): # binary search algorithm
    min = 0
    max = 256
    while True:
        value = int((min + max) / 2)
        gpio.output(dac, decimal2binary(value))
        time.sleep(0.01)
        if gpio.input(comp) == 0:
            min = value
        else:
            max = value
        if min == max or min + 1 == max:
            return float(min) / 255.0 * 3.3
    return 3.3

try:
    startTime = time.time()
    
    voltages = []  # graph Y
    times = []     # graph X
    
    adcRead = adc()
    
    # expirement
    
    print("Initial voltage, V:", adcRead)
    print("Charging")
    gpio.output(troyka, 1)
    while adcRead < 2.6:
        adcRead = adc()
        voltages.append(adcRead)
        times.append(time.time() - startTime)
        
    print("Charged to, V:", adcRead)
    print("Discharging")
    gpio.output(troyka, 0)
    while adcRead > 2.2:
        adcRead = adc()
        voltages.append(adcRead)
        times.append(time.time() - startTime)
        
    print("Discharged to, V:", adcRead)
    
    # calc results
    
    endTime = time.time()
    experimentTime = endTime - startTime
    qstep = 3.3 / 256 # 8 bit
    srate = len(times) / experimentTime
    print("Experiment time, s:", experimentTime)
    print("Measurement period, s:", experimentTime / len(times))
    print("Quantization step, V:", qstep)
    print("Sampling rate, Hz:", srate)
    print("Writing to files")
    
    # write to files
    
    fileData = open("data.txt", "w")
    for i in voltages:
        fileData.write("{:.03f}\n".format(i))
    fileData.close()
    
    fileSettings = open("settings.txt", "w")
    fileSettings.write("{:f}\n{:f}".format(qstep, srate))
    fileSettings.close()
    
    print("Writed to data.txt and settings.txt")
    
    # plot results 
    plt.plot(times, voltages)
    plt.show()
     
finally: # cleanup
    gpio.output(dac, 0)
    gpio.output(leds, 0)
    gpio.output(troyka, 0)
    gpio.cleanup()