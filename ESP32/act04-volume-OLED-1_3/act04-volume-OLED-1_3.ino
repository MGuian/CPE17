#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

#define POT_PIN 34    // ESP32 Analog input pin
#define BUZZER_PIN 18 // ESP32 PWM pin for passive buzzer

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  u8g2.begin();
}

void loop() {
  int rawAnalog = analogRead(POT_PIN);
  int volume = map(rawAnalog, 0, 4095, 0, 100);
  volume = constrain(volume, 0, 100);

  // High-resonance pitch feedback
  if (volume > 0) {
    int pitch = map(volume, 1, 100, 2000, 3800);
    tone(BUZZER_PIN, pitch);
  } else {
    noTone(BUZZER_PIN);
  }

  u8g2.clearBuffer();

  // HEADER SECTION
  u8g2.setFont(u8g2_font_6x10_tf);
  const char* headerText = "VOLUME CONTROL";
  int headerX = (128 - u8g2.getStrWidth(headerText)) / 2;
  u8g2.drawStr(headerX, 10, headerText);
  
  u8g2.drawLine(4, 12, 123, 12); 

  // PERCENTAGE
  char volNumStr[10];
  snprintf(volNumStr, sizeof(volNumStr), "%d", volume);
  u8g2.setFont(u8g2_font_profont29_tf);
  int numWidth = u8g2.getStrWidth(volNumStr);

  u8g2.setFont(u8g2_font_logisoso20_tr);
  int percentWidth = u8g2.getStrWidth("%");

  int totalTextWidth = numWidth + 4 + percentWidth;
  int startX = (128 - totalTextWidth) / 2;

  // Draw Number
  u8g2.setFont(u8g2_font_profont29_tf);
  u8g2.drawStr(startX, 38, volNumStr);

  // Draw Large % Symbol
  u8g2.setFont(u8g2_font_logisoso20_tr);
  u8g2.drawStr(startX + numWidth + 4, 38, "%");

  // PROGRESS BAR
  int frameWidth = 108;
  int frameX = (128 - frameWidth) / 2;
  int barWidth = map(volume, 0, 100, 0, frameWidth - 4);

  u8g2.drawFrame(frameX, 46, frameWidth, 14);
  u8g2.drawBox(frameX + 2, 48, barWidth, 10);

  u8g2.sendBuffer();

  delay(30);
}