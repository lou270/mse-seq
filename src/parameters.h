/******************************
| Project       : MSE Avionics
| Board         : SEQ
| Description   : Parameters
| Licence       : CC BY-NC-SA 
| https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
******************************/

// Debug
#define DEBUG true

// Buzzer
#define BUZ_ON true
#define BUZ_FREQ 1500 // [Hz]
#define BUZ_TIME_BEFORE_LAUNCH 3000 // [ms]
#define BUZ_TIME_AFTER_LAUNCH 1000 // [ms]
#define BUZ_TIME_AFTER_APOGEE 10000 // [ms]
// enum buzzer_power_enum {low, med, high};
#define BUZ_POWER low // low - med - high

// Window timer
#define START_WINDOW_TIME 10000 // [ms]
#define END_WINDOW_TIME 20000 // [ms]

// Parachute motors
#define CLOSE_PARA_DOOR_VAL 0x1
#define TIME_CLOSE_PARA_DOOR 5000 // [ms]
#define OPEN_PARA_DOOR_VAL 0x2
#define TIME_OPEN_PARA_DOOR 5000 // [ms]
