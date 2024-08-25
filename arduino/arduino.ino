// definitions
// #define DEBUG

// constants
#include "src/lib/Constants/Constants.h"
#include "src/lib/Constants/ErrorCodes.h"
#include "src/lib/Secrets/Secrets.h"

// local lib
#include "src/lib/Accelerometer/Accelerometer.h"
#include "src/lib/SensorReading/SensorReading.h"
#include "src/lib/WebSocketConnection/WebSocketConnection.h"
#include "src/lib/WiFiConnection/WiFiConnection.h"
#include "src/lib/DebugPrint/DebugPrint.h"

// Accelerometer struct
Accelerometer mtu;

// wifi connection
WiFiConnection connection(NETWORK, NETWORK_PASS, HOSTNAME, HOSTPORT, ENDPOINT);
bool connectionGood = false;

// websocket connection
WebSocketConnection wsConnection = WebSocketConnection();

// reusable reading struct
SensorReading reading;

void flash_led()
{
  bool toggle = false;
  pinMode(LED_BUILTIN, OUTPUT);

  while (true)
  {
    toggle = !toggle;
    if (toggle)
    {
      digitalWrite(LED_BUILTIN, HIGH);
    }
    else
    {
      digitalWrite(LED_BUILTIN, LOW);
    }
    delay(1000);
  }
}

void setup()
{
#ifdef DEBUG
  Serial.begin(BAUD_RATE);
  while (!Serial)
  {
    ;
  }
#endif

  pinMode(LED_BUILTIN, OUTPUT);

  ErrorCode init_result = Accelerometer_initialize(mtu);

  if (ErrorCode::IMUInitializationFailure == init_result)
  {
    DEBUG_PRINTLN("Failed to initialize IMU, halting.");
    flash_led();
  }

  // set up wifi connection
  init_result = WiFiConnection_Connect(connection);
  if (ErrorCode::WiFiNoModule == init_result)
  {
    DEBUG_PRINTLN("Failed to initialize the wifi module: No Module Error.");
    flash_led();
  }
  else if (ErrorCode::WiFiFirmwareOutdated == init_result)
  {
    DEBUG_PRINTLN("Failed to initialize the wifi module: Firmware Outdated.");
    flash_led();
  }
  else if (ErrorCode::WiFiClientTimeout == init_result)
  {
    DEBUG_PRINTLN("Failed to initialize the wifi module: Timed out.");
    flash_led();
  }
  else if (ErrorCode::Success != init_result)
  {
    DEBUG_PRINTLN("Failed to initialize the wifi module: Unspecified error.");
    flash_led();
  }

  connectionGood = true;
  DEBUG_PRINTLN("Set up wifi connection");

  // set up websocket connection
  wsConnection.begin(connection.serverIP, "/ws", connection.port);
  DEBUG_PRINTLN("Set up ws connection");

  // initialize reusable reading struct
  reading = SensorReading_new();
}

void loop()
{
  wsConnection.loop();

  ErrorCode readResult = Accelerometer_Read(reading);

  // if reading procured
  if (ErrorCode::Success == readResult)
  {
    // try sending reading to server
    wsConnection.sendSensorReading(reading);
    DEBUG_PRINTLN("Sent reading.");
  }
  else if (ErrorCode::IMUNotReady != readResult)
  {
    DEBUG_PRINTLN("Failed to get accelerometer reading.");
  }
}