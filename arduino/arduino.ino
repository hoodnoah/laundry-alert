// constants
#include "src/lib/Constants/Constants.h"
#include "src/lib/Constants/ErrorCodes.h"
#include "src/lib/Secrets/Secrets.h"

// local lib
#include "src/lib/Accelerometer/Accelerometer.h"
#include "src/lib/SensorReading/SensorReading.h"
#include "src/lib/Window/Window.h"
#include "src/lib/WiFiConnection/WiFiConnection.h"

// State, ::ACTIVE ::INACTIVE
MachineState state;

// Accelerometer struct
Accelerometer mtu;

// circular window manager
Window window;

// wifi connection
WiFiConnection connection(NETWORK, NETWORK_PASS, HOSTNAME, HOSTPORT, ENDPOINT);

void setup()
{
  Serial.begin(BAUD_RATE);
  while (!Serial)
    ; // wait for serial to be ready

  // Set up built in LED as running/idle status output
  pinMode(LED_BUILTIN, OUTPUT);

  ErrorCode init_result = Accelerometer_initialize(mtu);

  if (ErrorCode::IMUInitializationFailure == init_result)
  {
    Serial.println("Failed to initialize IMU, halting.");
    while (1)
      ; // indefinite pause, would be a good time to have some kind of flashing error message (flash red led in a pattern?)
  }

  // set up wifi connection
  init_result = WiFiConnection_Connect(connection);
  if (ErrorCode::WiFiNoModule == init_result)
  {
    Serial.println("Failed to initialize the wifi module: No Module Error.");
    while (1)
      ;
  }
  else if (ErrorCode::WiFiFirmwareOutdated == init_result)
  {
    Serial.println("Failed to initialize the wifi module: Firmware Outdated.");
    while (1)
      ;
  }
  else if (ErrorCode::WiFiClientTimeout == init_result)
  {
    Serial.println("Failed to initialize the wifi module: Timed out.");
    while (1)
      ;
  }
  else if (ErrorCode::Success != init_result)
  {
    Serial.println("Failed to initialize the wifi module: Unspecified error.");
    while (1)
      ;
  }

  Serial.println("Set up wifi connection");

  // find the accelerometer's zero
  Accelerometer_zero(mtu, ZEROING_SAMPLE_SIZE);

  Serial.println("Zeroed accelerometer");

  // initialize the rolling window and machine's state
  window = Window_new(SAMPLE_WINDOW_SIZE);
  state = MachineState::INACTIVE;
}

void loop()
{
  // read the instantaneous state of the machine
  MachineState momentaryState = Accelerometer_try_get_state(mtu);

  if (MachineState::NOT_READY != momentaryState)
  {
    bool active;
    if (MachineState::ACTIVE == momentaryState)
    {
      active = true;
    }
    else
    {
      active = false;
    }

    // push the new reading onto the rolling window
    Window_AddReading(window, active);

    // evaluate the rolling window for overall activity, note if it's a change from the last measurement
    bool currentlyActive = Window_IsActive(window);
    bool stateChanged = (currentlyActive && (MachineState::ACTIVE != state)) || (!currentlyActive && (MachineState::INACTIVE != state));

    if (stateChanged)
    {
      if (currentlyActive)
      {
        state = MachineState::ACTIVE;
        digitalWrite(LED_BUILTIN, HIGH);
        Serial.print("Sending state ACTIVE to server... ");
      }
      else
      {
        state = MachineState::INACTIVE;
        digitalWrite(LED_BUILTIN, LOW);
        Serial.print("Sending state INACTIVE to server... ");
      }

      ErrorCode sendResult = WiFiConnection_SendStatus(connection, state);

      if (ErrorCode::HttpClientPostRejected == sendResult)
      {
        Serial.print("Error 400: POST request rejected by server.\n");
      }
      else if (ErrorCode::Success != sendResult)
      {
        Serial.print("Error: POST request failed for unidentified reason.\n");
      }
      else if (ErrorCode::Success == sendResult)
      {
        Serial.print("Ok\n");
      }
    }
  }
}