#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <DHT.h>

// Safe GPIO pin for upload stability
#define DHTPIN 19
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

#define SERVICE_UUID "0000FFE0-0000-1000-8000-00805F9B34FB"
#define CHAR_UUID    "0000FFE1-0000-1000-8000-00805F9B34FB"

BLECharacteristic *pChar = NULL;
bool deviceConnected = false;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      pServer->startAdvertising();
    }
};

void setup() {
  Serial.begin(115200);
  dht.begin();

  BLEDevice::init("ESP32_DHT11");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pChar = pService->createCharacteristic(
    CHAR_UUID, 
    BLECharacteristic::PROPERTY_READ     | 
    BLECharacteristic::PROPERTY_WRITE    | 
    BLECharacteristic::PROPERTY_WRITE_NR | 
    BLECharacteristic::PROPERTY_NOTIFY
  );

  pChar->addDescriptor(new BLE2902());

  pService->start();
  BLEDevice::getAdvertising()->start();
  Serial.println("BLE DHT11 Monitoring Ready!");
}

void loop() {
  static unsigned long lastRead = 0;
  if (millis() - lastRead >= 2000) {
    lastRead = millis();

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    String payload = "D0: Temp: " + String(t, 1) + "C  Hum: " + String(h, 1) + "%\n";

    Serial.print("Sending: ");
    Serial.print(payload);

    if (deviceConnected && pChar != NULL) {
      pChar->setValue(payload.c_str());
      pChar->notify();
    }
  }

  vTaskDelay(10 / portTICK_PERIOD_MS);
}