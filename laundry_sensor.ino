const int PIEZO_PIN = A0;
const float ADC_WIDTH = 4095.0;
const float REFERENCE_VOLTAGE = 3.3;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  int piezoReading = analogRead(PIEZO_PIN);
  float piezoVoltage = piezoReading / ADC_WIDTH * REFERENCE_VOLTAGE;
  Serial.println(piezoVoltage);

  delay(250);
}
