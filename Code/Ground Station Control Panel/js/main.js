$(document).ready(function() {
	/* Panel question marks */
	$(".panelQuestionMark").mouseenter(function() {
		$(".panelQuestionMark").text("Read Help");
		$(".panelQuestionMark").removeClass("blink");
		$(".panelQuestionMark").addClass("panelQuestionMark_Hovered");
	});

	$(".panelQuestionMark").mouseleave(function() {
		$(".panelQuestionMark").text("\u2713");
		$(".panelQuestionMark").removeClass("panelQuestionMark_Hovered");
	});

	$(".panelQuestionMark").click(function() {
		$(".panelQuestionMark").addClass("panelQuestionMark_Clicked");
		
		window.open('help.html');
	})
	
	/* Navigation bar */
	$(".controlPanelButton").click(function() {
		window.open('index.html', "_self");
	});
	
	$(".helpPageButton").click(function() {
		window.open("help.html", "_self");
	});
	
	$(".fossaPageButton").click(function() {
		window.open("http://fossa.systems", "_self");
	});
});