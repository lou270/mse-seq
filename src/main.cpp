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

// Utils
struct repeating_timer dataWriterTimer;
uint64_t currentTime, previousTime = 0;

// Rocket state machine
RocketState_t rocketState = PRE_FLIGHT;

void setupBoard() {
    // SEQ PLD UART
    #if DEBUG == true
    Serial.begin(115200);
    Serial.println(F("[SETUP] Initialisation..."));
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

    // Init I2C1 (dedicated to IMU)
    Wire1.setSDA(I2C1_SDA);
    Wire1.setSCL(I2C1_SCL);
    Wire1.setClock(400000);
    Wire1.begin();

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
    pinMode(PICO_BUTTON_PIN, INPUT_PULLUP);

    // Setup end stop
    pinMode(FDC1_PIN, INPUT);
    pinMode(FDC2_PIN, INPUT);
    pinMode(FDC3_PIN, INPUT);
    pinMode(FDC4_PIN, INPUT);

    // Setup cameras
    pinMode(CAM_N1_PIN, OUTPUT);
    pinMode(CAM_N2_PIN, OUTPUT);
    setCameraTrigger(1);
}

void setup() {
    delay(1000);

    setupBoard();

    setupIMU();

    setBuzzer(BUZ_ON, 100, 30);
    delay(300);
    setBuzzer(false);

    setBuzzer(BUZ_ON, BUZ_TIME_PRE_LAUNCH, 1000);

    #if DEBUG == true
    Serial.println(F("[SETUP] Board setup complete"));
    #endif
}

void loop() {
    // Rocket state machine
    // =========== PRE-LAUNCH
    if (rocketState == PRE_FLIGHT) {
        processSensors();
        rocketState = seqPreLaunch();
    } 
    // =========== PYRO READY
    else if (rocketState == PYRO_RDY) {
        processSensors();
        rocketState = seqPyroRdy();
    } 
    // =========== ASCEND
    else if (rocketState == ASCEND) {
        processSensors();
        rocketState = seqAscend();
    } 
    // =========== DEPLOY
    else if (rocketState == DEPLOY_TIMER || rocketState == DEPLOY_ALGO) {
        processSensors();
        rocketState = seqDeploy();
    }
    // =========== DESCEND
    else if (rocketState == DESCEND) {
        processSensors();
        rocketState = seqDescend();
    }
    // =========== TOUCHDOWN
    else {
        processSensors();
        rocketState = seqTouchdown();
    }

    // Save periodic data
    // writeDataToBufferFile();

    #if DEBUG == true
    delay(500);
    #endif
}

