#include <Arduino_LSM6DS3.h>

const int BAUD_RATE = 115200;
const int POLLING_INTERVAL = 1; // polling interval in ms
#define BUFFER_SIZE 200
#define OUTPUT_BUFFER_SIZE 53
const int combinedOutputSize = BUFFER_SIZE * (OUTPUT_BUFFER_SIZE + 1); // +1 for newline

typedef struct SensorReading
{
  unsigned long timestamp;
  float x;
  float y;
  float z;
} SensorReading;

void floatToFixedString(char *buffer, size_t bufferSize, float value)
{
  int scaledValue = (int)(value * 1000);
  snprintf(buffer, bufferSize, "%d.%03d", scaledValue / 1000, abs(scaledValue % 1000));
}

int sensorReadingToString(char *outputBuffer, size_t bufferSize, const SensorReading &sensorReading)
{
  char xBuffer[9], yBuffer[9], zBuffer[9];
  floatToFixedString(xBuffer, sizeof(xBuffer), sensorReading.x);
  floatToFixedString(yBuffer, sizeof(yBuffer), sensorReading.y);
  floatToFixedString(zBuffer, sizeof(zBuffer), sensorReading.z);
  return snprintf(outputBuffer, bufferSize, "%lu -> (%s, %s, %s)\n", sensorReading.timestamp, xBuffer, yBuffer, zBuffer);
}

// buffer of reading objects
SensorReading buffer[BUFFER_SIZE];
int bufferIndex = 0;

// reusable string buffer for a single reading
char outputBuffer[OUTPUT_BUFFER_SIZE + 1]; // +1 for newline

// reusable output string buffer to reduce Serial.print(...) calls
char combinedOutput[combinedOutputSize];

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

// flushes buffer to serial
void flush_buffer(SensorReading *buffer, int &bufferIndex)
{
  // set combinedOutput to empty string
  combinedOutput[0] = '\0';

  for (int i = 0; i < bufferIndex; i++)
  {
    int length = sensorReadingToString(outputBuffer, sizeof(outputBuffer), buffer[i]);
    if (length < 0)
    {
      Serial.println("error formatting sensor reading...");
      continue;
    }
    strncat(combinedOutput, outputBuffer, sizeof(combinedOutput) - strlen(combinedOutput) - 1);
  }

  Serial.print(combinedOutput);
  Serial.flush();
  bufferIndex = 0;
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
      // add to serial printing buffer
      buffer[bufferIndex].timestamp = currentMillis;
      buffer[bufferIndex].x = x;
      buffer[bufferIndex].y = y;
      buffer[bufferIndex].z = z;

      bufferIndex++;
    }
  }

  if (bufferIndex >= BUFFER_SIZE)
  {
    flush_buffer(buffer, bufferIndex);
  }
  delay(30);
}
