#ifndef BOARD
#define BOARD

#include <SPI.h>
#include <MFRC522.h>
#include "webserver.h"

#define RELAY D3
#define BUZZER D2
#define RESET D9
#define SLAVESELECT D14

#define CREDENTIAL "C2 D6 F7 2C"

MFRC522 rfid(SLAVESELECT, RESET);
bool isInitialized = false;

class Board{
    public:
    static void Setup()
    {
        pinMode(RELAY, OUTPUT);
        pinMode(BUZZER, OUTPUT);
        pinMode(LED_BUILTIN, OUTPUT);

        SPI.begin();
        rfid.PCD_Init();
        delay(10);

        WebServer::Setup();
    }

    class RFID{
        public:
        static void Open()
        {
            if(!isInitialized)
            {
                Serial.println("Initiate...");
                digitalWrite(LED_BUILTIN, LOW);
                digitalWrite(RELAY, LOW);
                digitalWrite(BUZZER, HIGH);
                delay(500);
                digitalWrite(LED_BUILTIN, HIGH);
                digitalWrite(RELAY, HIGH);
                digitalWrite(BUZZER, LOW);
                Serial.println("Initiate Complete.");
                isInitialized = true;
            }
        }

        static void Authenticate()
        {
            Serial.println("[ Waiting for an answer ... ]");
            delay(1000);

            if(!rfid.PICC_IsNewCardPresent())
                return;

            if(!rfid.PICC_ReadCardSerial())
                return;

            Serial.print("UID tag: ");
            String content = "";
            for (byte i = 0; i < rfid.uid.size; i++)
            {
                byte uid = rfid.uid.uidByte[i];
                String letter = uid < 0x10 ? " 0" : " ";

                Serial.print(letter);
                Serial.print(uid, HEX);
                content.concat(String(letter));
                content.concat(String(uid, HEX));
            }

            Serial.println();
            Serial.print("Authentication: ");
            content.toUpperCase();

            if(content.substring(1) == "C2 D6 F7 2C")
            {
                String sensorReadings = WebServer::httpGETRequest(serverName);
                Serial.println(sensorReadings);

                Serial.println("Access Granted");
                digitalWrite(RELAY, LOW);
                digitalWrite(LED_BUILTIN, LOW);
                delay(2500);
                digitalWrite(RELAY, HIGH);
                digitalWrite(LED_BUILTIN, HIGH);
            }
            else
            {
                Serial.println("Access Denied");
                digitalWrite(BUZZER, HIGH);
                delay(500);
                digitalWrite(BUZZER, LOW);
            }
        }
    };

};

#endif