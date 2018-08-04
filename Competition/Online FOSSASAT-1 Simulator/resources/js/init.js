/* @brief global variables */
Editor = {};

Editor.editingDiv = ace.edit("editor");

ace.require('ace/ext/settings_menu').init(Editor.editingDiv);
ace.require('ace/ext/language_tools');

ace.config.loadModule("ace/ext/keybinding_menu", function(module)
{
	module.init(Editor.editingDiv);
})

Editor.editingDiv.setTheme("ace/theme/monokai");
Editor.editingDiv.session.setMode("ace/mode/javascript");

Editor.editingDiv.setOptions({
	enableBasicAutocompletion: true,
	enableSnippets: true,
	enableLiveAutocompletion: true
});

Editor.editingDiv.getSession().on("change", function(event)
{
	// take contents of the editor.
	var contents = Editor.editingDiv.getValue();
	
	var name = Editor.activeScriptName;
	
	console.log("On changed: " + contents + " :" + name);
	
	if (name == "") return;
	
	// replace the text in the Editor.scriptPages[] object
	var scriptPage = Editor.setScriptPageContents(name, contents);
});