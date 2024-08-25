#include "WebSocketConnection.h"

// external lib
#include "WebSocketsClient.h"

// constants
#include "../Constants/ErrorCodes.h"

// local lib
#include "../DebugPrint/DebugPrint.h"
#include "../SensorReading/SensorReading.h"

// default constructor
WebSocketConnection::WebSocketConnection() {}

// connects the ws client to the server,
// sets the event listener
void WebSocketConnection::begin(const IPAddress ip, const char *endpoint, const uint16_t port)
{
  // Initialize the websocket w/ the stored configuration variables
  this->wsClient.begin(ip, port, endpoint);

  this->wsClient.onEvent([this](WStype_t type, uint8_t *payload, size_t length)
                         { this->webSocketEventHandler(type, payload, length); });
}

// accessor for loop method
void WebSocketConnection::loop()
{
  this->wsClient.loop();
}

// event handler for webSocket events
void WebSocketConnection::webSocketEventHandler(WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    DEBUG_PRINTLN("[WSc] Disconnected");
    break;
  case WStype_CONNECTED:
    DEBUG_PRINTLN("[WSc] Connected");
    break;
  case WStype_TEXT:
    DEBUG_PRINT("[WSc] Received text: ");
    DEBUG_PRINTLN((char *)payload);
    break;
  case WStype_BIN:
  case WStype_ERROR:
  case WStype_FRAGMENT_TEXT_START:
  case WStype_FRAGMENT_BIN_START:
  case WStype_FRAGMENT:
  case WStype_PING:
  case WStype_PONG:
  case WStype_FRAGMENT_FIN:
    break;
  }
}

// directly sends a SensorReading to the server
void WebSocketConnection::sendSensorReading(const SensorReading &reading)
{
  String jsonData = reading.toJson();
  DEBUG_PRINTLN(jsonData);
  this->wsClient.sendTXT(jsonData);
}