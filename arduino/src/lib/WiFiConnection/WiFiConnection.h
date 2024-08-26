#ifndef WIFI_H
#define WIFI_H

// external lib
#include "WiFiNINA.h"
#include "ArduinoHttpClient.h"
#include "ArduinoJson.h"

// local lib
#include "../Constants/ErrorCodes.h"
#include "../Accelerometer/Accelerometer.h"
#include "../WasherState/WasherState.h"

// constants
#define MIN_WIFININA_FW_VERSION "1.8.14"

class WiFiConnection
{
public:
  WiFiConnection(const char *ssid, const char *url, const uint16_t port, const char *urlPath);
  ErrorCode connect();
  ErrorCode sendStatus(WasherState &state);

private:
  const char *ssid, *pass, *url, *urlPath;
  int status;
  IPAddress serverIP;
  uint16_t port;
  WiFiClient wifiClient;
  HttpClient httpClient;
  StaticJsonDocument<200> jsonDoc;
};

// struct WiFiConnection
// {
//   const char *ssid;
//   const char *pass;
//   int status;
//   const char *url;
//   IPAddress serverIP;
//   uint16_t port;
//   const char *urlPath;
//   WiFiClient wifiClient;
//   HttpClient httpClient;
//   StaticJsonDocument<200> jsonDoc;

//   WiFiConnection(const char *ssid, const char *pass, const char *url, const uint16_t port, const char *urlPath) : ssid(ssid), pass(pass), url(url), port(port), urlPath(urlPath), wifiClient(), httpClient(HttpClient(wifiClient, url, port)) {}
// };

// ErrorCode WiFiConnection_Connect(WiFiConnection &connection);
// ErrorCode WiFiConnection_SendStatus(WiFiConnection &connection, MachineState state);
// ErrorCode WiFiConnection_SendReading(WiFiConnection &connection, SensorReading &reading, unsigned long timeStampMS);

#endif