/******************************
| Project       : MSE Avionics
| Board         : SEQ
| Description   : Motor management
| Licence       : CC BY-NC-SA 
| https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
******************************/
#ifndef MOTOR_HEADER_FILE
#define MOTOR_HEADER_FILE

#include <Arduino.h>
#include <pico/time.h>
#include "board.h"
#include "parameters.h"

void writeAllMotor(uint8_t val);
void writeMotor(uint8_t motor, uint8_t val);
void closeParachuteDoor();
void openParachuteDoor();
int64_t stopMotorCallback(alarm_id_t id, void *user_data);

#endif