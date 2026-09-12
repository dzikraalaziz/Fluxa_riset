#include <Wire.h>
#include <DHT.h>
#include "MAX30100_PulseOximeter.h"
#include "driver/i2s.h"

// =====================================================
// PIN SENSOR
// =====================================================

// DHT11
#define DHT_PIN 15
#define DHT_TYPE DHT11

// MQ-135
#define MQ135_PIN 2

// MAX30100 I2C
#define I2C_SDA 8
#define I2C_SCL 9

// INMP441
#define I2S_PORT I2S_NUM_0
#define I2S_SCK 12
#define I2S_WS  10
#define I2S_SD  11


// =====================================================
// OBJECT
// =====================================================

DHT dht(DHT_PIN, DHT_TYPE);

PulseOximeter pox;


// =====================================================
// VARIABEL
// =====================================================

uint32_t tsLastReport = 0;


// =====================================================
// CALLBACK MAX30100
// =====================================================

void onBeatDetected()
{
  Serial.println("❤️ Detak jantung terdeteksi");
}


// =====================================================
// SETUP I2S INMP441
// =====================================================

void setupI2S()
{
  const i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(
      I2S_MODE_MASTER |
      I2S_MODE_RX
    ),

    .sample_rate = 16000,

    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,

    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,

    .communication_format = I2S_COMM_FORMAT_I2S,

    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,

    .dma_buf_count = 8,

    .dma_buf_len = 64,

    .use_apll = false,

    .tx_desc_auto_clear = false,

    .fixed_mclk = 0
  };

  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };

  i2s_driver_install(
    I2S_PORT,
    &i2s_config,
    0,
    NULL
  );

  i2s_set_pin(
    I2S_PORT,
    &pin_config
  );

  i2s_zero_dma_buffer(I2S_PORT);
}


// =====================================================
// BACA INMP441
// =====================================================

int32_t readINMP441()
{
  int32_t samples[64];

  size_t bytesRead = 0;

  i2s_read(
    I2S_PORT,
    samples,
    sizeof(samples),
    &bytesRead,
    portMAX_DELAY
  );

  if (bytesRead > 0)
  {
    long total = 0;

    int jumlahSample =
      bytesRead / sizeof(int32_t);

    for (int i = 0; i < jumlahSample; i++)
    {
      total += abs(samples[i] >> 14);
    }

    return total / jumlahSample;
  }

  return 0;
}


// =====================================================
// SETUP
// =====================================================

void setup()
{
  Serial.begin(115200);

  delay(1000);

  Serial.println();
  Serial.println("======================================");
  Serial.println("   SISTEM MONITORING SENSOR ESP32-S3");
  Serial.println("======================================");


  // ---------------------------------------------------
  // DHT11
  // ---------------------------------------------------

  dht.begin();

  Serial.println("DHT11 : OK");


  // ---------------------------------------------------
  // I2C MAX30100
  // ---------------------------------------------------

  Wire.begin(
    I2C_SDA,
    I2C_SCL
  );

  Serial.println("Mencoba mendeteksi MAX30100...");

  if (!pox.begin())
  {
    Serial.println("MAX30100 : GAGAL");
  }
  else
  {
    Serial.println("MAX30100 : OK");

    pox.setIRLedCurrent(
      MAX30100_LED_CURR_7_6MA
    );

    pox.setOnBeatDetectedCallback(
      onBeatDetected
    );
  }


  // ---------------------------------------------------
  // INMP441
  // ---------------------------------------------------

  setupI2S();

  Serial.println("INMP441 : OK");


  // ---------------------------------------------------
  // MQ135
  // ---------------------------------------------------

  pinMode(
    MQ135_PIN,
    INPUT
  );

  Serial.println("MQ-135 : OK");

  Serial.println();
  Serial.println("Semua sensor mulai membaca...");
  Serial.println();
}


// =====================================================
// LOOP
// =====================================================

void loop()
{
  // ===================================================
  // MAX30100
  // ===================================================

  pox.update();


  // ===================================================
  // BACA SENSOR SETIAP 1 DETIK
  // ===================================================

  if (millis() - tsLastReport > 1000)
  {
    tsLastReport = millis();


    // -------------------------------------------------
    // DHT11
    // -------------------------------------------------

    float humidity = dht.readHumidity();

    float temperature = dht.readTemperature();


    Serial.println("--------------------------------------");

    if (isnan(humidity) || isnan(temperature))
    {
      Serial.println("DHT11 : GAGAL MEMBACA");
    }
    else
    {
      Serial.print("Suhu      : ");
      Serial.print(temperature);
      Serial.println(" °C");

      Serial.print("Kelembapan: ");
      Serial.print(humidity);
      Serial.println(" %");
    }


    // -------------------------------------------------
    // MQ135
    // -------------------------------------------------

    int mq135Value =
      analogRead(MQ135_PIN);

    Serial.print("MQ-135 ADC: ");
    Serial.println(mq135Value);


    // Tegangan ADC
    float voltage =
      (mq135Value / 4095.0) * 3.3;

    Serial.print("MQ-135 Volt: ");
    Serial.print(voltage, 3);
    Serial.println(" V");


    // -------------------------------------------------
    // MAX30100
    // -------------------------------------------------

    float heartRate =
      pox.getHeartRate();

    float spo2 =
      pox.getSpO2();


    Serial.print("Heart Rate: ");
    Serial.print(heartRate);
    Serial.println(" BPM");

    Serial.print("SpO2      : ");
    Serial.print(spo2);
    Serial.println(" %");


    // -------------------------------------------------
    // INMP441
    // -------------------------------------------------

    int32_t audioValue =
      readINMP441();

    Serial.print("INMP441   : ");
    Serial.println(audioValue);


    Serial.println("--------------------------------------");
  }
}