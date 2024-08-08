#ifndef WIFI_H
#define WIFI_H

// external lib
#include "WiFiNINA.h"
#include "ArduinoHttpClient.h"

// local lib
#include "../Constants/ErrorCodes.h"
#include "../Accelerometer/Accelerometer.h"

// constants
#define MIN_WIFININA_FW_VERSION "1.8.14"

struct WiFiConnection
{
  const char *ssid;
  const char *pass;
  int status;
  const char *url;
  uint16_t port;
  const char *urlPath;
  WiFiClient wifiClient;
  HttpClient httpClient;

  WiFiConnection(const char *ssid, const char *pass, const char *url, const uint16_t port, const char *urlPath) : ssid(ssid), pass(pass), url(url), port(port), urlPath(urlPath), wifiClient(), httpClient(HttpClient(wifiClient, url, port)) {}
};

ErrorCode WiFiConnection_Connect(WiFiConnection &connection);
ErrorCode WiFiConnection_SendStatus(WiFiConnection &connection, MachineState state);

#endif