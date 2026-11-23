#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

#define DHTPIN 19
#define DHTTYPE DHT11

const char* ssid = "WIFI_NAME";
const char* password = "WIFI_PASSWORD";

// Use your PC's IP, NOT localhost
const char* serverUrl = "http://192.168.1.27:8000/sensors/";  // Change ipv4 address

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT11!");
    delay(2000);
    return;
  }

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"sensor_id\":\"DHT11-001\","
                    "\"temperature\":" + String(temperature, 1) + ","
                    "\"humidity\":" + String(humidity, 1) + ","
                    "\"location\":\"Room A\"}";

    int httpCode = http.POST(payload);
    Serial.print("POST ");
    Serial.println(payload);
    Serial.print("Response code: ");
    Serial.println(httpCode);   // 201 = success

    http.end();
  } else {
    Serial.println("WiFi disconnected, reconnecting...");
    WiFi.reconnect();
  }

  delay(10000);  // send every 10 seconds
}