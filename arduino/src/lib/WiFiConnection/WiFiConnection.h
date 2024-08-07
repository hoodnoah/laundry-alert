#ifndef WIFI_H
#define WIFI_H

// external lib
#include "WiFiNINA.h"

// local lib
#include "../Constants/ErrorCodes.h"
#include "../Accelerometer/Accelerometer.h"

// secrets
#include "../../../secrets/Secrets.h"

// constants
#define MIN_WIFININA_FW_VERSION "1.8.14"

struct WiFiConnection
{
  int status;
  const char *url;
  uint16_t port;
  const char *urlPath;
  WiFiClient client;

  WiFiConnection(const char *url = hostName, uint16_t port = hostPort, const char *urlPath = endPoint) : url(url), port(hostPort), urlPath(endPoint), status(0) {}
};

ErrorCode WiFiConnection_Initialize(WiFiConnection &connection);
ErrorCode WiFiConnection_SendStatus(WiFiConnection &connection, MachineState state);

#endif