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

typedef enum {PRE_FLIGHT = 0, ASCEND, DEPLOY_ALGO, DEPLOY_TIMER, DESCEND, TOUCHDOWN} RocketState_t;

RocketState_t seqPreLaunch();
RocketState_t seqAscend();
RocketState_t seqDeploy();
RocketState_t seqDescend();
RocketState_t seqTouchdown();

bool launchDetection();
bool apogeeDetection(Angle_t *angle);
void sendInfoPld(RocketState_t st);

#endif