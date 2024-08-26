#include "Arduino_LSM6DS3.h" // accelerometer lib

#include "Accelerometer.h"

#include "../SensorReading/SensorReading.h"
#include "../Constants/Constants.h"
#include "../Constants/ErrorCodes.h"

// constructor
Accelerometer::Accelerometer(float threshold)
{
  this->reading = SensorReading();
  this->threshold = threshold;
  this->x, this->y, this->z = 0.0;
  this->xZero, this->yZero, this->zZero, this->magnitudeZero = 0.0;
  this->initialized = false;
}

// attempts to begin the IMU
// returning ErrorCode::ImuInitializationFailure on fail,
// ErrorCode::Success on success
ErrorCode Accelerometer::initialize()
{
  if (!IMU.begin())
  {
    return ErrorCode::IMUInitializationFailure;
  }

  this->initialized = true;
  return ErrorCode::Success;
}

// reads the acceleration into a provided SensorReading object
// returns ErrorCode::IMUNotReady when acceleration is unavailable
// ErrorCode::IMUNotInitialized if the accelerometer has not been initialized
// ErrorCode::Success when the reading is taken
ErrorCode Accelerometer::readAcceleration()
{
  if (!this->initialized)
  {
    return ErrorCode::IMUNotInitialized;
  }

  if (!IMU.accelerationAvailable())
  {
    return ErrorCode::IMUNotReady;
  }

  IMU.readAcceleration(this->x, this->y, this->z);

  this->reading.setTimeStampMs(millis());
  this->reading.setX(this->x - this->xZero);
  this->reading.setY(this->y - this->yZero);
  this->reading.setZ(this->z - this->zZero);

  return ErrorCode::Success;
}

// zero the accelerometer; take the mean of several readings
// and store them as dopes for each acceleration vector dimension, to be
// used to isolate the *dynamic* acceleration in any dimension.
ErrorCode Accelerometer::zero()
{
  if (!this->initialized)
  {
    return ErrorCode::IMUNotInitialized;
  }

  // collect sample of readings
  size_t num_obs = 0;
  float sumX, sumY, sumZ, sumMagnitude = 0.0;

  while (num_obs < ACCELEROMETER_ZERO_OBSERVATIONS)
  {
    ErrorCode readResult = this->readAcceleration();
    if (ErrorCode::Success == readResult)
    {
      sumX += this->reading.getX();
      sumY += this->reading.getY();
      sumZ += this->reading.getZ();
      sumMagnitude += this->reading.getMagnitude();
    }
    else if (ErrorCode::IMUNotReady != readResult)
    {
      return readResult;
    }
  }

  // set vector dopes to mean of zeroing observations
  this->xZero = sumX / num_obs;
  this->yZero = sumY / num_obs;
  this->zZero = sumZ / num_obs;
  this->magnitudeZero = sumMagnitude / num_obs;

  return ErrorCode::Success;
}

// classifies the instantaneous state as active or not based on
// the threshold provided to the constructor.
ErrorCode Accelerometer::isActive(bool &active)
{
  ErrorCode readResult = this->readAcceleration();

  if ((ErrorCode::Success != readResult) || (ErrorCode::IMUNotReady != readResult))
  {
    return readResult;
  }

  // wait for reading to be ready, if it isn't yet
  while (ErrorCode::IMUNotReady == readResult)
  {
    ;
  }

  active = abs(this->reading.getMagnitude()) > this->threshold;
  return ErrorCode::Success;
}