// external libraries
#include "WiFiNINA.h"
#include "ArduinoJson.h"

// local lib headers
#include "WiFiConnection.h"
#include "../Constants/ErrorCodes.h"
#include "../Accelerometer/Accelerometer.h"

// secrets
#include "../../../secrets/Secrets.h"

ErrorCode WiFiConnection_Initialize(WiFiConnection &connection)
{
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
    connection.status = WiFi.begin(ssid, pass);
    delay(5000);
  }

  return ErrorCode::Success;
}

// prepares a json payload
ErrorCode prepJsonPostPayload(WiFiConnection &connection, bool status)
{
  // create json
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["active"] = status;

  // serialize json into string
  String jsonString;
  serializeJson(jsonDoc, jsonString);

  // create HTTP POST request
  connection.client.println("POST " + String(connection.urlPath) + " HTTP/1.1");
  connection.client.println("Host: " + String(hostName));
  connection.client.println("Content-Type: application/json");
  connection.client.print("Content-Length: ");
  connection.client.println(jsonString.length());
  connection.client.println();
  connection.client.println(jsonString);

  return ErrorCode::Success;
}

ErrorCode WiFiConnection_SendStatus(WiFiConnection &connection, MachineState state)
{
  bool status = false;
  if (MachineState::ACTIVE == state)
  {
    status = true;
  }

  // prepare request
  prepJsonPostPayload(connection, status);

  // send request
  if (!connection.client.connect(connection.urlPath, connection.port))
  {
    return ErrorCode::WiFiClientConnectFailed;
  }
  else
  {
    while (connection.client.connected())
    {
      if (connection.client.available())
      {
        String response = connection.client.readStringUntil('\n');
        Serial.println(response);
      }
    }
  }

  connection.client.stop();
  return ErrorCode::Success;
}