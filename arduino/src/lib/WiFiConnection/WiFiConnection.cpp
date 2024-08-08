// external libraries
#include "WiFiNINA.h"
#include "ArduinoJson.h"

// local lib headers
#include "WiFiConnection.h"
#include "../Constants/ErrorCodes.h"
#include "../Constants/Constants.h"
#include "../Secrets/Secrets.h"
#include "../Accelerometer/Accelerometer.h"

// Attempts to connect the wifi module to the provided connection information,
// fails after a set number of retries.
ErrorCode WiFiConnection_Connect(WiFiConnection &connection)
{
  int numTries = 0;

  // wifi module not connected/unavailable
  if (WiFi.status() == WL_NO_MODULE)
  {
    return ErrorCode::WiFiNoModule;
  }

  if (WiFi.firmwareVersion() < WIFI_FIRMWARE_LATEST_VERSION)
  {
    return ErrorCode::WiFiFirmwareOutdated;
  }

  while (connection.status != WL_CONNECTED)
  {
    if (numTries > WIFI_CONNECT_RETRY_LIMIT)
    {
      return ErrorCode::WiFiClientTimeout;
    }
    numTries++;

    connection.status = WiFi.begin(connection.ssid, connection.pass);
    delay(10000); // delay 10 seconds for connection to be established

    IPAddress dns(192, 168, 10, 5);
    WiFi.setDNS(dns); // set DNS server

    WiFi.hostByName(connection.url, connection.serverIP);
  }

  return ErrorCode::Success;
}

// prepares a json payload
ErrorCode WiFiConnection_SendStatus(WiFiConnection &connection, MachineState state)
{
  bool status = false;
  if (MachineState::ACTIVE == state)
  {
    status = true;
  }

  // create json
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["active"] = status;

  // serialize json into string
  String jsonString;
  serializeJson(jsonDoc, jsonString);

  // create HTTP POST request
  connection.httpClient.post(connection.urlPath, "application/json", jsonString);
  int statusCode = connection.httpClient.responseStatusCode();

  connection.httpClient.stop(); // close connection

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