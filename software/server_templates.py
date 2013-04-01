#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""server.py
FRAMenergymeter
Track your energy consumption with a TI MSP-EXP430FR5739

https://github.com/iOperator/FRAMenergymeter

Max Gröning, 2013
"""

TPL_INDEX = """\
<p><b>FRAMenergymeter</b></p>
%if data['connection']:
<p>Connected to device.</p>
%else:
<p>Cannot connect to device.</p>
%end
<p>Device firmware: v{{data['firmware']}}</p>
<p>Supports {{str(len(data['sensors']))}} sensors.</p>
<p>Active sensors:<br />
<ul>
%for sensor in data['active_sensors']:
  <li>{{sensor[0]}}, type: {{sensor[1]}}</li>
%end
</ul>
</p>
<p>Device stores {{data['memory'][0]}} of {{data['memory'][1]}} possible entries. ({{int(data['memory'][0]) / float(data['memory'][1]) * 100}}% full)</p>
"""
