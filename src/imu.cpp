/******************************
| Project       : MSE Avionics
| Board         : SEQ/PLD
| Description   : IMU board
| Licence       : CC BY-NC-SA 
| https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
******************************/
#include "imu.h"

MPU9250 imu(MPU_INT_PIN);

static float magCalib[3] = {0};
static uint64_t timer = 0;
bool imuInitialised = 0;

/* initialiaze the X axis Kalman */
static Kalman_t kalmanX = {
    .Q_angle   = 0.001f,
    .Q_bias    = 0.003f,
    .R_measure = 0.03f
};
/* initialiaze the Y axis Kalman */
static Kalman_t kalmanY = {
    .Q_angle   = 0.001f,
    .Q_bias    = 0.003f,
    .R_measure = 0.03f,
};

void setupIMU() {
    if(imu.readByte(MPU9250_1_ADDRESS, WHO_AM_I_MPU9250) == WHO_AM_I_MPU9250_VAL) {
        imu.initMPU9250(MPU9250_1_ADDRESS, IMU_ACC_RES, IMU_GYRO_RES, 4);
        imu.initAK8963Slave(MPU9250_1_ADDRESS, IMU_MAG_RES, M_100Hz, magCalib);
        #if DEBUG == true
        Serial.println(F("[IMU] Init done"));
        #endif
        imuInitialised = 1;
    } else {
        imuInitialised = 0;
        #if DEBUG == true
        Serial.println(F("[IMU] Init failed /!\\"));
        #endif
    }
}

void getImuData(Imu_t* imuData, bool acqMag) {
    #if DEBUG == true
    // uint64_t firstTime = rp2040.getCycleCount64();
    #endif

    imu.readAccelData(MPU9250_1_ADDRESS, &imuData->raw_ax);
    imu.readGyroData(MPU9250_1_ADDRESS, &imuData->raw_gx);
    if (acqMag) {
        imu.readMagData(MPU9250_1_ADDRESS, &imuData->raw_mx);
    }

    #if DEBUG == true
    // uint64_t lastTime = rp2040.getCycleCount64();
    #endif
    
    rawToSi(imuData);
    
    #if DEBUG == true
        Serial.printf("[IMU] ax: %.2f | ay: %.2f | az: %.2f\n", imuData->ax, imuData->ay, imuData->az);
        Serial.printf("[IMU] gx: %.2f | gy: %.2f | gz: %.2f\n", imuData->gx, imuData->gy, imuData->gz);
        Serial.printf("[IMU] mx: %.2f | my: %.2f | mz: %.2f\n", imuData->mx, imuData->my, imuData->mz);
        // Serial.printf("[IMU] Time acq : %.2f", (lastTime-firstTime)*1000.0/rp2040.f_cpu());
    #endif
}

/** ************************************************************* *
 * @brief       read all and apply the kalman filter to the result
 * 
 * @return      Angle computed 
 * ************************************************************* **/
void computeAngle(Imu_t* imuData, Angle_t* angle) {
    // Kalman angle solve
    double dt = (double) (rp2040.getCycleCount64() - timer) / rp2040.f_cpu();
    timer = rp2040.getCycleCount64();

    double roll;
    double roll_sqrt = sqrt(imuData->raw_ax * imuData->raw_ax + imuData->raw_az * imuData->raw_az);
    if (roll_sqrt != 0.0) {
        roll = atan(imuData->raw_ay / roll_sqrt) * RAD_TO_DEG;
    } 
	else {
        roll = 0.0;
    }

    double pitch = atan2(-imuData->raw_ax, imuData->raw_az) * RAD_TO_DEG;
    if((pitch < -90 && angle->y > 90) 
	    || (pitch > 90 && angle->y < -90)) {
        kalmanY.angle = pitch;
        angle->y = pitch;
    }  
	else {
        angle->y = getAngleKalman(&kalmanY, pitch, imuData->gy, dt);
    }

    if (fabs(angle->y) > 90) 
        imuData->gx = -imuData->gx;
    angle->x = getAngleKalman(&kalmanX, roll, imuData->gy, dt);


    // double yaw = atan2(imuData->raw_ax, imuData->raw_ay) * RAD_TO_DEG;
    // double pitch = atan2(imuData->raw_az, imuData->raw_ay) * RAD_TO_DEG;

    // angle->x = getAngleKalman(&kalmanX, yaw, imuData->gz, dt);
    // angle->y = getAngleKalman(&kalmanY, pitch, imuData->gx, dt);
}

void rawToSi(Imu_t *imuData) {
    imuData->ax = imuData->raw_ax * imu.getAres(IMU_ACC_RES);
    imuData->ay = imuData->raw_ay * imu.getAres(IMU_ACC_RES);
    imuData->az = imuData->raw_az * imu.getAres(IMU_ACC_RES);
    imuData->gx = imuData->raw_gx * imu.getGres(IMU_GYRO_RES);
    imuData->gy = imuData->raw_gy * imu.getGres(IMU_GYRO_RES);
    imuData->gz = imuData->raw_gz * imu.getGres(IMU_GYRO_RES);
    imuData->mx = imuData->raw_mx * imu.getMres(IMU_MAG_RES);
    imuData->my = imuData->raw_my * imu.getMres(IMU_MAG_RES);
    imuData->mz = imuData->raw_mz * imu.getMres(IMU_MAG_RES);
}

/** ************************************************************* *
 * @brief       Apply the kalman filter with the input args and
 *              return the computed angle
 * 
 * @param       Kalman Kalman structure
 * @param       newAngle New angle from IMU data
 * @param       newRate New rate from IMU gyro
 * @param       dt Delta time
 * @return      Angle computed 
 * ************************************************************* **/
double getAngleKalman(Kalman_t *Kalman, double newAngle, double newRate, double dt) {
    double rate = newRate - Kalman->bias;
    Kalman->angle += dt * rate;

    Kalman->P[0][0] += dt * (dt * Kalman->P[1][1] - Kalman->P[0][1] - Kalman->P[1][0] + Kalman->Q_angle);
    Kalman->P[0][1] -= dt * Kalman->P[1][1];
    Kalman->P[1][0] -= dt * Kalman->P[1][1];
    Kalman->P[1][1] += Kalman->Q_bias * dt;

    double S = Kalman->P[0][0] + Kalman->R_measure;
    double K[2];
    K[0] = Kalman->P[0][0] / S;
    K[1] = Kalman->P[1][0] / S;

    double y = newAngle - Kalman->angle;
    Kalman->angle += K[0] * y;
    Kalman->bias += K[1] * y;

    double P00_temp = Kalman->P[0][0];
    double P01_temp = Kalman->P[0][1];

    Kalman->P[0][0] -= K[0] * P00_temp;
    Kalman->P[0][1] -= K[0] * P01_temp;
    Kalman->P[1][0] -= K[1] * P00_temp;
    Kalman->P[1][1] -= K[1] * P01_temp;

    return Kalman->angle;
}

