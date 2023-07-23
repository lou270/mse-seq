/******************************
| Project       : MSE Avionics
| Board         : SEQ
| Description   : Motor management
| Licence       : CC BY-NC-SA 
| https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
******************************/
#ifndef SEQUENCER_HEADER_FILE
#define SEQUENCER_HEADER_FILE

#include <Arduino.h>
#include <pico/time.h>
#include "board.h"
#include "parameters.h"
#include "imu.h"
#include "buzzer.h"
#include "motor.h"

typedef enum {PRE_FLIGHT = 0, PYRO_RDY, ASCEND, DEPLOY_ALGO, DEPLOY_TIMER, DESCEND, TOUCHDOWN} RocketState_t;

RocketState_t seqPreLaunch();
RocketState_t seqPyroRdy();
RocketState_t seqAscend();
RocketState_t seqDeploy();
RocketState_t seqDescend();
RocketState_t seqTouchdown();

void processSensors();
bool launchDetection();
bool apogeeDetection(DerivPressure_t* derivP);
void sendInfoPld(RocketState_t st);
void getPressureDerivate(DerivPressure_t* derivP, float_t currPressure, uint64_t currTime);

// void setOnCameraTrigger();
// void setOffCameraTrigger();
void setCameraTrigger(uint8_t pinState);
int64_t setOffCameraTriggerCallback(alarm_id_t id, void *user_data);

#endif