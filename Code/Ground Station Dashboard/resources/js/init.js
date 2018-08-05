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