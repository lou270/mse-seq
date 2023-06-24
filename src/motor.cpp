/******************************
| Project       : MSE Avionics
| Board         : SEQ
| Description   : Motor management
| Licence       : CC BY-NC-SA 
| https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
******************************/
#include "motor.h"

void writeMotor(uint16_t val) {
    digitalWrite(M1_A_PIN, val & 0x1);
    digitalWrite(M1_B_PIN, val & 0x2);
    digitalWrite(M2_A_PIN, val & 0x1);
    digitalWrite(M2_B_PIN, val & 0x2);
}

void closeParachuteDoor() {
    writeMotor(CLOSE_PARA_DOOR_VAL);
    delay(TIME_CLOSE_PARA_DOOR);
    writeMotor(0);
}

void openParachuteDoor() {
    writeMotor(OPEN_PARA_DOOR_VAL);
    delay(TIME_OPEN_PARA_DOOR);
    writeMotor(0);
}
