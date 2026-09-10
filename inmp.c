#include <Arduino.h>
#include "driver/i2s.h"

// ===============================
// PIN INMP441
// ===============================
#define I2S_PORT I2S_NUM_0

#define I2S_BCLK 4    // SCK
#define I2S_LRCL 5    // WS
#define I2S_DOUT 6    // SD

// ===============================
// KONFIGURASI
// ===============================
#define SAMPLE_RATE 16000
#define BUFFER_SIZE 512

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("==============================");
  Serial.println(" ESP32-S3 + INMP441");
  Serial.println("==============================");

  // Konfigurasi I2S
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(
      I2S_MODE_MASTER |
      I2S_MODE_RX
    ),

    .sample_rate = SAMPLE_RATE,

    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,

    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,

    .communication_format = I2S_COMM_FORMAT_STAND_I2S,

    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,

    .dma_buf_count = 8,

    .dma_buf_len = 64,

    .use_apll = false,

    .tx_desc_auto_clear = false,

    .fixed_mclk = 0
  };

  // Konfigurasi pin
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCLK,
    .ws_io_num = I2S_LRCL,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_DOUT
  };

  // Memulai I2S
  esp_err_t result = i2s_driver_install(
    I2S_PORT,
    &i2s_config,
    0,
    NULL
  );

  if (result != ESP_OK) {
    Serial.println("ERROR: I2S gagal dimulai!");
    return;
  }

  // Mengatur pin
  result = i2s_set_pin(
    I2S_PORT,
    &pin_config
  );

  if (result != ESP_OK) {
    Serial.println("ERROR: Pin I2S gagal!");
    return;
  }

  // Bersihkan buffer DMA
  i2s_zero_dma_buffer(I2S_PORT);

  Serial.println("INMP441 berhasil terhubung!");
  Serial.println("Silakan berbicara atau tepuk tangan...");
  Serial.println();
}

void loop() {

  int32_t buffer[BUFFER_SIZE];

  size_t bytesRead = 0;

  // Membaca data dari INMP441
  esp_err_t result = i2s_read(
    I2S_PORT,
    buffer,
    sizeof(buffer),
    &bytesRead,
    portMAX_DELAY
  );

  if (result != ESP_OK) {
    Serial.println("Gagal membaca INMP441!");
    return;
  }

  int samplesRead = bytesRead / sizeof(int32_t);

  int64_t total = 0;

  // Menghitung amplitudo suara
  for (int i = 0; i < samplesRead; i++) {

    // Data INMP441 adalah 24-bit
    int32_t sample = buffer[i] >> 8;

    total += abs(sample);
  }

  // Rata-rata amplitudo
  if (samplesRead > 0) {

    int32_t average =
      total / samplesRead;

    Serial.println(average);
  }

  delay(50);
}