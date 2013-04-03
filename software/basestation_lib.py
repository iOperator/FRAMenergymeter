#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""basestation_lib.py
FRAMenergymeter
Track your energy consumption with a TI MSP-EXP430FR5739

https://github.com/iOperator/FRAMenergymeter

Max Gröning, 2013
"""

import datetime

###############################################################################

def datum_string(datum):
    """Converts a datum from the basestation into a
    JavaScript compatible format.
    """
    return datum[0:4] + '-' + datum[4:6] + '-' + datum[6:8] + \
        ' ' + datum[8:10] + ':' + datum[10:12] + ':' + datum[12:14]

def datum_difference(datum1, datum2):
    """Returns the time difference between datum1 and datum2 in seconds.

    datum2 is expected to be the later one.
    """
    dt1 = datetime.datetime.strptime(datum1, "%Y%m%d%H%M%S")
    dt2 = datetime.datetime.strptime(datum2, "%Y%m%d%H%M%S")
    return (dt2 - dt1).seconds
