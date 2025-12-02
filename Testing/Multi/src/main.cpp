/* ESP32 test: OLED + rotary encoder + I2S mic (16-bit) + reserved LED pins
   Working pins (as used in diagnostic that returned valid data):
     OLED SDA=21, SCL=22
     Encoder A=32, B=33, BTN=25
     I2S SCK(BCLK)=26, WS(LRCLK)=27, SD(DATA)=14
     Tie L/R pin on mic to GND (left) or 3.3V (right) — keep it tied.
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "driver/i2s.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Encoder pins
const int pinCLK = 32;
const int pinDT  = 33;
const int pinSW  = 25;

// I2S pins (these matched the diagnostic that produced valid samples)
#define I2S_BCLK 26
#define I2S_WS   27
#define I2S_SD   14

// LED pins (reserved, not driven)
#define LED1_PIN 4
#define LED2_PIN 16
#define LED3_PIN 17
#define LED4_PIN 5

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
    .bck_io_num = I2S_BCLK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
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
  pinMode(pinCLK, INPUT_PULLUP);
  pinMode(pinDT, INPUT_PULLUP);
  pinMode(pinSW, INPUT_PULLUP);
  lastCLK = digitalRead(pinCLK);

  // OLED
  Wire.begin(21, 22); // SDA, SCL
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 init failed");
    for (;;) delay(10);
  }
  display.clearDisplay();
  display.display();

  // I2S mic
  setupI2SMic();

  // Reserve LED pins so nothing else grabs them (not used in test)
  pinMode(LED1_PIN, OUTPUT); digitalWrite(LED1_PIN, LOW);
  pinMode(LED2_PIN, OUTPUT); digitalWrite(LED2_PIN, LOW);
  pinMode(LED3_PIN, OUTPUT); digitalWrite(LED3_PIN, LOW);
  pinMode(LED4_PIN, OUTPUT); digitalWrite(LED4_PIN, LOW);
}

void loop() {
  // -- encoder handling (simple)
  int currentCLK = digitalRead(pinCLK);
  if (currentCLK != lastCLK && currentCLK == LOW) {
    if (digitalRead(pinDT) == HIGH) encoderValue++;
    else encoderValue--;
  }
  lastCLK = currentCLK;
  bool buttonPressed = (digitalRead(pinSW) == LOW);

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
