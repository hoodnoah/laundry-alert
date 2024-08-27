// external libraries
#include "WiFiNINA.h"
#include "ArduinoHttpClient.h"
#include "ArduinoJson.h"

#include "../Config/Config.h"

// local lib headers
#include "WiFiConnection.h"
#include "../Constants/ErrorCodes.h"
#include "../Constants/Constants.h"
#include "../Secrets/Secrets.h"
#include "../Accelerometer/Accelerometer.h"
#include "../DebugPrint/DebugPrint.h"

WiFiConnection::WiFiConnection(const char *ssid, const char *pass, const char *url, const uint16_t port, const char *urlPath)
    : wifiClient(), httpClient(wifiClient, url, port)
{
  this->ssid = ssid;
  this->pass = pass;
  this->url = url;
  this->port = port;
  this->urlPath = urlPath;
  this->status = WL_DISCONNECTED;
}

ErrorCode WiFiConnection::connect()
{
  int numTries = 0;
  if (WL_NO_MODULE == WiFi.status())
  {
    return ErrorCode::WiFiNoModule;
  }
  else if (WiFi.firmwareVersion() < WIFI_FIRMWARE_LATEST_VERSION)
  {
    return ErrorCode::WiFiFirmwareOutdated;
  }

  while (WL_CONNECTED != this->status)
  {
    if (numTries > WIFI_CONNECT_RETRY_LIMIT)
    {
      return ErrorCode::WiFiClientTimeout;
    }
    numTries++;

    this->status = WiFi.begin(this->ssid, this->pass);
    delay(7000);

    // lookup url w/ dns
    IPAddress dns(192, 168, 10, 5);
    WiFi.setDNS(dns);
    WiFi.hostByName(this->url, this->serverIP);

    // set timeout
    this->httpClient.setTimeout(5000);
  }

  return ErrorCode::Success;
}

ErrorCode WiFiConnection::sendStatus(WasherState &state)
{
  bool status = false;
  if (Activity::ACTIVE == state.getState())
  {
    status = true;
  }

  this->jsonDoc["active"] = status;
  String jsonString;
  serializeJson(this->jsonDoc, jsonString);

  this->httpClient.post(this->urlPath, "application/json", jsonString);
  int statusCode = this->httpClient.responseStatusCode();
  this->httpClient.stop(); // close connection

  if (202 == statusCode)
  {
    return ErrorCode::Success;
  }
  else if (400 == statusCode)
  {
    return ErrorCode::HttpClientPostRejected;
  }

  return ErrorCode::HttpClientPostFailed;
}