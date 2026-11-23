#include <SPI.h>
#include <mcp2515.h>
#include <DHT.h>

// ---------- DHT11 ----------
#define DHTPIN  21
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ---------- CAN ----------
#define CAN_CLOCK MCP_8MHZ        // change to MCP_16MHZ if your crystal says 16.000

struct can_frame txMsg;
struct can_frame rxMsg;
MCP2515 mcp2515(5, 1000000);      // CS = GPIO 5, SPI clock = 1 MHz

#define CAN_DATA_ID 0x036
#define CAN_ACK_ID  0x037
#define MAX_RETRIES 3

bool initCAN()
{
    MCP2515::ERROR r1 = mcp2515.reset();
    MCP2515::ERROR r2 = mcp2515.setBitrate(CAN_500KBPS, CAN_CLOCK);
    MCP2515::ERROR r3 = mcp2515.setNormalMode();

    Serial.print("reset=");       Serial.print(r1);
    Serial.print(" bitrate=");    Serial.print(r2);
    Serial.print(" normalMode="); Serial.println(r3);

    return (r1 == MCP2515::ERROR_OK && r2 == MCP2515::ERROR_OK && r3 == MCP2515::ERROR_OK);
}

void setup()
{
    Serial.begin(115200);
    delay(500);

    dht.begin();

    SPI.begin(18, 19, 23, -1);    // SCK, MISO, MOSI, no hardware CS (library controls CS)
    delay(300);                   // let the MCP2515 power up

    while (!initCAN()) {
        Serial.println("MCP2515 NOT responding. Check SPI wiring (CS/SI/SO/SCK), 5V and GND.");
        delay(2000);
    }
    Serial.println("MCP2515 OK, CAN transmitter ready");
}

void loop()
{
    float temperatureC = dht.readTemperature();

    if (isnan(temperatureC)) {
        Serial.println("DHT11 read failed. Check wiring and pull-up resistor.");
        delay(2000);
        return;
    }

    int16_t tempInt = (int16_t)(temperatureC * 100);

    txMsg.can_id  = CAN_DATA_ID;
    txMsg.can_dlc = 2;
    txMsg.data[0] = (tempInt >> 8) & 0xFF;
    txMsg.data[1] = tempInt & 0xFF;

    bool messageSent = false;
    int retries = 0;

    while (!messageSent && retries < MAX_RETRIES) {
        MCP2515::ERROR err = mcp2515.sendMessage(&txMsg);

        if (err == MCP2515::ERROR_OK) {
            Serial.print("Temperature sent: ");
            Serial.print(temperatureC);
            Serial.println(" C");

            bool ackReceived = false;
            unsigned long startTime = millis();

            while (millis() - startTime < 500) {
                if (mcp2515.readMessage(&rxMsg) == MCP2515::ERROR_OK) {
                    if (rxMsg.can_id == CAN_ACK_ID) {
                        ackReceived = true;
                        break;
                    }
                }
            }

            if (ackReceived) {
                Serial.println("ACK received");
                messageSent = true;
            } else {
                Serial.println("ACK not received, retrying...");
                retries++;
                initCAN();        // clears any frame stuck in the TX buffer
            }
        } else {
            Serial.print("Error sending message, code ");
            Serial.println(err);
            retries++;

            if (err == MCP2515::ERROR_ALLTXBUSY) {
                Serial.println("TX buffers stuck (no node acknowledging). Re-initializing MCP2515...");
                initCAN();
            }
        }
    }

    if (!messageSent) {
        Serial.println("Failed to send message after retries");
    }

    delay(2000);
}
