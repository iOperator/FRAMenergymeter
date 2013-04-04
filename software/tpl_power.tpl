%include: tpl_header title='Power', load_graph=load_graph
%from basestation_lib import *
<h2>Power report</h2>
%# Get data from database
%db_results = db.execute("SELECT id, datum, sensor FROM data").fetchall()
%data = []
%for row in db_results:
  %data.append([row[0], row[1], row[2]])
%end
%if data == []:
<p>Database is empty.</p>
%else:
<div id="graphdiv" style="width: 950px; heigth: 600px;"></div>
<script type="text/javascript">
  g = new Dygraph(
  // Containing DIV
  document.getElementById("graphdiv"),
  // Data
  "Time and date,S0,\n"
  %i = 0
  %last_datum = data[0][1]
  %for datum in data:
    %i += 1
    %dstr = datum_string(datum[1])
    %timediff = datum_difference(last_datum, datum[1])
    %last_datum = datum[1]
    %if timediff == 0:
      %power = 1
    %else:
      %power = 1 / float(timediff)
    %end
  + "{{dstr}},{{power}}\n"
  %end
  , { fillGraph: true }  // fillGraph: true, animatedZooms: true, showRangeSelector: true
  );
</script>
%end
%include tpl_footer
