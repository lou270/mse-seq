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

RocketState_t seqPreLaunch() {
    if(launchDetection() == true) {
        #if DEBUG == true
        Serial.println(F("Launch detected"));
        #endif

        // Launch apogee timer
        launchTime = rp2040.getCycleCount64()/(rp2040.f_cpu()/1000.0);
        // Change buzzer sound
        setBuzzer(BUZ_ON, BUZ_TIME_AFTER_LAUNCH, 40);

        #if DEBUG == true
        Serial.println(F("Waiting for apogee detection..."));
        #endif

        sendInfoPld(ASCEND);
        return ASCEND;
    } else {
        #if INPUT_CLOSE_MOT_ACTION == true
        bool fdc1 = digitalRead(FDC1_PIN); // Motor 1 ON/OFF
        bool fdc2 = digitalRead(FDC2_PIN); // Motor 2 ON/OFF
        bool fdc3 = digitalRead(FDC3_PIN); // OPEN/CLOSE
        bool fdc4 = digitalRead(FDC4_PIN); // Not used

        if (fdc1 == false) {
            if (fdc3 == true) {
                writeMotor(1, CLOSE_PARA_DOOR_VAL);
            } else {
                writeMotor(1, OPEN_PARA_DOOR_VAL);
            }
        } else if (fdc2 == false) {
            if (fdc3 == true) {
                writeMotor(2, CLOSE_PARA_DOOR_VAL);
            } else {
                writeMotor(2, OPEN_PARA_DOOR_VAL);
            }
        } else {
            writeAllMotor(0);
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

RocketState_t seqAscend() {
    getImuData(&imuData, false);
    computeAngle(&imuData, &angle);
    timeAfterLaunch = (rp2040.getCycleCount64()/(rp2040.f_cpu()/1000)) - (uint64_t)launchTime;
    if (timeAfterLaunch > START_WINDOW_TIME) {
        if(apogeeDetection(&angle) == true) {
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
    setBuzzer(BUZ_ON, BUZ_TIME_AFTER_APOGEE, 2000);
    writeAllMotor(OPEN_PARA_DOOR_VAL);
    add_alarm_in_ms(TIME_OPEN_PARA_DOOR, stopMotorCallback, NULL, true);

    #if DEBUG == true
    Serial.println(F("Done opening parachute door"));
    #endif

    sendInfoPld(DESCEND);
    return DESCEND;
}

RocketState_t seqDescend() {
    // return TOUCHDOWN;
    return DESCEND;
}

RocketState_t seqTouchdown() {
    // Nothing for now
    return TOUCHDOWN;
}

bool launchDetection() {
    return !digitalRead(ACC_CONT_PIN);
}

bool apogeeDetection(Angle_t *angle) {
    if (angle->x >= ANGLE_DET_APOGEE || angle->y >= ANGLE_DET_APOGEE) {
        return true;
    } else {
        return false;
    }
}

void sendInfoPld(RocketState_t st) {
    Serial1.write((uint8_t)st);
}