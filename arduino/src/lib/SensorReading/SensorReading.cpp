#include <math.h>

#include "SensorReading.h"
#include "../Accelerometer/Accelerometer.h"
#include "../Constants/Constants.h"

SensorReading SensorReading_new()
{
  return SensorReading(0, 0.0, 0.0, 0.0);
}