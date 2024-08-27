// config
#include "src/lib/Config/Config.h"

// constants
#include "src/lib/Constants/Constants.h"
#include "src/lib/Constants/ErrorCodes.h"
#include "src/lib/Secrets/Secrets.h"

// local lib
#include "src/lib/Accelerometer/Accelerometer.h"
#include "src/lib/DebugPrint/DebugPrint.h"
#include "src/lib/SensorReading/SensorReading.h"
#include "src/lib/WasherState/WasherState.h"
#include "src/lib/WiFiConnection/WiFiConnection.h"

// Accelerometer struct
Accelerometer mtu = Accelerometer(ACCELEROMETER_THRESHOLD);

// wifi connection
WiFiConnection connection(NETWORK, NETWORK_PASS, HOSTNAME, HOSTPORT, ENDPOINT);
bool connectionGood = false;

// washer state machine
WasherState washerState = WasherState(COOLDOWN_MS);

bool momentarilyActive = false;

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
  pinMode(LED_BUILTIN, OUTPUT);

#ifdef DEBUG
  Serial.begin(BAUD_RATE);
  while (!Serial)
  {
    ;
  }
#endif

  // set up wifi connection
  ErrorCode init_result = connection.connect();
  if (ErrorCode::Success != init_result)
  {
    DEBUG_PRINTLN("Failed to initialize the wifi module.");
    flash_led();
  }

  connectionGood = true;
  DEBUG_PRINTLN("Set up wifi connection");

  init_result = mtu.initialize();

  if (ErrorCode::Success != init_result)
  {
    DEBUG_PRINTLN("Failed to initialize IMU, halting.");
    flash_led();
  }
  DEBUG_PRINTLN("Initialized IMU");

  ErrorCode zero_result = mtu.zero();
  if (ErrorCode::Success != zero_result)
  {
    DEBUG_PRINTLN("Failed to zero IMU, halting.");
    flash_led();
  }
  DEBUG_PRINTLN("Zeroed IMU.");

  DEBUG_PRINTLN("Sending initial status to server...");
  ErrorCode send_result = connection.sendStatus(washerState);
  if (ErrorCode::Success != send_result)
  {
    DEBUG_PRINTLN("Failed to send initial state to server.");
    flash_led();
  }

  DEBUG_PRINTLN("Sent initial state.");
}

void loop()
{
  ErrorCode readResult = mtu.isActive(momentarilyActive);

  // if reading procured
  if (ErrorCode::Success == readResult)
  {
    DEBUG_PRINT("Momentarily active: ");
    DEBUG_PRINTLN(momentarilyActive);

    bool changed = false;
    if (momentarilyActive)
    {
      changed = washerState.setActive();
    }
    else
    {
      changed = washerState.setIdle();
    }

    if (changed)
    {
      // log state
      DEBUG_PRINTLN(washerState);

      // send state to server
      ErrorCode sendResult = connection.sendStatus(washerState);

      if (ErrorCode::Success != sendResult)
      {
        DEBUG_PRINTLN("Failed to send reading to server.");
        pinMode(LED_BUILTIN, HIGH);
      }
      else
      {
        pinMode(LED_BUILTIN, LOW);
      }
    }
  }
}