#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_SSD1306.h>

#define LED_SCREEN_WIDTH 128 // OLED display width, in pixels
#define LED_SCREEN_HEIGHT 64 // OLED display height, in pixels
#define LED_OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define LED_SCREEN_ADDRESS 0x3C ///< See datasheet for Address
#define LCD_SCREEN_WIDTH 16
#define LCD_SCREEN_HEIGHT 2
#define LCD_SCREEN_ADDRESS 0x27

Adafruit_SSD1306 display(LED_SCREEN_WIDTH, LED_SCREEN_HEIGHT, &Wire, LED_OLED_RESET);
LiquidCrystal_I2C LCD = LiquidCrystal_I2C(LCD_SCREEN_ADDRESS, LCD_SCREEN_WIDTH, LCD_SCREEN_HEIGHT);

void setup() {
    Serial.begin(115200);
    
    //LCD display
    LCD.clear();
    LCD.init();
    LCD.backlight();
    LCD.setCursor(0, 0);
    LCD.print("Hello from I2C LCD");
    
    //LED display
    if (!display.begin(SSD1306_SWITCHCAPVCC, LED_SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;); // Don't proceed, loop forever
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.println("Hello from I2C LED");
    display.display();
    delay(5000);
}

void loop() {

}