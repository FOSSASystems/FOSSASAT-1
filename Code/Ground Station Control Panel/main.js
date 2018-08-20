var SerialPort = require("serialport");

var ComPortsAvailable = [];

SerialPort.list(function(err, ports)
{
	ports.forEach(function(port)
	{
		ComPortsAvailable.push(port);
	});
});

var WebSocket = require("ws");

var wss = new WebSocket.Server({ port: 8080 });

wss.on("connection", function connection(ws)
{
	ws.on("message", function incoming(msg)
	{
		console.log("Received: %s", msg);
	});

	ws.send("Hello client, this is the server!");
});