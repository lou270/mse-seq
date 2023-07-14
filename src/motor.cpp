/******************************
| Project       : MSE Avionics
| Board         : SEQ
| Description   : Motor management
| Licence       : CC BY-NC-SA 
| https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
******************************/
#include "motor.h"

void writeAllMotor(uint8_t val) {
    digitalWrite(M1_A_PIN, val & 0x1);
    digitalWrite(M1_B_PIN, val & 0x2);
    digitalWrite(M2_A_PIN, val & 0x1);
    digitalWrite(M2_B_PIN, val & 0x2);
}

void writeMotor(uint8_t motor, uint8_t val) {
    if (motor == 1) {
        digitalWrite(M1_A_PIN, val & 0x1);
        digitalWrite(M1_B_PIN, val & 0x2);
    } else if (motor == 2) {
        digitalWrite(M2_A_PIN, val & 0x1);
        digitalWrite(M2_B_PIN, val & 0x2);
    }
}

void closeParachuteDoorBlocking() {
    writeAllMotor(CLOSE_PARA_DOOR_VAL);
    delay(TIME_CLOSE_PARA_DOOR);
    writeAllMotor(0);
}

void openParachuteDoorBlocking() {
    writeAllMotor(OPEN_PARA_DOOR_VAL);
    delay(TIME_OPEN_PARA_DOOR);
    writeAllMotor(0);
}

int64_t stopMotorCallback(alarm_id_t id, void *user_data) {
    writeAllMotor(0);
    return 0;
}
