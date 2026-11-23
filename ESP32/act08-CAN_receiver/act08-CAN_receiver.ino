#include <SPI.h>
#include <Wire.h>
#include <mcp2515.h>
#include <U8g2lib.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// ---------- CAN ----------
#define CAN_CLOCK MCP_8MHZ        // change to MCP_16MHZ if your crystal says 16.000

struct can_frame rxMsg;
struct can_frame ackMsg;
MCP2515 mcp2515(5, 1000000);      // CS = GPIO 5, SPI clock = 1 MHz

#define CAN_DATA_ID 0x036
#define CAN_ACK_ID  0x037

unsigned long lastRxTime = 0;
bool timeoutShown = false;

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

// Draw a string horizontally centered at the given baseline y
void drawCentered(const char *text, int y)
{
    int w = u8g2.getStrWidth(text);
    u8g2.drawStr((128 - w) / 2, y, text);
}

void showMessage(const char *line1, const char *line2)
{
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x12_tf);
    drawCentered(line1, 28);
    drawCentered(line2, 46);
    u8g2.sendBuffer();
}

void showTemperature(float tempC)
{
    char buf[16];
    char line[20];
    dtostrf(tempC, 4, 1, buf);              // e.g. "30.7"
    snprintf(line, sizeof(line), "%s C", buf);

    u8g2.clearBuffer();

    u8g2.setFont(u8g2_font_6x12_tf);
    drawCentered("Temperature:", 20);

    u8g2.setFont(u8g2_font_logisoso24_tr);
    drawCentered(line, 54);

    u8g2.sendBuffer();
}

void setup()
{
    Serial.begin(115200);
    delay(500);

    Wire.begin(21, 22);                 // SDA, SCL
    u8g2.begin();
    showMessage("CAN Receiver", "Starting...");

    SPI.begin(18, 19, 23, -1);          // SCK, MISO, MOSI, no hardware CS
    delay(300);                         // let the MCP2515 power up

    while (!initCAN()) {
        Serial.println("MCP2515 NOT responding. Check SPI wiring (CS/SI/SO/SCK), 5V and GND.");
        showMessage("MCP2515 ERROR", "Check wiring");
        delay(2000);
    }

    Serial.println("MCP2515 OK, CAN receiver ready");
    showMessage("CAN Receiver", "Waiting for data");
    lastRxTime = millis();
}

void loop()
{
    if (mcp2515.readMessage(&rxMsg) == MCP2515::ERROR_OK) {
        if (rxMsg.can_id == CAN_DATA_ID && rxMsg.can_dlc >= 2) {

            int16_t tempInt = (int16_t)((rxMsg.data[0] << 8) | rxMsg.data[1]);
            float temperatureC = tempInt / 100.0;

            Serial.print("Temperature received: ");
            Serial.print(temperatureC);
            Serial.println(" C");

            showTemperature(temperatureC);
            lastRxTime = millis();
            timeoutShown = false;

            // Send acknowledgment (use a separate frame, don't reuse rxMsg)
            ackMsg.can_id  = CAN_ACK_ID;
            ackMsg.can_dlc = 0;
            MCP2515::ERROR err = mcp2515.sendMessage(&ackMsg);

            if (err == MCP2515::ERROR_OK) {
                Serial.println("ACK sent");
            } else {
                Serial.print("ACK send error, code ");
                Serial.println(err);
            }
        }
    }

    // If nothing arrives for 10 s, tell the user
    if (!timeoutShown && millis() - lastRxTime > 10000) {
        showMessage("No data", "Check CAN H / L");
        timeoutShown = true;
    }
}
