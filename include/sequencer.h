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
#include "board.h"
#include "parameters.h"
#include "imu.h"
#include "buzzer.h"
#include "motor.h"

typedef enum {PRE_FLIGHT, ASCEND, DEPLOY, DESCEND, TOUCHDOWN} RocketState_t;

void sequencer(void);

void seqPreLaunch();
void seqAscend();
void seqDeploy();
void seqDescend();
void seqTouchdown();

bool launchDetection();
bool apogeeDetection(Angle_t *angle);

#endif