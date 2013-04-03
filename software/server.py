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

@route('/static/style.css')
def static_style():
    return static_file('style.css', root='./static')

@route('/static/pattern.png')
def static_pattern():
    return static_file('pattern2.png', root='./static')

@route('/static/dygraph-combined.js')
def static_graph():
    return static_file('dygraph-combined.js', root='./static')

# run(host=HOST, port=PORT)
run(host=HOST, port=PORT, debug=True, reloader=True)  # For easier debugging
