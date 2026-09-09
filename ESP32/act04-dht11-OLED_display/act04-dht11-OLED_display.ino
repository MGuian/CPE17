#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h>

// OLED Display Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// DHT11 Pin Configuration
#define DHTPIN 4
#define DHTTYPE DHT11

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  // Read sensor data
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Error check
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Sensor Error!");
    display.display();
    delay(2000);
    return;
  }
  display.clearDisplay();

  // TEMPERATURE SECTION 
  display.setTextSize(1);
  display.setCursor(10, 0);
  display.println("Temperature:");

  display.setTextSize(2);
  display.setCursor(10, 12);
  display.print(temperature, 2); 
  display.print(" ");
  display.cp437(true);           // Enable ASCII code page 437 for extended characters
  display.write(248);            // ASCII code 247 renders the degree symbol (°)
  display.println("C");

  // HUMIDITY SECTION
  display.setTextSize(1);
  display.setCursor(10, 34);
  display.println("Humidity:");

  display.setTextSize(2);
  display.setCursor(10, 46);
  display.print(humidity, 2); 
  display.println(" %");

  // Push buffer contents to the screen
  display.display();

  delay(2000); 
}