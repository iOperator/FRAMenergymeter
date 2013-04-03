#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""server.py
FRAMenergymeter
Track your energy consumption with a TI MSP-EXP430FR5739

https://github.com/iOperator/FRAMenergymeter

Max Gröning, 2013
"""

from bottle import route, run, template, static_file, install  # Bottle
from bottle import get, post, request
from bottle_sqlite import SQLitePlugin  # Bottle SQLite Plugin
from bottle import debug
from basestation_uart import *

# Bottle settings
HOST = 'localhost'
PORT = 8080
DBFILENAME = 'data.db'

###############################################################################

install(SQLitePlugin(dbfile=DBFILENAME))

@route('/')
def index():
    data = main()
    return template('tpl_index', data=data, load_graph=True)

@route('/energy')
def energy():
    data = main()
    return template('tpl_energy', data=data, load_graph=True)

@route('/power')
def power():
    data = main()
    return template('tpl_power', data=data, load_graph=True)

@route('/update')
@route('/update', method='POST')
def update(db):
    data = main()
    return template('tpl_update', data=data, load_graph=False, request=request, db=db)

@route('/static/<filename>')
def static_serve(filename):
    return static_file(filename, root='./static')

# run(host=HOST, port=PORT)
run(host=HOST, port=PORT, debug=True, reloader=True)  # For easier debugging
