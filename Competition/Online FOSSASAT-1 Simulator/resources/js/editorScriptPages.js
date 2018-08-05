Editor.scriptPages = [];
Editor.activeScriptName = "";

Editor.editingDiv.getSession().on("change", function(event)
{
	// take contents of the editor.
	var contents = Editor.editingDiv.getValue();
	
	var name = Editor.activeScriptName;
	
	if (name == "") return;
	
	// replace the text in the Editor.scriptPages[] object
	var scriptPage = Editor.setScriptPageContents(name, contents);
});

Editor.updateDOM = function ()
{			
	// clear tab bar area.
	$(".file_tabbar").empty();

	// page already exist?
	for (var pageIndex = 0; pageIndex < Editor.scriptPages.length; pageIndex++)
	{
		// for each script given in drop.
		var scriptPage = Editor.scriptPages[pageIndex];
		var scriptPageFilename = scriptPage.filename;
		scriptPageFilename = scriptPageFilename.replace(".", "♥");

		var tabHtml = "<div id='" + scriptPageFilename + "_tab' class='file_tabbar_tab'>" +
			"<div class='file_tabbar_tab_page_icon'>" +
			"<img src='resources/icons/script_code_red.png'>" +
			"</div>" +
			"<div class='file_tabbar_tab_text'>" + scriptPageFilename.replace("♥", ".") + "</div>" +
			"</div>";

		$(".file_tabbar").append(tabHtml);

		$(".file_tabbar_tab").click(function()
		{
			Editor.onTabBarTabPressed(this);
		});
	}

	// get the first page and set it to "open"/shown.
	// put contents into the editor
	if (Editor.scriptPages.length > 0)
	{
		Editor.editingDiv.setValue(Editor.scriptPages[0].contents, -1);
		Editor.activeScriptName = Editor.scriptPages[0].filename;
	}
};

Editor.doesScriptPageExist = function (scriptPageName)
{
	// page already exist?
	for (var pageIndex = 0; pageIndex < Editor.scriptPages.length; pageIndex++)
	{
		var scriptPage = Editor.scriptPages[pageIndex];
		var scriptPageFilename = scriptPage.filename;

		if (scriptPageFilename == scriptPageName)
		{
			return true;		
		}
	}

	return false;
};

Editor.getScriptPage = function (scriptPageName)
{
	for (var pageIndex = 0; pageIndex < Editor.scriptPages.length; pageIndex++)
	{
		var scriptPage = Editor.scriptPages[pageIndex];
		var scriptPageFilename = scriptPage.filename;

		// found the script filename in the pages array, return its object.
		if (scriptPageFilename == scriptPageName)
		{
			return Editor.scriptPages[pageIndex];		
		}
	}
};

Editor.setScriptPageContents = function (scriptPageName, scriptPageContents)
{
	var scriptPage = Editor.getScriptPage(scriptPageName);

	scriptPage.contents = scriptPageContents;
};

Editor.dropHandler = function (e)
{
	e.preventDefault();			

	if (e.dataTransfer.items)
	{
		// for each file in the dropped file array.
		for (var i = 0; i < e.dataTransfer.items.length; i++)
		{
			var transferItem = e.dataTransfer.items[i];
			if (transferItem.kind === "file")
			{
				var transferFile = transferItem.getAsFile();
				var reader = new FileReader();

				// Closure to capture the file information.
				reader.onload = (function(theFile)
				{
					return function(e)
					{			
						var filename = theFile.name;
						filename = filename.replace(".", "♥");
						var contents = e.currentTarget.result;

						// does this page already exist?
						if (Editor.doesScriptPageExist(filename))
						{
							// update the contents.
							var scriptPage = Editor.getScriptPage(filename);
							scriptPage.contents = contents;
						}
						else
						{														
							Editor.scriptPages.push({
								filename: filename,
								contents: contents
							});
						}

						// update the DOM view.
						Editor.updateDOM();
					};
				})(transferFile);

			  reader.readAsText(transferFile);
			}
		}
	}
};

Editor.dragOverHandler = function (e)
{
	e.preventDefault();
};

// create first script page.
var startingScriptPage = { filename : "startup_page♥js", contents : "testing"};
Editor.scriptPages.push(startingScriptPage);
Editor.updateDOM();