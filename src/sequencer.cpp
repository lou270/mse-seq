/******************************
| Project       : MSE Avionics
| Board         : SEQ
| Description   : Sequencer code
| Licence       : CC BY-NC-SA 
| https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
******************************/
#include "sequencer.h"

uint64_t currTime, prevTime = 0;
double launchTime, apogeeTime = 0;

RocketState_t rocketState = PRE_FLIGHT;

void sequencer() {
    switch (rocketState) {
        case PRE_FLIGHT:
            seqPreLaunch();
            break;
        case ASCEND:
            seqAscend();
            break;
        case DEPLOY:
            seqDeploy();
            break;
        case DESCEND:
            seqDescend();
            break;
        case TOUCHDOWN:
            seqTouchdown();
            break;
        
        default:
            rocketState = PRE_FLIGHT;
            break;
    }
}

void seqPreLaunch() {
    if(launchDetection() == true) {
        #if DEBUG == true
        Serial.println(F("Launch detected"));
        #endif
        rocketState = ASCEND;
        // Launch apogee timer
        launchTime = rp2040.getCycleCount64()/(rp2040.f_cpu()/1000.0);
        // Change buzzer sound
        setBuzzer(BUZ_ON, BUZ_TIME_AFTER_LAUNCH, 100);
        #if DEBUG == true
        Serial.println(F("Waiting for apogee detection..."));
        #endif
    } else {
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
        currTime = (rp2040.getCycleCount64()/(rp2040.f_cpu()/1000));
        if ((currTime - prevTime) >= 250) {
            prevTime = currTime;
            char txt[60] = "";
            sprintf(txt, "[IMU] angle X: %.2f | angle Y: %.2f | angle Z: %.2f", angle->x, angle->y, angle->z);
            Serial.println(txt);
        }
        #endif
        rocketState = PRE_FLIGHT;
    }
}

void seqAscend() {
    Angle_t * angle = computeAngle();
    currTime = (rp2040.getCycleCount64()/(rp2040.f_cpu()/1000)) - (int)launchTime;
    if (currTime > START_WINDOW_TIME) {
        if(apogeeDetection(angle)) {
            rocketState = DEPLOY;
            apogeeTime = (rp2040.getCycleCount64()/(rp2040.f_cpu()/1000.0));
            #if DEBUG == true
            Serial.println(F("Apogee detection done (ALGO), opening parachute door..."));
            #endif
        }
    }
    if (currTime > END_WINDOW_TIME) {
        rocketState = DEPLOY;
        apogeeTime = (rp2040.getCycleCount64()/(rp2040.f_cpu()/1000.0));
        #if DEBUG == true
        Serial.println(F("Apogee detection done (TIMER), opening parachute door..."));
        #endif
    }
}

void seqDeploy() {
    // Change buzzer sound
    setBuzzer(BUZ_ON, BUZ_TIME_AFTER_APOGEE, 2000);
    openParachuteDoor();
    rocketState = DESCEND;
    #if DEBUG == true
    Serial.println(F("Done opening parachute door"));
    #endif
}

void seqDescend() {
    rocketState = TOUCHDOWN;
    if(launchDetection() == true) {
        rocketState = TOUCHDOWN;
    } else {
        rocketState = DESCEND;
    }
}

void seqTouchdown() {
    // Nothing for now
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

