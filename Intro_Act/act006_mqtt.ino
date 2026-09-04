#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "YourWiFi";
const char* password = "YourPassword";
const char* mqtt_server = "192.168.1.100"; 

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected");
}

void callback(char* topic, byte* message, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)message[i]);
    }
    Serial.println();
}

void reconnect() {
    while (!client.connected()) {
        if (client.connect("ESP32Client")) {
            client.subscribe("home/livingroom/light");
        } else {
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    }

void loop() {
    if (!client.connected()) reconnect();
    client.loop();

    // Publish temperature
    client.publish("home/livingroom/temperature", "28.5");
    delay(5000);
}