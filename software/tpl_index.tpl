%include: tpl_header title='info', load_graph=load_graph
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
<p>Time and date of the device {{data['date']}}, {{data['time']}}</p>
%if data['data'] == []:
<p>No new data.</p>
%else:
<p>Getting new data ...</p>
  %# Testing only
<ul>
  %for line in data['data']:
  <li>{{line}}</li>
  %end
</ul>
<div id="graphdiv" style="width: 950px; heigth: 600px;"></div>
<script type="text/javascript">
  g = new Dygraph(
  // Containing DIV
  document.getElementById("graphdiv"),
  // Data
  "Time and date,{{','.join([entry[0] for entry in data['active_sensors']])}},\n"
  %i = 0
  %for datum in data['data']:
    %i += 1
    %datetime = datum[0:4] + '-' + datum[4:6] + '-' + datum[6:8] + ' ' + datum[8:10] + ':' + datum[10:12] + ':' + datum[12:14]
  + "{{datetime}},{{round(i/75.0, 3)}}\n"
  %end
  );
</script>
%end
%include tpl_footer
