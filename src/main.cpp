/******************************
| Project       : MSE Avionics
| Board         : SEQ
| Description   : Sequencer main code
| Licence       : CC BY-NC-SA 
| https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
******************************/
#include <Arduino.h>
#include <pico/time.h>
#include "board.h"
#include "parameters.h"
#include "buzzer.h"
#include "motor.h"
#include "imu.h"
#include "sequencer.h"

/* TODO List
* Add PLD communication
* Add apogee detection
* Add 10dof access
* Add flash data writing
* Add usb data access
*/

struct repeating_timer dataWriterTimer;
uint64_t currentTime, previousTime = 0;

void setupBoard() {
    // SEQ PLD UART
    #if DEBUG == true
    Serial.begin(115200);
    Serial.println(F("Initialisation..."));
    #endif

    // SEQ<->PLD UART (Serial1 = UART0)
    Serial1.setRX(SEQ_PLD_UART_RX_PIN);
    Serial1.setTX(SEQ_PLD_UART_TX_PIN);
    Serial1.setFIFOSize(128);
    Serial1.begin(115200);

    // IHM UART
    Serial2.setRX(IHM_UART_RX_PIN);
    Serial2.setTX(IHM_UART_TX_PIN);
    Serial2.setFIFOSize(128);
    Serial2.begin(115200);

    // Setup acc contact
    pinMode(ACC_CONT_PIN, INPUT);

    // Setup motors
    pinMode(M1_A_PIN, OUTPUT);
    pinMode(M1_B_PIN, OUTPUT);
    pinMode(M2_A_PIN, OUTPUT);
    pinMode(M2_B_PIN, OUTPUT);

    // Setup buzzer
    pinMode(BUZZER_PIN, OUTPUT);
    analogWriteFreq(FREQ_PWM);
    analogWriteRange(MAX_VAL_PWM);

    // Setup LED Pico
    pinMode(PICO_LED_PIN, OUTPUT);
    digitalWrite(PICO_LED_PIN, LOW);

    // Setup button Pico
    pinMode(PICO_BUTTON_PIN, INPUT);

    // Setup end stop
    pinMode(FDC1_PIN, INPUT);
    pinMode(FDC2_PIN, INPUT);
    pinMode(FDC3_PIN, INPUT);
    pinMode(FDC4_PIN, INPUT);

    // Setup 10 DoF Pico
    setupIMU();
}

void setup() {
    setupBoard();

    #if DEBUG == true
    Serial.println(F("Board setup complete"));
    #endif

    setBuzzer(BUZ_ON, 500, 0.1);
    delay(2000);
    setBuzzer(false);

    setBuzzer(BUZ_ON, BUZ_TIME_BEFORE_LAUNCH, 100);
}

void loop() {
    uint64_t t0, t1 = 0;

    #if DEBUG == true
    Serial.println(F("Waiting for launch..."));
    #endif

    // =========== PRE-LAUNCH
    while(!launchDetection()) {
        #if INPUT_CLOSE_MOT_ACTION == true
        if(digitalRead(FDC4_PIN) == 1) {
            writeMotor(CLOSE_PARA_DOOR_VAL);
        } else {
            writeMotor(0);
        }
        #endif
        #if DEBUG == true
        // readIMUData();
        Angle_t * angle = computeAngle();
        currentTime = (rp2040.getCycleCount64()/(rp2040.f_cpu()/1000));
        if ((currentTime - previousTime) >= 250) {
            previousTime = currentTime;
            char txt[60] = "";
            sprintf(txt, "[IMU] angle X: %.2f | angle Y: %.2f | angle Z: %.2f", angle->x, angle->y, angle->z);
            Serial.println(txt);
        }
        #endif
        // seqPreLaunch();
        // t1 = rp2040.getCycleCount64();
        // Serial.println((t1-t0)*1000.0/rp2040.f_cpu());
        // t0 = rp2040.getCycleCount64();
    }

    // =========== ASCEND
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
        Angle_t * angle = computeAngle();

        currentTime = (rp2040.getCycleCount64()/(rp2040.f_cpu()/1000)) - launchTime;
        if (currentTime > START_WINDOW_TIME) {
            apogeeDetectedInWin = apogeeDetection(angle);
        }
        if (currentTime > END_WINDOW_TIME) {
            timerDone = true;
        }

    }

    // =========== DEPLOY PARACHUTE
    #if DEBUG == true
    Serial.println(F("Apogee detection done, opening parachute door..."));
    #endif

    // Change buzzer sound
    setBuzzer(BUZ_ON, BUZ_TIME_AFTER_APOGEE, 2000);
    openParachuteDoor();

    #if DEBUG == true
    Serial.println(F("Done opening parachute door"));
    #endif

    // =========== DESCEND
    // Nothing for now

    // =========== TOUCHDOWN
    // Nothing for now

    while(1)
    ;
}

