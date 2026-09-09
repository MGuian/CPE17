#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <DHT.h>

// DHT11 Pin Configuration
#define DHTPIN 4
#define DHTTYPE DHT11

// Initialize DHT Sensor and U8g2 Display (SH1106 for 1.3-inch OLED)
DHT dht(DHTPIN, DHTTYPE);
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  u8g2.begin();
}

void loop() {
  // Read temperature and humidity
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Clear display buffer
  u8g2.clearBuffer();

  // Handle sensor reading errors
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    u8g2.setFont(u8g2_font_6x10_tf);
    
    const char* errStr = "Sensor Error!";
    int errX = (128 - u8g2.getStrWidth(errStr)) / 2;
    u8g2.drawStr(errX, 35, errStr);
    
    u8g2.sendBuffer();
    delay(2000);
    return;
  }

  char tempNumStr[10];
  char humNumStr[10];
  dtostrf(temperature, 4, 1, tempNumStr); // 1 decimal place
  dtostrf(humidity, 4, 1, humNumStr);

  // --- TEMPERATURE SECTION ---
  u8g2.setFont(u8g2_font_6x10_tf);
  const char* tempLabel = "Temperature";
  int labelTempX = (128 - u8g2.getStrWidth(tempLabel)) / 2;
  u8g2.drawStr(labelTempX, 10, tempLabel);

  u8g2.setFont(u8g2_font_logisoso16_tf);
  // Total width = width of number + degree symbol offset (8px) + width of 'C'
  int totalTempWidth = u8g2.getStrWidth(tempNumStr) + 8 + u8g2.getStrWidth("C");
  int startTempX = (128 - totalTempWidth) / 2;

  u8g2.drawStr(startTempX, 28, tempNumStr);
  int degX = startTempX + u8g2.getStrWidth(tempNumStr) + 1;
  u8g2.drawGlyph(degX, 28, 176);           // Degree symbol
  u8g2.drawStr(degX + 7, 28, "C");

  // --- HUMIDITY SECTION ---
  u8g2.setFont(u8g2_font_6x10_tf);
  const char* humLabel = "Humidity";
  int labelHumX = (128 - u8g2.getStrWidth(humLabel)) / 2;
  u8g2.drawStr(labelHumX, 42, humLabel);

  u8g2.setFont(u8g2_font_logisoso16_tf);
  // Total width = width of number + width of '%'
  int totalHumWidth = u8g2.getStrWidth(humNumStr) + u8g2.getStrWidth("%");
  int startHumX = (128 - totalHumWidth) / 2;

  u8g2.drawStr(startHumX, 60, humNumStr);
  int percentX = startHumX + u8g2.getStrWidth(humNumStr);
  u8g2.drawStr(percentX, 60, "%");

  u8g2.sendBuffer();

  delay(2000); 
}