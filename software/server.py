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
from basestation_lib import *
from basestation_uart import *

# Bottle settings
HOST = 'localhost'
PORT = 8080
DBFILENAME = 'data.db'

###############################################################################

install(SQLitePlugin(dbfile=DBFILENAME))

@route('/')
def index(db):
    return template('tpl_index', load_graph=False, db=db)

@route('/energy')
def energy(db):
    return template('tpl_energy', load_graph=True, db=db)

@route('/power')
def power(db):
    return template('tpl_power', load_graph=True, db=db)

@route('/update')
@route('/update', method='POST')
def update(db):
    data = main()
    return template('tpl_update', data=data, load_graph=False, request=request, db=db)

@route('/config')
@route('/config', method='POST')
def config(db):
    # Form update
    update = False
    if request.POST.get('config') != None:
        update = True
        # Update booleans:
        for setting in ('fillGraph', 'animatedZooms', 'showRangeSelector'):
            value = "" if request.POST.get(setting) == None else "true"
            db.execute("UPDATE config SET value=? WHERE setting=?", (value, setting))
        for setting in ('s0type', 's1type', 's2type', 's3type',
                        's0impulses', 's1impulses', 's2impulses', 's3impulses',
                        's0unit', 's1unit', 's2unit', 's3unit',
                        's0combined', 's1combined', 's2combined', 's3combined'):
            value = request.POST.get(setting)
            db.execute("UPDATE config SET value=? WHERE setting=?", (value, setting))

    # Get config from database
    config={}
    db_result = db.execute("SELECT * FROM config").fetchall()
    for row in db_result:
        config[row['setting']] = row['value']
    return template('tpl_config', load_graph=False, config=config, update=update)

@route('/static/<filename>')
def static_serve(filename):
    return static_file(filename, root='./static')

# run(host=HOST, port=PORT)
run(host=HOST, port=PORT, debug=True, reloader=True)  # For easier debugging
