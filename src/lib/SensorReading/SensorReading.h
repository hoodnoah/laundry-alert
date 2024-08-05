#ifndef SENSOR_READING_H
#define SENSOR_READING_H

struct SensorReading
{
  float x;
  float y;
  float z;

  SensorReading(float x = 0.0, float y = 0.0, float z = 0.0) : x(x), y(y), z(z) {}
};

SensorReading SensorReading_new();

#endif