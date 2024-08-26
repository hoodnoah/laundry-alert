#ifndef SENSOR_READING_H
#define SENSOR_READING_H

// external lib
#include <Arduino.h>
#include <ArduinoJson.h>

class SensorReading : public Printable
{
public:
  SensorReading();
  size_t printTo(Print &p) const;

  // setters
  void setTimeStampMs(unsigned long timeStampMs);
  void setX(float x);
  void setY(float y);
  void setZ(float z);

  // getters
  float getTimeStampMs();
  float getX();
  float getY();
  float getZ();
  float getMagnitude();

private:
  unsigned long timeStampMs;
  float x, y, z;
};

#endif