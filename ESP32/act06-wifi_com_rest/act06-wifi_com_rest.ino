#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "WIFI_NAME";
const char* password = "WIFI_Password";

// Your FastAPI server IP & port
const char* serverURL = "http://192.168.1.27:8000/esp32/post-data"; // Change ip address depends in your ipv4

void setup() {
    Serial.begin(115200);
    delay(1000);

    // Connect to WiFi
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected!");
    Serial.print("ESP32 IP Address: ");
    Serial.println(WiFi.localIP());
}

void loop() {
    // You can call sendPostRequest() periodically if needed
    sendPostRequest();
    delay(10000);
}

void sendPostRequest() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        http.begin(serverURL);
        http.addHeader("Content-Type", "application/json");

        String jsonPayload = "{\"Temperature\":\"<reading from BMP520>\",\"Humidity\":\"<Reading from BMP520>\"}";

        int httpResponseCode = http.POST(jsonPayload);

        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println("Server Response:");
            Serial.println(response);
        } else {
            Serial.print("Error on sending POST: ");
            Serial.println(httpResponseCode);
        }

        http.end();
    } else {
        Serial.println("WiFi not connected");
    }
}