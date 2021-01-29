function init(){testWebSocket();}
  function testWebSocket()
  {
    websocket = new WebSocket("ws://localhost:4444");
    websocket.onopen = function(evt) { onOpen(evt) };
    websocket.onclose = function(evt) { onClose(evt) };
  }
  function onOpen(evt){doSend("{\"message-id\":\"1\",\"request-type\":\"SetCurrentScene\",\"scene-name\":\"Slideshow Scene\"}");}
  function onClose(evt){}
  function doSend(message){websocket.send(message);}
    window.addEventListener("load", init, false);