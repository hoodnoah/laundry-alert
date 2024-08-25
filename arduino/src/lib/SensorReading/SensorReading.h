#ifndef SENSOR_READING_H
#define SENSOR_READING_H

// external lib
#include <Arduino.h>
#include <ArduinoJson.h>

struct SensorReading : public Printable
{
  unsigned long timeStampMs;
  float x;
  float y;
  float z;

  SensorReading(unsigned long timeStampMs = 0, float x = 0.0, float y = 0.0, float z = 0.0) : timeStampMs(timeStampMs), x(x), y(y), z(z) {}
  size_t printTo(Print &p) const override
  {
    size_t n = 0;
    n += p.print("(");
    n += p.print(x);
    n += p.print(", ");
    n += p.print(y);
    n += p.print(", ");
    n += p.print(z);
    n += p.print(")");
    return n;
  }

  String toJson() const
  {
    StaticJsonDocument<200> doc;
    doc["timeStampMs"] = timeStampMs;
    doc["x"] = x;
    doc["y"] = y;
    doc["z"] = z;

    String jsonString;
    serializeJson(doc, jsonString);
    return jsonString;
  }
};

SensorReading SensorReading_new();

#endif