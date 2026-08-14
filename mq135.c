const int sensorPin = 5; 

void setup() {
  Serial.begin(115200);
  pinMode(sensorPin, INPUT); 
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
}

void loop() {
  int adc = analogRead(sensorPin);
  float voltage = adc * (3.3 / 4095.0);

  Serial.print("ADC = ");
  Serial.print(adc);
  Serial.print("   Voltage = ");
  Serial.println(voltage);

  delay(1000);
}