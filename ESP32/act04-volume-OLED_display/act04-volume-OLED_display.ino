#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

#define POT_PIN 34    // ESP32 Analog input pin
#define BUZZER_PIN 18 // ESP32 PWM pin for passive buzzer

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  delay(1000); 
  Wire.begin(21, 22);

  pinMode(BUZZER_PIN, OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.ssd1306_command(SSD1306_DISPLAYON);
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  // Read and map potentiometer
  int rawAnalog = analogRead(POT_PIN);
  int volume = map(rawAnalog, 0, 4095, 0, 100);
  volume = constrain(volume, 0, 100);

  if (volume > 0) {
    int pitch = map(volume, 1, 100, 2000, 3800);
    tone(BUZZER_PIN, pitch);
  } else {
    noTone(BUZZER_PIN);
  }

  display.clearDisplay();

  // HEADER SECTION
  display.setTextSize(1);
  display.setCursor(20, 0);
  display.println("VOLUME CONTROL");
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

  // NUMERIC PERCENTAGE
  display.setTextSize(3);
  display.setCursor(25, 18);
  display.print(volume);

  display.setTextSize(2);
  display.print("%");

  // BAR
  int barWidth = map(volume, 0, 100, 0, 104);
  display.drawRect(10, 46, 108, 14, SSD1306_WHITE); 
  display.fillRect(12, 48, barWidth, 10, SSD1306_WHITE); 

  display.display();

  delay(30);
}