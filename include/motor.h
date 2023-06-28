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
#include "board.h"
#include "parameters.h"

void writeMotor(uint16_t val);
void closeParachuteDoor();
void openParachuteDoor();

#endif