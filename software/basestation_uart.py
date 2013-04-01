#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""basestation_uart.py
FRAMenergymeter
Track your energy consumption with a TI MSP-EXP430FR5739

https://github.com/iOperator/FRAMenergymeter

Max Gröning, 2013
"""

import serial  # pyserial
import time
import re  # regular expressions

# UART settings
BAUDRATE = 9600
PORT = '/dev/ttyUSB1'  # Windows ports start from '0'(=COM1),
                       # Linux: e.g. '/dev/ttyUSB0'
TIMEOUT = 2  # Timeout in secon
SEND_DELAY = 0.1  # Delay between sent chars
READ_DELAY = 0.1  # Wait if more date arrives

##############################################################################

def send(string):
    """Sends string with delay between each character.
    """
    for char in string:
        ser.write(char)
        time.sleep(SEND_DELAY)

def read():
    """Reads characters from serial buffer. Waits if new chars have arrived,
    then returns a string containing all received characters.
    """
    no_chars = 0;
    while 1:
        in_buf = ser.inWaiting()
        if in_buf != no_chars:  # New characters in buffer
            no_chars = in_buf
            time.sleep(READ_DELAY)
        else:  # No change in characters in buffer
            break
    return ser.read(no_chars)


##############################################################################
ser = serial.Serial() # Open serial connection
ser.baudrate = BAUDRATE
ser.port = PORT
ser.timeout = TIMEOUT

ser.open()

if ser.isOpen():
    print("\nPort opened.\n")
else:
    print("\nCannot open port.\n")
    exit()

send('\n')  # Make sure the device is awake
response = read()
if 'FRAMenergymeter' not in response:
    # Device was running, get the info again
    send('info\n')
    response = read()
# Extract the infomation from received data
FIRMWARE = re.search('Firmware v([\d\.]+)', response).group(1)
print("Device firmware: v" + FIRMWARE)
SENSORS = re.findall('(S\d{1}), (\w+), (\d+), (\d+)', response)
print("Supports " + str(len(SENSORS)) + " sensors.")
print("Active sensors:")
active_sensors = []  # Store active sensors here
for sensor in SENSORS:
    if sensor[1] != 'disabled':
        active_sensors.append(sensor)
        print(sensor[0] + ', type: ' + sensor[1])
MEMORY = [re.search('Memory: (\d+) \w+ (\d+)', response).group(1),
          re.search('Memory: (\d+) \w+ (\d+)', response).group(2)]
print('Device stores ' + MEMORY[0] + ' of ' + MEMORY[1] + ' possible entries.' +
      ' (' + str(int(MEMORY[0]) / float(MEMORY[1]) * 100) + '% full)')
TIME = re.search('RTC time: ([\d:]+)', response).group(1)
DATE = re.search('RTC date: (\w+, [\d-]+)', response).group(1)
print('Time and date of the device: ' + DATE + ', ' + TIME)
# Getting data from device
if MEMORY[0] == '0':
    print('\nNo new data.')
    exit()
else:
    print('\nGetting new data ...')
    send('send\n')
    response = read()
    DATA = re.findall('\r\n(\d+)', response)

if len(DATA) == int(MEMORY[0]):  # Make sure we collected all data
    print('got all data.')
    # Clear device now
    # ...
else:
    print('Data changed, collecting data again ...')
    # Collect missing data
    pass

# Testing only
for line in DATA:
    print line
