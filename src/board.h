/*************
* Project : MSE Avionics
* Board : SEQ
* Description : Board description
**************/

// ==== PIN

// Cam
#define CAM_N1_PIN 3
#define CAM_N2_PIN 2

// Parachute mechanism - Motors
#define M1_A_PIN 16
#define M1_B_PIN 17
#define M2_A_PIN 18
#define M2_B_PIN 19
#define M1_OPEN_PIN 11
#define M1_CLOSE_PIN 12
#define M2_OPEN_PIN 13
#define M2_CLOSE_PIN 14

// UART SEQ <-> PLD
#define SEQ_PLD_UART_TX_PIN 0
#define SEQ_PLD_UART_RX_PIN 1

// UART IHM
#define IHM_UART_TX_PIN 8
#define IHM_UART_RX_PIN 9

// Voltage acquisition
#define VBAT_M_PIN A0 // Pin 26 ADC0
#define VBAT_SEQ_PIN A1 // Pin 27 ADC1 
#define VD_DIV (2.0/(10.0+2.0))

// Buzzer
#define BUZZER_PIN 10
