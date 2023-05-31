/******************************
| Project       : MSE Avionics
| Board         : SEQ
| Description   : Sequencer main code
| Licence       : CC BY-NC-SA 
| https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
******************************/
#include <Arduino.h>
#include "board.h"
#include "parameters.h"
#include <pico/time.h>

/* TODO List
* Add PLD communication
* Add apogee detection
* Add flash data writing
* Add usb data access
*/

bool apogeeDetected = false;
float_t buzzerPulseTime = 0.1;
volatile bool buzzerEnable = false;
struct repeating_timer buzzerTimer;
struct repeating_timer dataWriterTimer;

void setupBoard() {
    // SEQ PLD UART
    Serial.begin(115200);
    #if DEBUG == true
    Serial.println(F("Initialisation..."));
    #endif
    // IHM UART
    Serial1.begin(115200);

    // Setup motors
    pinMode(M1_A_PIN, OUTPUT);
    pinMode(M1_B_PIN, OUTPUT);
    pinMode(M2_A_PIN, OUTPUT);
    pinMode(M2_B_PIN, OUTPUT);

    // Setup buzzer
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    // Setup LED
    pinMode(PICO_LED_PIN, OUTPUT);
    digitalWrite(PICO_LED_PIN, LOW);
}

bool buzzerCallback(struct repeating_timer *t) {
    if(buzzerEnable) {
        tone(BUZZER_PIN, BUZ_FREQ, (unsigned long)t->user_data);
    }
    boolean wut = digitalRead(PICO_LED_PIN);
    digitalWrite(PICO_LED_PIN, !wut);
    Serial.println("TIMER FIRED");
    return true;
}

void setBuzzer(bool enable, uint16_t beatPeriod = 0, uint16_t beatDuration = 0) {
    bool a = cancel_repeating_timer(&buzzerTimer);
    if(a) {
        Serial.println("DONE REMOVING");
    } else {
        Serial.println("NO TIMER TO REMOVE");
    }
    buzzerEnable = enable;
    if (!enable) {
        digitalWrite(BUZZER_PIN, 0);
    }
    if (beatPeriod > 0) {
        a = add_repeating_timer_ms(beatPeriod, buzzerCallback, &beatDuration, &buzzerTimer);
        if(a) {
            Serial.println("ADD TIMER OK");
        } else {
            Serial.println("ADD TIMER NOT OK");
        }
    }


}

bool launchDetection() {
    return false;
}

bool apogeeDetection() {
    return false;
}

void closeParachuteDoor() {
    digitalWrite(M1_A_PIN, CLOSE_PARA_DOOR_VAL & 0x1);
    digitalWrite(M1_B_PIN, CLOSE_PARA_DOOR_VAL & 0x2);
    digitalWrite(M2_A_PIN, CLOSE_PARA_DOOR_VAL & 0x1);
    digitalWrite(M2_B_PIN, CLOSE_PARA_DOOR_VAL & 0x2);
    delay(TIME_CLOSE_PARA_DOOR);
    digitalWrite(M1_A_PIN, 0x0);
    digitalWrite(M1_B_PIN, 0x0);
    digitalWrite(M2_A_PIN, 0x0);
    digitalWrite(M2_B_PIN, 0x0);
}

void openParachuteDoor() {
    digitalWrite(M1_A_PIN, OPEN_PARA_DOOR_VAL & 0x1);
    digitalWrite(M1_B_PIN, OPEN_PARA_DOOR_VAL & 0x2);
    digitalWrite(M2_A_PIN, OPEN_PARA_DOOR_VAL & 0x1);
    digitalWrite(M2_B_PIN, OPEN_PARA_DOOR_VAL & 0x2);
    delay(TIME_OPEN_PARA_DOOR);
    digitalWrite(M1_A_PIN, 0x0);
    digitalWrite(M1_B_PIN, 0x0);
    digitalWrite(M2_A_PIN, 0x0);
    digitalWrite(M2_B_PIN, 0x0);
}

bool statusCallback(struct repeating_timer *t) {
  boolean wut = digitalRead(PICO_LED_PIN);
  digitalWrite(PICO_LED_PIN, !wut);
  return true;
}

bool dataWriterCallback(struct repeating_timer *t) {
//   boolean wut = digitalRead(PICO_LED_PIN);
//   digitalWrite(PICO_LED_PIN,!wut);
  return true;
}

void setup() {
    delay(5000);
    setupBoard();

    #if DEBUG == true
    Serial.println(F("Board setup complete"));
    #endif

    setBuzzer(BUZ_ON, 500, 0.1);
    delay(600);
    setBuzzer(false);

    #if DEBUG == true
    Serial.println(F("Closing parachute door..."));
    #endif

    closeParachuteDoor();

    #if DEBUG == true
    Serial.println(F("Done closing parachute door"));
    #endif
    
    setBuzzer(BUZ_ON, 500, 0.1);
    delay(1600);
    
    setBuzzer(BUZ_ON, BUZ_TIME_BEFORE_LAUNCH, 100);
}

void loop() {

    #if DEBUG == true
    Serial.println(F("Waiting for launch..."));
    #endif

    // Wait until launch is detected
    while(!launchDetection())
        ;

    #if DEBUG == true
    Serial.println(F("Launch detected"));
    #endif

    // Launch apogee timer
    uint64_t launchTime = rp2040.getCycleCount64()/(rp2040.f_cpu()/1000);
    // Change buzzer sound
    setBuzzer(BUZ_ON, BUZ_TIME_AFTER_LAUNCH, 100);

    #if DEBUG == true
    Serial.println(F("Waiting for apogee detection..."));
    #endif

    bool apogeeDetectedInWin = false;
    bool timerDone = false;
    while(!apogeeDetectedInWin && !timerDone) {

        uint64_t curTime = (rp2040.getCycleCount64()/(rp2040.f_cpu()/1000)) - launchTime;

        apogeeDetected = apogeeDetection();
        if (curTime > START_WINDOW_TIME) {
            apogeeDetectedInWin = apogeeDetected;
        }
        
        if (curTime > END_WINDOW_TIME) {
            timerDone = true;
        }
    }

    #if DEBUG == true
    Serial.println(F("Apogee detection done, opening parachute door..."));
    #endif

    // Change buzzer sound
    setBuzzer(BUZ_ON, BUZ_TIME_AFTER_APOGEE, 2000);
    openParachuteDoor();

    #if DEBUG == true
    Serial.println(F("Done opening parachute door"));
    #endif

    while(1)
        ;
}
