/*************
* Project : MSE Avionics
* Board : SEQ
* Description : Board test
**************/
#include <Arduino.h>
#include "board.h"

void setup() {
    // SEQ PLD UART
    Serial.begin(115200);
    // IHM UART
    Serial1.begin(115200);

    Serial.println(F("Initialisation..."));

    pinMode(M1_A_PIN, OUTPUT);
    pinMode(M1_B_PIN, OUTPUT);
    pinMode(M2_A_PIN, OUTPUT);
    pinMode(M2_B_PIN, OUTPUT);

    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    delay(5000);
    
    Serial.println(F("Starting tests..."));
}


void loop() {

    // Voltage
    Serial.println(F("== Voltage test =="));
    uint16_t vbatSeq = analogRead(VBAT_SEQ_PIN);
    Serial.print(F("VBAT SEQ = "));
    Serial.println(vbatSeq*3.3/1024.0*1.0/VD_DIV);

    uint16_t vbatMot = analogRead(VBAT_M_PIN);
    Serial.print(F("VBAT MOT = "));
    Serial.println(vbatMot*3.3/1024.0*1.0/VD_DIV);

    // Buzzer
    Serial.println(F("== Buzzer test =="));
    // tone(BUZZER_PIN, 1000, 50);
    delay(500);
    // tone(BUZZER_PIN, 1000, 50);

    // IHM UART
    Serial1.print(F("I"));
    if(Serial1.read() == (int)"I") {
        Serial.println(F("[IHM] UART OK"));
    } else {
        Serial.println(F("/!\\ IHM UART NOT OK"));
    }

    // Motor
    Serial.println(F("== MOTOR test =="));
    uint8_t cnt = 4;
    uint8_t val = 0;
    while(cnt--) {
        Serial.print(F("[MOTOR] value = "));
        Serial.println(val);
        digitalWrite(M1_A_PIN, val & 0x1);
        digitalWrite(M1_B_PIN, val & 0x2);
        digitalWrite(M2_A_PIN, val & 0x1);
        digitalWrite(M2_B_PIN, val & 0x2);
        val = (val + 1) & 0x3;
        delay(1000);
    }

    delay(2000);
}
