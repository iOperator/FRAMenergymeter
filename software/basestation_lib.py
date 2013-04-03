#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""basestation_uart.py
FRAMenergymeter
Track your energy consumption with a TI MSP-EXP430FR5739

https://github.com/iOperator/FRAMenergymeter

Max Gröning, 2013
"""

def datum_string(datum):
    """Converts a datum from the basestation into a
    JavaScript compatible format.
    """
    return datum[0:4] + '-' + datum[4:6] + '-' + datum[6:8] + \
        ' ' + datum[8:10] + ':' + datum[10:12] + ':' + datum[12:14]
