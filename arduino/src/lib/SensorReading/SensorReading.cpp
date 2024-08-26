#include <math.h>

#include "SensorReading.h"
#include "../Accelerometer/Accelerometer.h"
#include "../Constants/Constants.h"

SensorReading::SensorReading()
{
  this->timeStampMs = 0;
  this->x = 0.0;
  this->y = 0.0;
  this->z = 0.0;
}

void SensorReading::setTimeStampMs(unsigned long timeStampMs)
{
  this->timeStampMs = timeStampMs;
}

void SensorReading::setX(float x)
{
  this->x = x;
}

void SensorReading::setY(float y)
{
  this->y = y;
}

void SensorReading::setZ(float z)
{
  this->z = z;
}

float SensorReading::getTimeStampMs()
{
  return this->timeStampMs;
}

float SensorReading::getX()
{
  return this->x;
}

float SensorReading::getY()
{
  return this->y;
}

float SensorReading::getZ()
{
  return this->z;
}

// calculates and returns the magnitude of a reading
// based off its x, y, and z values
float SensorReading::getMagnitude()
{
  return sqrt(pow(this->x, 2) + pow(this->y, 2) + pow(this->z, 2));
}

size_t SensorReading::printTo(Print &p) const
{
  size_t n = 0;
  n += p.print(timeStampMs);
  n += p.print(": ");
  n += p.print("(");
  n += p.print(x);
  n += p.print(", ");
  n += p.print(y);
  n += p.print(", ");
  n += p.print(z);
  n += p.print(")");
  return n;
}