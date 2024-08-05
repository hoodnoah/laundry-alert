#include <Arduino_LSM6DS3.h>

#include "src/lib/Constants/Constants.h"
#include "src/lib/Constants/ErrorCodes.h"

#include "src/lib/Accelerometer/Accelerometer.h"
#include "src/lib/SensorReading/SensorReading.h"
#include "src/lib/Window/Window.h"

// State, ::ACTIVE ::INACTIVE
MachineState state;

// Accelerometer struct
Accelerometer mtu;

// circular window manager
Window window;

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

  Accelerometer_zero(mtu, ZEROING_SAMPLE_SIZE);

  window = Window_new(SAMPLE_WINDOW_SIZE);
  state = MachineState::INACTIVE;
}

void loop()
{
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

    Window_AddReading(window, active);

    if (Window_IsActive(window))
    {
      if (state != MachineState::ACTIVE) // only alter the state if it's a change
      {
        Serial.println("ACTIVE");
        state = MachineState::ACTIVE;
      }
    }
    else
    {
      if (state != MachineState::INACTIVE)
      {
        Serial.println("INACTIVE");
        state = MachineState::INACTIVE;
      }
    }
  }
}
