/* @brief Global state manager for dropdowns, hides on not clicked.
* 01/08/2018
* R.G.Bamford
*/

Editor.onPageClick = function(e)
{
	/* @brief static list of elements which are dropdown buttons and their menus. */
	var dropdownElements =
	[
		{
			dropdownButton:"toolbar_file_button", dropdownMenu:"toolbar_file_dropdown_menu"
		}
	];

	var eventPath = e.originalEvent.path;

	// set all dropdowns to false except the one we have clicked.
	for (var dropdownElementIndex = 0; dropdownElementIndex < eventPath.length - 1; dropdownElementIndex++)
	{
		var dropdownObject = dropdownElements[dropdownElementIndex];
		if (dropdownObject == undefined) break;


		// if we have clicked on a dropdown button, do not hide it.
		var shouldHideDropdownObject = true;
		for (var elementIndex = 0; elementIndex < eventPath.length; elementIndex++)
		{
			var thisElementInEventPath = eventPath[elementIndex];
			var thisElementInEventPathClassName = thisElementInEventPath.className;
			if (thisElementInEventPathClassName == dropdownObject.dropdownButton)
			{
				shouldHideDropdownObject = false;
			}
		}

		if (shouldHideDropdownObject)
		{
			$("." + dropdownObject.dropdownMenu).hide();
		}
	}
}

$(document).click(function(e)
{
	Editor.onPageClick(e);
});
