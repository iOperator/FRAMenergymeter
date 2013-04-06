#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""makedb.py
FRAMenergymeter
Track your energy consumption with a TI MSP-EXP430FR5739

https://github.com/iOperator/FRAMenergymeter

Max Gröning, 2013
"""

import sqlite3

DBFILENAME = 'data.db'

DEFAULT_VALUES = (('fillGraph', 'true'),
                  ('animatedZooms', ''),
                  ('showRangeSelector', ''),
                  ('s0type', 'disabled'),
                  ('s1type', 'disabled'),
                  ('s2type', 'disabled'),
                  ('s3type', 'disabled'),
                  ('s0impulses', ''),
                  ('s1impulses', ''),
                  ('s2impulses', ''),
                  ('s3impulses', ''),
                  ('s0unit', ''),
                  ('s1unit', ''),
                  ('s2unit', ''),
                  ('s3unit', ''),
                  ('s0combined', ''),
                  ('s1combined', ''),
                  ('s2combined', ''),
                  ('s3combined', ''),
                  ('s0color', ''),
                  ('s1color', ''),
                  ('s2color', ''),
                  ('s3color', ''))

DEMO = True
DEMO_NUM = 1000  # Total count of samples
DEMO_START = (2013, 03, 01, 0, 0 , 0)  # First sample starts (year, month, day, hour, minute, second)
DEMO_TIME = 180  # Seconds between samples
DEMO_RAND = 10  # +/- x Seconds
DEMO_SEED = 0  # Seed for random generator. 'None' for current system time

###############################################################################

con = sqlite3.connect(DBFILENAME)

# Create a new database
con.execute("CREATE TABLE data (id INTEGER PRIMARY KEY, datum TEXT NOT NULL, sensor INTEGER NOT NULL)")
con.execute("CREATE TABLE config (setting TEXT PRIMARY KEY, value TEXT NOT NULL)")
con.commit()

# Storing default values
for (setting, value) in DEFAULT_VALUES:
    con.execute("INSERT INTO config (setting,value) VALUES (?,?)", (setting, value))
con.commit()

# Create demo samples
if DEMO:
    import datetime
    import random
    import math
    random.seed(DEMO_SEED)
    sample_time = [datetime.datetime(DEMO_START[0], DEMO_START[1],
                                     DEMO_START[2], DEMO_START[3],
                                     DEMO_START[4], DEMO_START[5])]
    for i in range(DEMO_NUM):
        offset = DEMO_TIME + 2 * DEMO_RAND * math.sin(i / 60.0) + DEMO_RAND * random.random()
        sample_time.append(sample_time[i] + datetime.timedelta(seconds = offset))

    for sample in sample_time:
        string = "{0:04d}{1:02d}{2:02d}{3:02d}{4:02d}{5:02d}".format(
            sample.year, sample.month, sample.day,
            sample.hour, sample.minute, sample.second)
        con.execute("INSERT INTO data (datum,sensor) VALUES (?,?)", (string, 0))
    con.commit()
