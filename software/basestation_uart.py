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
PORT = '/dev/ttyUSB1'  # Windows ports start from '0'(=COM1),
                       # Linux: e.g. '/dev/ttyUSB0'
BAUDRATE = 9600
TIMEOUT = 2  # Timeout in seconds
SEND_DELAY = 0.1  # Delay between sent chars
READ_DELAY = 0.1  # Wait if more date arrives

###############################################################################

def send(ser, string):
    """Sends string with delay between each character.
    """
    for char in string:
        ser.write(char)
        time.sleep(SEND_DELAY)

def read(ser):
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

def main(port=PORT, baudrate=BAUDRATE):
    """Performs all communication with the device and returns a dictionary with
    the collected data.
    """
    data = {}  # Stores all data that we return

    ser = serial.Serial() # Open serial connection
    ser.baudrate = baudrate
    ser.port = port
    ser.timeout = TIMEOUT

    try:
        ser.open()
    except:
        data['connection'] = False
        return data

    if ser.isOpen():
        data['connection'] = True
    else:
        data['connection'] = False
        return data

    send(ser, '\n')  # Make sure the device is awake
    response = read(ser)
    if 'FRAMenergymeter' not in response:
        # Device was running, get the info again
        send(ser, 'info\n')
        response = read(ser)

    # Extract the infomation from received data
    FIRMWARE = re.search('Firmware v([\d\.]+)', response).group(1)
    data['firmware'] = FIRMWARE

    SENSORS = re.findall('(S\d{1}), (\w+), (\d+), (\d+)', response)
    data['sensors'] = SENSORS
    active_sensors = []  # Store active sensors here
    for sensor in SENSORS:
        if sensor[1] != 'disabled':
            active_sensors.append(sensor)
    data['active_sensors'] = active_sensors
    MEMORY = [re.search('Memory: (\d+) \w+ (\d+)', response).group(1),
              re.search('Memory: (\d+) \w+ (\d+)', response).group(2)]
    data['memory'] = MEMORY

    TIME = re.search('RTC time: ([\d:]+)', response).group(1)
    data['time'] = TIME
    DATE = re.search('RTC date: (\w+, [\d-]+)', response).group(1)
    data['date'] = DATE

    # Getting data from device
    if MEMORY[0] == '0':
        data['data'] = []
        return data
    else:
        send(ser, 'send\n')
        response = read(ser)
        DATA = re.findall('\r\n(\d+)', response)
        data['data'] = DATA

    if len(DATA) == int(MEMORY[0]):  # Make sure we collected all data
#        print('got all data.')
        # Clear device now
        # [TBD]
        pass
    else:
#        print('Data changed, collecting data again ...')
        # Collect missing data
        # [TBD]
        pass

    return data

###############################################################################

if __name__ == "__main__":  # Run standalone
    data = main()
    if data['connection']:
        print("\nPort opened.\n")
    else:
        print("\nCannot open port.\n")
        exit()
    print("Device firmware: v" + data['firmware'])
    print("Supports " + str(len(data['sensors'])) + " sensors.")
    print("Active sensors:")
    for sensor in data['active_sensors']:
        print(sensor[0] + ', type: ' + sensor[1])
    print('Device stores ' + data['memory'][0] + ' of ' + data['memory'][1] +
          ' possible entries.' + ' (' +
          str(int(data['memory'][0]) / float(data['memory'][1]) * 100) +
          '% full)')
    print('Time and date of the device: ' + data['date'] + ', ' + data['time'])

    if data['data'] == []:
        print('\nNo new data.')
        exit()
    else:
        print('\nGetting new data ...')
    # Testing only
    for line in data['data']:
        print line
