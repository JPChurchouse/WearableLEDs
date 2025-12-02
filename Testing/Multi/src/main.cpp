#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "driver/i2s.h"

#define OLED_WIDTH  128
#define OLED_HEIGHT  64
#define OLED_PIN_SDA 21
#define OLED_PIN_SCL 22
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);

// Encoder pins
#define ENC_PIN_CLK 32
#define ENC_PIN_DT  33
#define ENC_PIN_SW  25

// I2S pins (these matched the diagnostic that produced valid samples)
#define MIC_PIN_SCK 26
#define MIC_PIN_WS  27
#define MIC_PIN_SD  14

// LED pins (reserved, not driven)
#define LEDS_PIN_A  4
#define LEDS_PIN_B 16
#define LEDS_PIN_C 17
#define LEDS_PIN_D  5

volatile int encoderValue = 0;
int lastCLK = 0;

// ----- I2S setup (16-bit samples) -----
void setupI2SMic() {
  i2s_config_t cfg = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 16000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S, // standard I2S; your mic duplicates 16-bit into 32-bit words so 16-bit works
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 128,
    .use_apll = false
  };

  i2s_pin_config_t pins = {
    .bck_io_num = MIC_PIN_SCK,
    .ws_io_num = MIC_PIN_WS,
    .data_out_num = -1,
    .data_in_num = MIC_PIN_SD
  };

  i2s_driver_install(I2S_NUM_0, &cfg, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pins);
}

// Read mic samples, return pseudo-RMS and optionally a sample peek for serial
struct MicReading {
  int rms;       // pseudo-RMS scaled for display
  int16_t peek;  // a recent raw sample (signed 16-bit)
};

// Read N 16-bit samples and compute mean-abs as RMS proxy
MicReading readMic() {
  const int samples = 256;
  // i2s_read returns bytes. For 16-bit mode each sample is 2 bytes.
  // We read into a uint8_t buffer then reinterpret as int16_t.
  static int16_t buffer[samples];
  size_t bytesRead = 0;

  esp_err_t res = i2s_read(I2S_NUM_0, buffer, sizeof(buffer), &bytesRead, 200 / portTICK_PERIOD_MS);
  MicReading r = {0, 0};
  if (res != ESP_OK || bytesRead == 0) {
    // no data, return zeros
    return r;
  }

  int count = bytesRead / 2; // samples read
  // compute mean absolute value (pseudo-RMS)
  uint64_t sumAbs = 0;
  for (int i = 0; i < count; ++i) {
    int16_t s = buffer[i];
    sumAbs += (uint16_t)(s < 0 ? -s : s);
  }
  if (count > 0) r.rms = (int)(sumAbs / count);
  r.peek = buffer[0];
  return r;
}

void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println("Test app starting...");

  // Encoder
  pinMode(ENC_PIN_CLK, INPUT_PULLUP);
  pinMode(ENC_PIN_DT, INPUT_PULLUP);
  pinMode(ENC_PIN_SW, INPUT_PULLUP);
  lastCLK = digitalRead(ENC_PIN_CLK);

  // OLED
  Wire.begin(OLED_PIN_SDA, OLED_PIN_SCL); // SDA, SCL
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 init failed");
    for (;;) delay(10);
  }
  display.clearDisplay();
  display.display();

  // I2S mic
  setupI2SMic();

  // Reserve LED pins so nothing else grabs them (not used in test)
  pinMode(LEDS_PIN_A, OUTPUT); digitalWrite(LEDS_PIN_A, LOW);
  pinMode(LEDS_PIN_B, OUTPUT); digitalWrite(LEDS_PIN_B, LOW);
  pinMode(LEDS_PIN_C, OUTPUT); digitalWrite(LEDS_PIN_C, LOW);
  pinMode(LEDS_PIN_D, OUTPUT); digitalWrite(LEDS_PIN_D, LOW);
}

void loop() {
  // -- encoder handling (simple)
  int currentCLK = digitalRead(ENC_PIN_CLK);
  if (currentCLK != lastCLK && currentCLK == LOW) {
    if (digitalRead(ENC_PIN_DT) == HIGH) encoderValue++;
    else encoderValue--;
  }
  lastCLK = currentCLK;
  bool buttonPressed = (digitalRead(ENC_PIN_SW) == LOW);

  // -- mic read
  MicReading mr = readMic();

  // Print a small serial debug every loop iteration (reduce if too chatty)
  Serial.print("mic-rms: ");
  Serial.print(mr.rms);
  Serial.print(" peek: ");
  Serial.println(mr.peek);

  // -- OLED update
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.print("ENC: ");
  display.print(encoderValue);

  display.setCursor(0, 12);
  display.print("BTN: ");
  display.print(buttonPressed ? "DOWN" : "UP");

  display.setCursor(0, 24);
  display.print("Mic RMS: ");
  display.println(mr.rms);

  // Show peek sample in hex and decimal
  display.setCursor(0, 36);
  display.print("Peek: 0x");
  uint16_t u = (uint16_t)mr.peek;
  if (u < 0x1000) display.print("0");
  display.print(u, HEX);
  display.print(" ");
  display.print(mr.peek);

  // Draw bar graph scaled from 0..max (tweak maxVal if necessary)
  const int maxVal = 15000; // adjust if your readings are higher/lower
  int bar = map(constrain(mr.rms, 0, maxVal), 0, maxVal, 0, 120);
  display.fillRect(0, 50, bar, 10, SSD1306_WHITE);

  display.display();

  delay(50);
}
