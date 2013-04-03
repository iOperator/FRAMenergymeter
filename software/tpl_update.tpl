%include: tpl_header title='Update', load_graph=load_graph
%if data['connection']:
<p>Connected to device.</p>
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
  %else:  # New data
    %if request.POST.get('store') == None:  # Show form
<!-- Form: save to database -->
<form action="/update" method="POST">
  <input type="submit" name="store" value="Store in database" />
</form>
    %else:  # Save data to database
<p>Storing data ...</p>
<ul id="device_data">
      %for datum in data['data']:
        %db.execute("INSERT INTO data (datum,sensor) VALUES (?,?)", (datum, 0))
  <li>{{datum}}</li>
      %end
</ul>
    %end
  %end
%else:  # Not connected to basestation
<p>Cannot connect to device.</p>
%end
%include tpl_footer
