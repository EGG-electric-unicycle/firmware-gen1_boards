
#include "stm32f10x.h"
#include "imu.h"

/**
 * Implementation of IMU interface for MPU6050
 */
#ifdef IMU_IS_MPU6050

#include "MPU6050/MPU6050.h"



#ifdef IMU_MODE_POLL
BOOL IMU_init() {
   MPU6050_I2C_Init();
   MPU6050_Initialize();
   return MPU6050_TestConnection();
}


void IMU_getData(float* fwBkAngle, uint8_t* sideLean);
   
}

#elif IMU_MODE_DMA

void (*IMU_dataAvail)(float*, uint8_t*);

BOOL IMU_init(void (*dataAvailable)(float*, uint8_t*)) {
   IMU_dataAvail = dataAvailable;
   MPU6050_I2C_Init();
   MPU6050_Initialize();
   return MPU6050_TestConnection();
}

void IMU_startDMAtransfer() {
   I2C_DMA_Read(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_XOUT_H);
}

#endif

float fwBkOldAngle = 0.0;
float leftRightOldAngle = 0.0;

void IMU_MPU6050_rawConvert(int16_t upDownAccel, int16_t frontBackAccel,
                            int16_t leftRightAccel, int16_t frontBackGyro,
			    int16_t leftRightGyro,
			    float* fwBkAngle, uint8_t* sideLean) {
   // calculate angle from accelerations (assuming 2g scale)
   
   
   // PI/2 - acos([frontBackAccel|leftRightAccel]/16383,5)
   float fwBkAccelAngle       = PI/2.0 - acos(frontBackAccel/16383.5);
   float leftRightAccelAngle  = PI/2.0 - acos(leftRightAccel/16383.5);
   
   // calculate gyro rate in rad/s
   float fwBkGyroRadS = 0.0;
   float fwBkGyroRadS = 0.0;
   
   *fwBkAngle = callFilter(fwBkOldAngle, fwBkAccelAngle, fwBkGyroRadS, int looptime);
   fwBkOldAngle = *fwBkAngle;
   
   
   float leftRightAngle = callFilter(leftRightOldAngle, leftRightAccelAngle,
                                     leftRightGyroRadS, int looptime);
   leftRightOldAngle = leftRightAngle;
   
   if (leftRightAngle > DEG45) {
      *sideLean = 2;
   } else if (leftRightAngle < -DEG45) {
      *sideLean = 1;
   } else {
      *sideLean = 0;
   }

}

#endif


/**
 * Filter implementations and selection
 */

#ifdef IMU_FILTER_COMPL
// a=tau / (tau + loop time)
float tau=0.075;
float a=0.0;

float Complementary(float oldAngle, float newAngle, float newRate,int looptime) {
    float dtC = ((float)looptime)/1000.0;
    a = tau/(tau+dtC);
    oldAngle = a* (oldAngle + newRate * dtC) + (1-a) * (newAngle);
    return oldAngle;
}
#endif



#ifdef IMU_FILTER_COMPL2
float Complementary2(float oldAngle, float newAngle, float newRate,int looptime) {
    float k=10;
    float dtc2=((float)looptime)/1000.0;

    x1 = (newAngle -   oldAngle)*k*k;
    y1 = dtc2*x1 + y1;
    x2 = y1 + (newAngle -   oldAngle)*2*k + newRate;
    oldAngle = dtc2*x2 + oldAngle;

    return oldAngle;
}


#ifdef IMU_FILTER_KALMAN
// KasBot V1 - Kalman filter module

float Q_angle  =  0.01; //0.001
float Q_gyro   =  0.0003;  //0.003
float R_angle  =  0.01;  //0.03

float x_bias = 0;
float P_00 = 0, P_01 = 0, P_10 = 0, P_11 = 0;
float  y, S;
float K_0, K_1;

// newAngle = angle measured with atan2 using the accelerometer
// newRate = angle measured using the gyro
// looptime = loop time in millis()

float kalmanCalculate(float oldAngle, float newAngle, float newRate,int looptime)
{
    float dt = ((float)looptime)/1000.0;
    oldAngle += dt * (newRate - x_bias);
    P_00 +=  - dt * (P_10 + P_01) + Q_angle * dt;
    P_01 +=  - dt * P_11;
    P_10 +=  - dt * P_11;
    P_11 +=  + Q_gyro * dt;

    y = newAngle - oldAngle;
    S = P_00 + R_angle;
    K_0 = P_00 / S;
    K_1 = P_10 / S;

    oldAngle +=  K_0 * y;
    x_bias  +=  K_1 * y;
    P_00 -= K_0 * P_00;
    P_01 -= K_0 * P_01;
    P_10 -= K_1 * P_00;
    P_11 -= K_1 * P_01;

    return oldAngle;
}
#endif


// Filter interface function
// newAngle = angle measured with atan2 using the accelerometer
// newRate = angle measured using the gyro
// looptime = loop time in millis()
float callFilter(float oldAngle, float newAngle, float newRate,int looptime) {
   #ifdef IMU_FILTER_COMPL
   return Complementary(oldAngle, newAngle, newRate, looptime);
   #elif IMU_FILTER_COMPL2
   return Complementary2(oldAngle, newAngle, newRate, looptime);
   #elif IMU_FILTER_KALMAN
   return kalmanCalculate(oldAngle, newAngle, newRate, looptime)
   #endif
}

//int16_t  AccelGyro[6]={0};
//MPU6050_GetRawAccelGyro(AccelGyro);

#endif
