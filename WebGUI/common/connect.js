// setup websocket with callbacks
var ws;

function doStop()
{
    ws.send('stop\n');
}

function doStart()
{
    ws = new WebSocket('ws://' + location.host);
    
    ws.onopen = function() {
        console.log('CONNECT');
        for (var name in knobDict) ws.send(name + '=?\n');
        return false;
        };

    ws.onclose = function()
    {
        console.log('DISCONNECT:' + event.code);
        document.getElementById("connectBtn").innerHTML = "Connect";
        ws = null;
        return true;
    };

    ws.onmessage = function(event)
    {
        var parts = event.data.split('=');
        if (knobDict.hasOwnProperty(parts[0]))
        {
            knobDict[parts[0]](Number(event.data.split('=')[1]));
        }
        else console.log('Message: ' + event.data);
        return false;
    };
    
    document.getElementById("connectBtn").innerHTML = "Disconnect";
}

function sendValueUpdate(id, newValue)
{
    var msg = id + "=" + String(newValue);
    //console.log("Send: " + msg);
    if (ws != null) ws.send(msg + "\n");
};

function doConnect()
{
    if (ws == null) doStart();
    else doStop();
}
