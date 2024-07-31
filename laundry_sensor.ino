#include <Arduino_LSM6DS3.h>

const int BAUD_RATE = 9600;
const int POLLING_INTERVAL = 1; // polling interval in ms

void setup()
{
  Serial.begin(BAUD_RATE);
  while (!Serial)
    ; // wait for serial to be ready

  if (!IMU.begin())
  {
    Serial.println("Failed to initialize IMU, halting.");
    while (1)
      ; // indefinite pause
  }

  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
  Serial.println();
  Serial.println("Acceleration in g's:");
  Serial.println("(x, y, z)");
}

void printAcceleration(float x, float y, float z)
{
  Serial.print("(");
  Serial.print(x);
  Serial.print(", ");
  Serial.print(y);
  Serial.print(", ");
  Serial.print(z);
  Serial.print(")\n");
}

void loop()
{
  float x, y, z;

  if (IMU.accelerationAvailable())
  {
    IMU.readAcceleration(x, y, z);
    printAcceleration(x, y, z);
  }
}
