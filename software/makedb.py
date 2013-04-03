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

###############################################################################

con = sqlite3.connect(DBFILENAME)

# Create a new database
con.execute("CREATE TABLE data (id INTEGER PRIMARY KEY, datum TEXT NOT NULL, sensor INTEGER NOT NULL)")
con.commit()
