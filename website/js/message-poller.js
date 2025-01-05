'use strict';

// A MessagePoller continuously polls the server for messages addressed to this
// recipient, and allows sending messages to other recipients.
//
// If g_websocket_url is a viable url, a WebSocket is used instead of continuous
// polling.  Messages are expressed as json strings and read or written to the
// websocket.  If using WebSockets, the MessagePoller sends an initial
// subscriber identification message, just {subscriber: recipient}.
//
// MessagePoller is used for webrtc communication between a viewer (replay
// kiosk) and a remote camera, when using a remote camera.  If websocket support
// is available, replay.php also creates an additional MessagePoller to receive
// replay commands, rather than poll (4x per second) for replay.message.

function make_id_string(stem) {
  var alphabet = "abcdefghjklmnpqrstuvwz0123456789";

  var id = stem;
  for (var k = 0; k < 12; ++k) {
    id += alphabet[Math.floor(Math.random() * alphabet.length)];
  }
  return id;
}

function MessagePoller(recipient, on_message, topics = []) {
  let poller = this;
  if (g_websocket_url != undefined && g_websocket_url != '') {
    var ws = new WebSocket(g_websocket_url);
    var closing = false;

    var init = function() {
      ws.onmessage = (event) => {
        try {
          var msg = JSON.parse(event.data);
          on_message(msg);
        } catch (e) {
          console.log('  Unable to parse json message:f, event.data');
        }
      };

      ws.onopen = (event) => {
        console.log('Websocket opened');
        poller.send_message({"subscriber": recipient, "topics": topics});
      };

      // If the connection can't be established, this gets called but with almost
      // no information in the error.
      ws.onerror = (event) => { console.error('Websocket error', event); };
      ws.onclose = (event) => {
        console.log('Websocket close event', event, ws.readyState);
        if (!closing) {
          // Try to reconnect, but allow a 10s cooling-off delay.
          setTimeout(function() {
            console.log('Attempting to re-establish websocket connection');
            ws = new WebSocket(g_websocket_url);
            init();
          }, 10000);
        }
      };
    };
    init();

    this.send_message = function(message_json) {
      if (ws.readyState === WebSocket.OPEN) {
        try {
          ws.send(JSON.stringify(message_json));
        } catch (e) {
          console.log('send_message error', e, e.code, e.name, e.message);  // TODO
        }
      }
    };
    this.close = function() {
      closing = true;
      if (ws) {
        ws.close();
        ws = null;
      }
    };
  } else {
    this.send_message = function(message_json) {
      $.ajax("action.php",
             {type: 'POST',
              data: {action: 'message.send',
                     message: message_json}});
    };

    this.retrieve_messages = function() {
      $.ajax("action.php",
             {type: 'POST',
              data: {action: 'message.retrieve',
                     recipient: recipient},
              success: function(data) {
                var msgs = data.messages;
                for (var i = 0; i < msgs.length; ++i) {
                  on_message(msgs[i]);
                }
              }
             });
    };

    let interval = setInterval(function() { poller.retrieve_messages(); }, 500 /* ms. */);

    this.close = function() { if (interval) clearInterval(interval); interval = null; };
 }
}
