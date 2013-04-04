%include: tpl_header title='Energy', load_graph=load_graph
%from basestation_lib import *
<h2>Energy report</h2>
%# Get data from database
%db_results = db.execute("SELECT id, datum, sensor FROM data").fetchall()
%data = []
%for row in db_results:
  %data.append([row[0], row[1], row[2]])
%end
<div id="graphdiv" style="width: 950px; heigth: 600px;"></div>
<script type="text/javascript">
  g = new Dygraph(
  // Containing DIV
  document.getElementById("graphdiv"),
  // Data
  "Time and date,S0,\n"
  %i = 0
  %for datum in data:
    %i += 1
    %dstr = datum_string(datum[1])
  + "{{dstr}},{{i}}\n"
  %end
  , { fillGraph: true }  // fillGraph: true, animatedZooms: true, showRangeSelector: true
  );
</script>
%include tpl_footer