#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include "../Constants/ErrorCodes.h"
#include "../SensorReading/SensorReading.h"

enum class MachineState
{
  INACTIVE = 0,
  ACTIVE,
  NOT_READY
};

struct AccelerometerZero
{
  float mean;
  float std;

  AccelerometerZero() : mean(0.0), std(0.0) {}
};

struct Accelerometer
{
  SensorReading reading;
  AccelerometerZero zero;
  bool isInitialized;
  bool isZeroed;

  Accelerometer() : reading(SensorReading()), zero(AccelerometerZero()), isInitialized(false), isZeroed(false) {}
};

ErrorCode Accelerometer_initialize(Accelerometer &mtu);
MachineState Accelerometer_try_get_state(Accelerometer &mtu);
ErrorCode Accelerometer_Read(SensorReading &outputReading);
float Accelerometer_get_sample_rate();
void Accelerometer_zero(Accelerometer &mtu, int numObservations);

#endif