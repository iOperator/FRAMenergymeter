%include: tpl_header title='Energy', load_graph=load_graph
%from basestation_lib import *
<h2>Energy report</h2>
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
  %for datum in data:
    %i += 1
    %dstr = datum_string(datum[1])
  + "{{dstr}},{{i}}\n"
  %end
  , {
    {{"fillGraph: true, " if config['fillGraph'] == 'true' else ""}}
    {{"animatedZooms: true, " if config['animatedZooms'] == 'true' else ""}}
    {{"showRangeSelector: true, " if config['showRangeSelector'] == 'true' else ""}}
  }
  );
</script>
%end
%include tpl_footer
