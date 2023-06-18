/******************************
| Project       : MSE Avionics
| Board         : SEQ
| Description   : Buzzer management header
| Licence       : CC BY-NC-SA 
| https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
******************************/
#ifndef BUZZER_HEADER_FILE
#define BUZZER_HEADER_FILE

#include <Arduino.h>
#include <pico/time.h>
#include "board.h"
#include "parameters.h"

void setBuzzer(bool enable, uint16_t beatPeriod = 0, uint16_t beatDuration = 0);
bool buzzerCallback(struct repeating_timer *t);
int64_t buzzerStopCallback(alarm_id_t id, void *user_data);

#endif
