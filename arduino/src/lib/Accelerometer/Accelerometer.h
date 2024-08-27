#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include "../Constants/ErrorCodes.h"
#include "../SensorReading/SensorReading.h"

class Accelerometer
{
public:
  Accelerometer(float threshold);
  ErrorCode initialize();
  ErrorCode zero();
  ErrorCode isActive(bool &active);

private:
  SensorReading reading;
  ErrorCode readAcceleration();
  ErrorCode maxMagnitudeSample(size_t sampleSize);
  float x, y, z;
  float xZero, yZero, zZero, magnitudeZero;
  bool initialized;
  float threshold;
};

#endif