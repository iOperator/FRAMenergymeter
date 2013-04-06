%setdefault('load_graph', False)
<!DOCTYPE html>
<html lang="en">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta name="author" content="Max Gröning">
<title>FRAMenergymeter - {{title}}</title>
<link rel="stylesheet" href="static/style.css" type="text/css">
</head>
%if load_graph:
<script type="text/javascript" src="static/dygraph-combined.js"></script>
%end
<body>
<div id="content">
<div id="header"><a href="/"><span class="title">FRAMenergymeter</span></a>
<ul id="navbar">
  <li><a href="/">Home</a></li>
  <li><a href="/power">Power</a></li>
  <li><a href="/energy">Energy</a></li>
  <li><a href="/update">Update</a></li>
  <li><a href="/config">Configuration</a></li>
</ul>
<hr></div>
