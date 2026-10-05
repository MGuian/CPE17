#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <U8g2_for_Adafruit_GFX.h>

// Set the legs of the TFT screen.
#define TFT_DC 15
#define TFT_CS 5
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;

void setup() {
  Serial.begin(115200);

  while (!Serial) {
      ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  tft.begin();
  tft.setCursor(26, 120);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(3);
  tft.println("Hello, UB!");
  tft.setCursor(20, 160);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.println("Test on SPI comm!");

  delay(2000);
}

void loop() {

}