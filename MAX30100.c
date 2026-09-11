#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

#define REPORTING_PERIOD_MS 1000

PulseOximeter pox;

uint32_t tsLastReport = 0;

void onBeatDetected()
{
    Serial.println("Beat!");
}

void setup()
{
    Serial.begin(115200);
    delay(1000);

    // MAX30100 I2C
    // SDA = GPIO 21
    // SCL = GPIO 20
    Wire.begin(21, 20);

    Serial.println();
    Serial.println("Initializing pulse oximeter..");

    if (!pox.begin()) {
        Serial.println("FAILED");

        while (1) {
            delay(1000);
        }
    }

    Serial.println("SUCCESS");

    pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop()
{
    pox.update();

    if (millis() - tsLastReport > REPORTING_PERIOD_MS)
    {
        Serial.print("Heart rate: ");
        Serial.print(pox.getHeartRate());

        Serial.print(" bpm / SpO2: ");
        Serial.print(pox.getSpO2());

        Serial.println(" %");

        tsLastReport = millis();
    }
}