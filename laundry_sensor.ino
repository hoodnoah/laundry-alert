#include <Arduino_LSM6DS3.h>

#define BAUD_RATE 115200
#define OUTPUT_BUFFER_SIZE 54 // 53 chars + newling

typedef struct SensorReading
{
  unsigned long timestamp;
  float x;
  float y;
  float z;
} SensorReading;

// reusable reading struct
SensorReading reading = SensorReading{0, 0.0, 0.0, 0.0};

// reusable string buffer for a single reading
char outputBuffer[OUTPUT_BUFFER_SIZE]; // +1 for newline

// converts a float to a fixed point string w/ 3 significant figures
void floatToFixedString(char *buffer, size_t bufferSize, float value)
{
  int scaledValue = (int)(value * 1000);
  snprintf(buffer, bufferSize, "%d.%03d", scaledValue / 1000, abs(scaledValue % 1000));
}

// fills a string with a formatted version of the SensorReading struct
int sensorReadingToString(char *outputBuffer, size_t bufferSize, const SensorReading &sensorReading)
{
  char xBuffer[9], yBuffer[9], zBuffer[9];
  floatToFixedString(xBuffer, sizeof(xBuffer), sensorReading.x);
  floatToFixedString(yBuffer, sizeof(yBuffer), sensorReading.y);
  floatToFixedString(zBuffer, sizeof(zBuffer), sensorReading.z);
  return snprintf(outputBuffer, bufferSize, "%lu -> (%s, %s, %s)\n", sensorReading.timestamp, xBuffer, yBuffer, zBuffer);
}

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

void loop()
{
  float x, y, z;
  unsigned long currentMillis = 0;

  if (IMU.accelerationAvailable())
  {
    currentMillis = millis();
    int readResult = IMU.readAcceleration(x, y, z);
    if (readResult != 1)
    {
      Serial.print("Failed to read IMU acceleration with error code ");
      Serial.print(readResult);
      Serial.println();
    }
    else
    {
      reading.timestamp = currentMillis;
      reading.x = x;
      reading.y = y;
      reading.z = z;

      sensorReadingToString(outputBuffer, OUTPUT_BUFFER_SIZE, reading);
      Serial.print(outputBuffer);
      Serial.flush();
      outputBuffer[0] = '\0';
    }
  }
  delay(15);
}
