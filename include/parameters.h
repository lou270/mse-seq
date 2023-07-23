/******************************
| Project       : MSE Avionics
| Board         : SEQ
| Description   : Parameters
| Licence       : CC BY-NC-SA 
| https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
******************************/
#ifndef PARAMETER_HEADER_FILE
#define PARAMETER_HEADER_FILE

#define MSE
// #define KRIPTONIT

#if defined(MSE)
    // Debug
    // #define DEBUG true

    // PWM
    #define FREQ_PWM 500
    #define MAX_VAL_PWM 65535 // min:16 max:65535

    // Buzzer
    #define BUZ_ON true
    #define BUZ_FREQ 500 // [Hz]
    #define BUZ_TIME_PRE_LAUNCH 10000 // [ms]
    #define BUZ_TIME_PYRO_RDY 1000 // [ms]
    #define BUZ_TIME_ASCEND 140 // [ms]
    #define BUZ_TIME_DESCEND 500 // [ms]
    #define BUZ_TIME_RECUP 30000 // [ms]
    enum buzzer_power_enum {low = 5, med = 50, high = 100}; // % of buzzer
    #define BUZ_POWER high // low - med - high

    // Apogee
    #define START_WINDOW_TIME 18000 // [ms]
    #define END_WINDOW_TIME 18000 // [ms]
    #define ANGLE_DET_APOGEE 100 // [°]
    #define THRESHOLD_DERIV_BARO_APOGEE -0.1
    #define BARO_MEAN_NB 100
    #define TIME_AFTER_APOGEE_TOUCHDOWN 800000

    // Parachute motors
    #define INPUT_CLOSE_MOT_ACTION true
    #define CLOSE_PARA_DOOR_VAL 0x2
    #define TIME_CLOSE_PARA_DOOR 20000 // [ms]
    #define OPEN_PARA_DOOR_VAL 0x1
    #define TIME_OPEN_PARA_DOOR 20000 // [ms]

#elif defined(KRIPTONIT)
    // Debug
    #define DEBUG true

    // PWM
    #define FREQ_PWM 500
    #define MAX_VAL_PWM 65535 // min:16 max:65535

    // Buzzer
    #define BUZ_ON false
    #define BUZ_FREQ 500 // [Hz]
    #define BUZ_TIME_PRE_LAUNCH 3000 // [ms]
    #define BUZ_TIME_ASCEND 1000 // [ms]
    #define BUZ_TIME_DESCEND 10000 // [ms]
    enum buzzer_power_enum {low = 5, med = 50, high = 100}; // % of buzzer
    #define BUZ_POWER low // low - med - high

    // Apogee
    #define START_WINDOW_TIME 1000 // [ms]
    #define END_WINDOW_TIME 4000 // [ms]
    #define ANGLE_DET_APOGEE 100 // [°]

    // Parachute motors
    #define INPUT_CLOSE_MOT_ACTION false
    #define CLOSE_PARA_DOOR_VAL 0x2
    #define TIME_CLOSE_PARA_DOOR 30000 // [ms]
    #define OPEN_PARA_DOOR_VAL 0x1
    #define TIME_OPEN_PARA_DOOR 30000 // [ms]
#else
    #error "Define a project in parameter.h before compilation"
#endif

#endif
