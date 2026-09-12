#include <DHT.h>

#define DHT_PIN 21
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(115200);

  dht.begin();

  Serial.println("================================");
  Serial.println("     TEST SENSOR DHT11");
  Serial.println("     ESP32-S3 USB OTG");
  Serial.println("================================");
}

void loop() {
  delay(2000);

  float suhu = dht.readTemperature();
  float kelembapan = dht.readHumidity();

  if (isnan(suhu) || isnan(kelembapan)) {
    Serial.println("Gagal membaca DHT11!");
    return;
  }

  Serial.print("Suhu       : ");
  Serial.print(suhu);
  Serial.println(" °C");

  Serial.print("Kelembapan : ");
  Serial.print(kelembapan);
  Serial.println(" %");

  Serial.println("-------------------------------");
}