#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""server.py
FRAMenergymeter
Track your energy consumption with a TI MSP-EXP430FR5739

https://github.com/iOperator/FRAMenergymeter

Max Gröning, 2013
"""

from bottle import route, run, template  # Bottle
from server_templates import *
from basestation_uart import *

# Bottle settings
HOST = 'localhost'
PORT = 8080

###############################################################################

@route('/')
@route('/info/')
def index():
    data = main()
    return template(tpl_index, data=data)

# run(host=HOST, port=PORT)
run(host=HOST, port=PORT, debug=True, reloader=True)  # For easier debugging
