const int BAUD_RATE = 9600;
const int PIEZO_1 = A0;
const int PIEZO_2 = A3;
const float ADC_WIDTH = 4095.0;
const float REFERENCE_VOLTAGE = 3.3;
const int POLLING_INTERVAL = 1; // polling interval in ms

void setup()
{
  // set pins as input
  pinMode(PIEZO_1, INPUT);
  pinMode(PIEZO_2, INPUT);

  Serial.begin(BAUD_RATE);
}

// converts an analog reading to a voltage
float analogToVoltage(int analog)
{
  return analog / ADC_WIDTH * REFERENCE_VOLTAGE;
}

// prints readings in the format:
// Pin 1: 0.00, Pin 2: 0,00\n
void printReadings(float pin1Reading, float pin2Reading)
{
  Serial.print("Pin 1: ");
  Serial.print(pin1Reading);
  Serial.print(", ");
  Serial.print("Pin 2: ");
  Serial.print(pin2Reading);
  Serial.print("\n");
}

void loop()
{
  int pin1Reading = analogRead(PIEZO_1);
  int pin2Reading = analogRead(PIEZO_2);

  float pin1Voltage = analogToVoltage(pin1Reading);
  float pin2Voltage = analogToVoltage(pin2Reading);

  printReadings(pin1Voltage, pin2Voltage);

  delay(POLLING_INTERVAL);
}
