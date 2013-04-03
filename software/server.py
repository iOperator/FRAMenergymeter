#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""server.py
FRAMenergymeter
Track your energy consumption with a TI MSP-EXP430FR5739

https://github.com/iOperator/FRAMenergymeter

Max Gröning, 2013
"""

from bottle import route, run, template, static_file  # Bottle
from bottle import debug
from basestation_uart import *

# Bottle settings
HOST = 'localhost'
PORT = 8080

###############################################################################

@route('/')
def index():
    data = main()
    return template('tpl_index', data=data, load_graph=True)

@route('/energy')
def index():
    data = main()
    return template('tpl_energy', data=data, load_graph=True)

@route('/power')
def index():
    data = main()
    return template('tpl_power', data=data, load_graph=True)

@route('/update')
def index():
    data = main()
    return template('tpl_update', data=data, load_graph=False)

@route('/static/<filename>')
def static_serve(filename):
    return static_file(filename, root='./static')

# run(host=HOST, port=PORT)
run(host=HOST, port=PORT, debug=True, reloader=True)  # For easier debugging
