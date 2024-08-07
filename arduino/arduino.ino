// constants
#include "src/lib/Constants/Constants.h"
#include "src/lib/Constants/ErrorCodes.h"

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
WiFiConnection connection;

void setup()
{
  Serial.begin(BAUD_RATE);
  while (!Serial)
    ; // wait for serial to be ready

  ErrorCode init_result = Accelerometer_initialize(mtu);

  if (ErrorCode::IMUInitializationFailure == init_result)
  {
    Serial.println("Failed to initialize IMU, halting.");
    while (1)
      ; // indefinite pause, would be a good time to have some kind of flashing error message (flash red led in a pattern?)
  }

  // wait 10 seconds to account for finishing plugging in/perturbing device
  // to allow for accurate zeroing
  delay(10000);

  // find the accelerometer's zero
  Accelerometer_zero(mtu, ZEROING_SAMPLE_SIZE);

  // initialize the rolling window and machine's state
  window = Window_new(SAMPLE_WINDOW_SIZE);
  state = MachineState::INACTIVE;

  // set up wifi connection
  connection = WiFiConnection(hostName, hostPort, endPoint);
  init_result = WiFiConnection_Initialize(connection);
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
  else if (ErrorCode::Success != init_result)
  {
    Serial.println("Failed to initialize the wifi module: Unspecified error.");
    while (1)
      ;
  }
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

    // evaluate the rolling window for overall activity
    if (Window_IsActive(window))
    {
      if (state != MachineState::ACTIVE) // only alter the state if it's a change
      {
        Serial.println("ACTIVE");
        state = MachineState::ACTIVE;
        WiFiConnection_SendStatus(connection, state);
      }
    }
    else
    {
      if (state != MachineState::INACTIVE)
      {
        Serial.println("INACTIVE");
        state = MachineState::INACTIVE;
        WiFiConnection_SendStatus(connection, state);
      }
    }
  }
}
