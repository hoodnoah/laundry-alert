#include "Arduino_LSM6DS3.h" // accelerometer lib

#include "Accelerometer.h"

#include "../SensorReading/SensorReading.h"
#include "../Constants/Constants.h"
#include "../Constants/ErrorCodes.h"

ErrorCode Accelerometer_initialize(Accelerometer &mtu)
{
  if (!IMU.begin())
  {
    return ErrorCode::IMUInitializationFailure;
  }

  mtu.isInitialized = true;
  return ErrorCode::Success;
}

// Attempts to read accelerometer values into a provided
// SensorReading struct.
// Returns ErrorCode::IMUNotReady when the acceleration is unavailable,
// ErrorCode::Success when the reading is taken
ErrorCode _Accelerometer_try_read(SensorReading &outputReading)
{
  if (!IMU.accelerationAvailable())
  {
    return ErrorCode::IMUNotReady;
  }

  IMU.readAcceleration(outputReading.x, outputReading.y, outputReading.z);
  outputReading.timeStampMs = millis();

  return ErrorCode::Success;
}

ErrorCode Accelerometer_Read(SensorReading &outputReading)
{
  return _Accelerometer_try_read(outputReading);
}

int32_t floatToFixed(float value)
{
  return static_cast<int32_t>(round(value * 1000));
}

// classifies an accelerometer reading as "active" or "inactive"
// based on its magnitude and constant threshold values defined in
// src/lib/Constants/Constants.h
MachineState _Accelerometer_classify_state(float magnitude)
{
  int magnitude_fixed = floatToFixed(magnitude);

  if (magnitude_fixed > ACTIVE_UPPER_THRESHOLD || magnitude_fixed < ACTIVE_LOWER_THRESHOLD)
  {
    return MachineState::ACTIVE;
  }

  return MachineState::INACTIVE;
}

// finds a mean, and standard deviation for normalization of
// accelerometer readings in different orientations/scenarios
void Accelerometer_zero(Accelerometer &mtu, int numObservations)
{
  float mean = 0.0;
  float m2 = 0.0;
  int n = 0;

  for (int i = 0; i < numObservations; i++)
  {
    if (_Accelerometer_try_read(mtu.reading) == ErrorCode::Success)
    {
      float magnitude = sqrt(pow(mtu.reading.x, 2) + pow(mtu.reading.y, 2) + pow(mtu.reading.z, 2));
      n++;
      float delta = magnitude - mean;
      mean += delta / n;
      float delta2 = magnitude - mean;
      m2 += delta * delta2;
    }
  }

  mtu.zero.mean = mean;
  mtu.zero.std = (n > 1) ? sqrt(m2 / (n - 1)) : 0.0;
  mtu.isZeroed = true;
}

float _Accelerometer_calc_normalized_magnitude(const AccelerometerZero &zero, const SensorReading &reading)
{
  float raw_magnitude = sqrt(pow(reading.x, 2) + pow(reading.y, 2) + pow(reading.z, 2));
  float normalized_magnitude = ((raw_magnitude - zero.mean) / zero.std) * BASELINE_STD + BASELINE_MEAN;
  return normalized_magnitude;
}

// attempts to get the state of the system from the accelerometer.
// returns a MachineState:
// ::ACTIVE -> machine read as active
// ::INACTIVE -> machine read as inactive
// ::NOT_READY -> accelerometer not ready to return a result
MachineState Accelerometer_try_get_state(Accelerometer &mtu)
{
  ErrorCode readResult = _Accelerometer_try_read(mtu.reading);

  if (ErrorCode::IMUNotReady == readResult)
  {
    return MachineState::NOT_READY;
  }

  float magnitude = _Accelerometer_calc_normalized_magnitude(mtu.zero, mtu.reading);

  return _Accelerometer_classify_state(magnitude);
}

float Accelerometer_get_sample_rate()
{
  return IMU.accelerationSampleRate();
}