/******************************
| Project       : MSE Avionics
| Board         : SEQ
| Description   : Sequencer code
| Licence       : CC BY-NC-SA 
| https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
******************************/
#include "sequencer.h"

uint64_t currTime, prevTime, timeAfterLaunch = 0;
double launchTime, apogeeTime = 0;

// Apogee detection
bool apogeeDetectedInWin = false;
bool timerDone = false;
Imu_t imuData;
Angle_t angle;
DerivPressure_t derivP = {
    .prevPressure = 0.0,
    .prevTime = 0.0,
    .dt = 0.0,
    .derivate = 0.0
};
uint8_t currentBaroMean = 0;
LPS22HB lps22hb(Wire1); // Pressure sensor

RocketState_t seqPreLaunch() {

    // Pull-up for every FDC
    bool fdc1 = digitalRead(FDC1_PIN); // Motor 1 ON/OFF
    bool fdc2 = digitalRead(FDC2_PIN); // Motor 2 ON/OFF
    bool fdc3 = digitalRead(FDC3_PIN); // Open/Close motor
    bool fdc4 = digitalRead(FDC4_PIN); // Flamme PYRO

    if(fdc4 == true) {
        setBuzzer(BUZ_ON, 200, 100);
        delay(1000);
        setBuzzer(BUZ_ON, BUZ_TIME_PYRO_RDY, 200);
        setCameraTrigger(0);
        sendInfoPld(PYRO_RDY);
        return PYRO_RDY;
    } else {
        // if (digitalRead(PICO_BUTTON_PIN) == false) {
        //     Serial.println("BUTTON ON");
        //     setCameraTrigger(1);
        // } else {
        //     Serial.println("BUTTON OFF");
        //     setCameraTrigger(0);
        // }

        #if INPUT_CLOSE_MOT_ACTION == true
        if (fdc1 == false) {
            if (fdc3 == true) {
                writeMotor(1, CLOSE_PARA_DOOR_VAL);
            } else {
                writeMotor(1, OPEN_PARA_DOOR_VAL);
            }
        } else {
            writeMotor(1, 0);
        }
        
        if (fdc2 == false) {
            if (fdc3 == true) {
                writeMotor(2, CLOSE_PARA_DOOR_VAL);
            } else {
                writeMotor(2, OPEN_PARA_DOOR_VAL);
            }
        } else {
            writeMotor(2, 0);
        }
        #endif

        #if DEBUG == true
        getImuData(&imuData, false);
        computeAngle(&imuData, &angle);
        currTime = (rp2040.getCycleCount64()/(rp2040.f_cpu()/1000));
        if ((currTime - prevTime) >= 250) {
            prevTime = currTime;
            char txt[60] = "";
            sprintf(txt, "[IMU] angle X: %.2f | angle Y: %.2f | angle Z: %.2f", angle.x, angle.y, angle.z);
            Serial.println(txt);
        }
        #endif
    }

    return PRE_FLIGHT;
}

RocketState_t seqPyroRdy() {
    if(launchDetection() == true) {
        #if DEBUG == true
        Serial.println(F("Launch detected"));
        #endif

        // Launch apogee timer
        launchTime = rp2040.getCycleCount64()/(rp2040.f_cpu()/1000.0);
        // Change buzzer sound
        setBuzzer(BUZ_ON, BUZ_TIME_ASCEND, 40);

        #if DEBUG == true
        Serial.println(F("Waiting for apogee detection..."));
        #endif

        sendInfoPld(ASCEND);
        return ASCEND;
    }
    return PYRO_RDY;
}

RocketState_t seqAscend() {
    getImuData(&imuData, false);
    computeAngle(&imuData, &angle);
    timeAfterLaunch = (rp2040.getCycleCount64()/(rp2040.f_cpu()/1000)) - (uint64_t)launchTime;
    if (timeAfterLaunch > START_WINDOW_TIME) {
        if(apogeeDetection(&derivP) == true) { // Not working
            #if DEBUG == true
            Serial.println(F("Apogee detection done (ALGO), opening parachute door..."));
            #endif

            apogeeTime = (rp2040.getCycleCount64()*1.0/(rp2040.f_cpu()/1000.0));
            sendInfoPld(DEPLOY_ALGO);
            return DEPLOY_ALGO;
        }
    }
    if (timeAfterLaunch > END_WINDOW_TIME) {
        #if DEBUG == true
        Serial.println(F("Apogee detection done (TIMER), opening parachute door..."));
        #endif

        apogeeTime = (rp2040.getCycleCount64()*1.0/(rp2040.f_cpu()/1000.0));
        sendInfoPld(DEPLOY_TIMER);
        return DEPLOY_TIMER;
    }
    return ASCEND;
}

RocketState_t seqDeploy() {
    // Change buzzer sound
    setBuzzer(BUZ_ON, BUZ_TIME_DESCEND, 250);
    writeAllMotor(OPEN_PARA_DOOR_VAL);
    add_alarm_in_ms(TIME_OPEN_PARA_DOOR, stopMotorCallback, NULL, true);

    #if DEBUG == true
    Serial.println(F("Done opening parachute door"));
    #endif

    sendInfoPld(DESCEND);
    return DESCEND;
}

RocketState_t seqDescend() {
    timeAfterLaunch = (rp2040.getCycleCount64()/(rp2040.f_cpu()/1000)) - (uint64_t)launchTime;

    if (timeAfterLaunch > TIME_AFTER_APOGEE_TOUCHDOWN) {
        setBuzzer(BUZ_ON, BUZ_TIME_RECUP, 5000);
        setCameraTrigger(1);
        delay(1000);
        setCameraTrigger(0);
        return TOUCHDOWN;
    }
    return DESCEND;
}

RocketState_t seqTouchdown() {
    // Nothing for now
    return TOUCHDOWN;
}

void processSensors() {
    int32_t currPressure = lps22hb.readRawPressure();
    float_t meanPressure = 0;
    uint64_t currTime = rp2040.getCycleCount64();

    if (currentBaroMean >= BARO_MEAN_NB-1) {
        currentBaroMean = 0;
        meanPressure /= BARO_MEAN_NB;
        getPressureDerivate(&derivP, meanPressure, currTime);
        meanPressure = 0;
    } else {
        currentBaroMean++;
        meanPressure += currPressure;
    }
}

bool launchDetection() {
    return !digitalRead(ACC_CONT_PIN);
}

bool apogeeDetection(DerivPressure_t* derivP) {
    // Angle_t *angle
    // if (angle->x >= ANGLE_DET_APOGEE || angle->y >= ANGLE_DET_APOGEE) {
    //     return true;
    // } else {
    //     return false;
    // }

    // if (derivP->derivate > THRESHOLD_DERIV_BARO_APOGEE) {
    //     return true;
    // } else {
    //     return false;
    // }

    return false;
}

void sendInfoPld(RocketState_t st) {
    Serial1.write((uint8_t)st);

    #if DEBUG == true
    Serial.printf("[STATE] %d\n", (uint8_t)st);
    #endif
}

void getPressureDerivate(DerivPressure_t* derivP, float_t currPressure, uint64_t currTime) {
    derivP->dt = currTime - derivP->prevTime;
    derivP->derivate = (currPressure - derivP->prevPressure)/(derivP->dt);
    derivP->prevTime = currTime;
    derivP->prevPressure = currPressure;
}

// void setOnCameraTrigger() {
//     digitalWrite(CAM_N1_PIN, HIGH);
//     digitalWrite(CAM_N2_PIN, HIGH);
//     // add_alarm_in_ms(2000, setOffCameraTriggerCallback, NULL, true);
// }

void setCameraTrigger(uint8_t pinState) {
    digitalWrite(CAM_N1_PIN, pinState);
    digitalWrite(CAM_N2_PIN, pinState);
}

int64_t setOffCameraTriggerCallback(alarm_id_t id, void *user_data) {
    digitalWrite(CAM_N1_PIN, LOW);
    digitalWrite(CAM_N2_PIN, LOW);
    return 0;
}
