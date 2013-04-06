%include: tpl_header title='info', load_graph=load_graph

<form action="/config" method="POST">
  <fieldset>
    <legend>Plotting</legend>
    <table>
      <tr>
        <td><input type="checkbox" name="fillGraph"{{" checked" if bool(config['fillGraph']) else ""}}></td>
        <td>Fill area under graph</td>
      </tr>
      <tr>
        <td><input type="checkbox" name="animatedZooms"{{" checked" if bool(config['animatedZooms']) else ""}}></td>
        <td>Animated zooms</td>
      </tr>
      <tr>
        <td><input type="checkbox" name="showRangeSelector"{{" checked" if bool(config['showRangeSelector']) else ""}}></td>
        <td>Show range selector</td>
      </tr>
    </table>
  </fieldset>
  <fieldset>
    <legend>Basestation</legend>
    <table>
      <tr>
        <th>Sensor</th>
        <th>Type</th>
        <th>Impulses/Unit</th>
        <th>Unit</th>
        <th>Combined impulses</th>
	<th>Color</th>
      </tr>
      <tr>
        <td>S0</td>
        <td>
	  <select name="s0type">
	    <option{{" selected" if config['s0type'] == 'disabled' else ""}}>disabled</option>
	    <option{{" selected" if config['s0type'] == 'electricity' else ""}}>electricity</option>
	    <option{{" selected" if config['s0type'] == 'water' else ""}}>water</option>
	    <option{{" selected" if config['s0type'] == 'gas' else ""}}>gas</option>
	  </select>
	</td>
        <td><input type="number" name="s0impulses" min="1" max="255" step="1" value={{config['s0impulses']}}></td>
        <td><input type="text" name="s0unit" size="10" maxlength="10" value={{config['s0unit']}}></td>
        <td><input type="number" name="s0combined" min="0" max="255" step="1" value={{config['s0combined']}}></td>
        <td>
	  <select name="s0color">
	  </select>
	</td>
      </tr>
      <tr>
        <td>S1</td>
        <td>
	  <select name="s1type">
	    <option{{" selected" if config['s1type'] == 'disabled' else ""}}>disabled</option>
	    <option{{" selected" if config['s1type'] == 'electricity' else ""}}>electricity</option>
	    <option{{" selected" if config['s1type'] == 'water' else ""}}>water</option>
	    <option{{" selected" if config['s1type'] == 'gas' else ""}}>gas</option>
	  </select>
	</td>
        <td><input type="number" name="s1impulses" min="1" max="255" step="1" value={{config['s1impulses']}}></td>
        <td><input type="text" name="s1unit" size="10" maxlength="10" value={{config['s1unit']}}></td>
        <td><input type="number" name="s1combined" min="0" max="255" step="1" value={{config['s1combined']}}></td>
        <td>
	  <select name="s1color">
	  </select>
	</td>
      </tr>
      <tr>
        <td>S2</td>
        <td>
	  <select name="s2type">
	    <option{{" selected" if config['s2type'] == 'disabled' else ""}}>disabled</option>
	    <option{{" selected" if config['s2type'] == 'electricity' else ""}}>electricity</option>
	    <option{{" selected" if config['s2type'] == 'water' else ""}}>water</option>
	    <option{{" selected" if config['s2type'] == 'gas' else ""}}>gas</option>
	  </select>
	</td>
        <td><input type="number" name="s2impulses" min="1" max="255" step="1" value={{config['s2impulses']}}></td>
        <td><input type="text" name="s2unit" size="10" maxlength="10" value={{config['s2unit']}}></td>
        <td><input type="number" name="s2combined" min="0" max="255" step="1" value={{config['s2combined']}}></td>
        <td>
	  <select name="s2color">
	  </select>
	</td>
      </tr>
      <tr>
        <td>S3</td>
        <td>
	  <select name="s3type">
	    <option{{" selected" if config['s3type'] == 'disabled' else ""}}>disabled</option>
	    <option{{" selected" if config['s3type'] == 'electricity' else ""}}>electricity</option>
	    <option{{" selected" if config['s3type'] == 'water' else ""}}>water</option>
	    <option{{" selected" if config['s3type'] == 'gas' else ""}}>gas</option>
	  </select>
	</td>
        <td><input type="number" name="s3impulses" min="1" max="255" step="1" value={{config['s3impulses']}}></td>
        <td><input type="text" name="s3unit" size="10" maxlength="10" value={{config['s3unit']}}></td>
        <td><input type="number" name="s3combined" min="0" max="255" step="1" value={{config['s3combined']}}></td>
        <td>
	  <select name="s3color">
	  </select>
	</td>
      </tr>
    </table>
  </fieldset>
  <br>
  <input type="submit" name="config" value="Save settings">
  <input type="reset" value="Cancel">
  %if update:
  Data updated.
  %end
</form>
%include tpl_footer
