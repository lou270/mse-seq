/******************************
| Project       : MSE Avionics
| Board         : SEQ
| Description   : Buzzer management
| Licence       : CC BY-NC-SA 
| https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
******************************/

#include "buzzer.h"

struct repeating_timer buzzerTimer;
volatile bool buzzerEnable = false;
volatile uint16_t buzzerBeatDuration = 0;

void setBuzzer(bool enable, uint16_t beatPeriod, uint16_t beatDuration) {
    bool a = cancel_repeating_timer(&buzzerTimer);
    buzzerEnable = enable;
    if (!enable) {
        // noTone(BUZZER_PIN);
        // digitalWrite(BUZZER_PIN, 0);
        analogWrite(BUZZER_PIN, 0);
    }
    if (beatPeriod > 0) {
        buzzerBeatDuration = beatDuration;
        a = add_repeating_timer_ms(beatPeriod, buzzerCallback, NULL, &buzzerTimer);
    }
}

bool buzzerCallback(struct repeating_timer *t) {
    if(buzzerEnable) {
        analogWrite(BUZZER_PIN, BUZ_POWER/100.0 * MAX_VAL_PWM);
        add_alarm_in_ms(buzzerBeatDuration, buzzerStopCallback, NULL, true);
    }
    bool wut = digitalRead(PICO_LED_PIN);
    digitalWrite(PICO_LED_PIN, !wut);
    return true;
}

int64_t buzzerStopCallback(alarm_id_t id, void *user_data) {
    analogWrite(BUZZER_PIN, 0);
    return 0;
}
