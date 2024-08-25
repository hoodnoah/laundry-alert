#ifndef WEBSOCKETCONNECTION_H
#define WEBSOCKETCONNECTION_H

// external lib
#include "WebSocketsClient.h"

// constants
#include "../Constants/ErrorCodes.h"

// local lib
#include "../SensorReading/SensorReading.h"

class WebSocketConnection
{
public:
  // default ctor
  WebSocketConnection();

  // public methods
  void begin(const IPAddress ip, const char *endpoint, const uint16_t port);
  void loop();
  void sendSensorReading(const SensorReading &reading);

private:
  WebSocketsClient wsClient;

  // private methods
  void webSocketEventHandler(WStype_t type, uint8_t *payload, size_t length);
};

#endif