/******************************
| Project       : MSE Avionics
| Board         : SEQ
| Description   : IMU header
| Licence       : CC BY-NC-SA 
| https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
******************************/
#ifndef IMU_HEADER_FILE
#define IMU_HEADER_FILE

#include <Arduino.h>
#include "Wire.h"
#include "board.h"
#include "parameters.h"
#include "MPU9250.h"
#include "lps22hb.h"

#define WHO_AM_I_MPU9250_VAL 0x71

// IMU parameters
#define IMU_ACC_RES AFS_16G
#define IMU_GYRO_RES GFS_2000DPS
#define IMU_MAG_RES MFS_16BITS

// IMU data type
typedef struct {
    // Raw values
    int16_t raw_ax, raw_ay, raw_az;
    int16_t raw_gx, raw_gy, raw_gz;
    int16_t raw_mx, raw_my, raw_mz;

    // True values
    float ax, ay, az;
    float gx, gy, gz;
    float mx, my, mz;
} Imu_t;

// Angle data type
typedef struct  {
    // Angle
    double x, y, z;
} Angle_t;

// Kalman structure type
typedef struct {
    double Q_angle;
    double Q_bias;
    double R_measure;
    double angle;
    double bias;
    double P[2][2];
} Kalman_t;

void setupIMU(void);
void readIMUData(void);
Angle_t * computeAngle(void);
void rawToSi(Imu_t *imuData);
double getAngleKalman(Kalman_t *Kalman, double newAngle, double newRate, double dt);

#endif
