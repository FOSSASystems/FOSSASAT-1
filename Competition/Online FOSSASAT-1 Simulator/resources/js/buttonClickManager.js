/* @brief This is the program that allows the tab bar to have clickable
* dropdowns.
* 01/08/2018
* R.G.Bamford
*/

Editor.onTabBarTabPressed = function (element)
{
	var scriptElementId = element.id;
	
	var filename = scriptElementId.substring(0, scriptElementId.length - 4);
	
	Editor.activeScriptName = filename;
	
	console.log("Active script name now: " + Editor.activeScriptName);
	
	var contents = Editor.getScriptPage(filename).contents;

	Editor.editingDiv.setValue(contents, -1);
};

Editor.onToolbarFileButtonClicked = function (e)
{
	var dropdownState = $(".toolbar_file_dropdown_menu").css("display");

	if (dropdownState == "none")
	{
		$(".toolbar_file_dropdown_menu").css("display", "flex");
	}
	else if (dropdownState == "flex")
	{
		$(".toolbar_file_dropdown_menu").css("display", "none");
	}
};

/* @brief not a dropdown */
Editor.onBottomBarSettingsButtonClicked = function (e)
{
	Editor.editingDiv.showSettingsMenu();
};

Editor.onBottomBarKeyboardShortcutsButtonClicked = function (e)
{
	Editor.editingDiv.showKeyboardShortcuts();
};

$(document).ready(function()
{
	/* If we have clicked the toggle button we want to toggle its display value */
	$(".toolbar_file_button").click(function(e)
	{
		Editor.onToolbarFileButtonClicked(e);
	});

	$(".toolbar_bottom_item_editor_settings").click(function(e)
	{
		Editor.onBottomBarSettingsButtonClicked(e);
	});
	
	$(".toolbar_bottom_item_keyboard_shortcuts").click(function(e)
	{
		Editor.onBottomBarKeyboardShortcutsButtonClicked(e);									   
	});
	  
});