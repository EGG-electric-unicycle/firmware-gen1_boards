/*  Copyright (C) 2015 Joerg Hoener

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stm32f10x.h"
#include "imu.h"
#include "math.h"
#include "MPU6050/MPU6050.h"

// Y axis gives the front/rear inclination of the wheel
// Z axis gives the lateral inclination of the wheel

static s16 accel_gyro[6];
static s16 accel_gyro_average[6];
static float accel_x = 0;
static float accel_y = 0;
static float accel_z = 0;
static float pitch = 0;
static float roll = 0;
static float gyro_x = 0;
static float gyro_y = 0;
static float gyro_z = 0;
static float gyro_angle_x = 0;
static float gyro_angle_y = 0;
static float gyro_angle_z = 0;

static float angle_x = 0;

BOOL IMU_init(void)
{
  MPU6050_I2C_Init();
  MPU6050_Initialize();
  return MPU6050_TestConnection();
}

BOOL IMU_read(void)
{
  unsigned int x = 0;

  for (x = 0; x < 25; x++)
  {
    MPU6050_GetRawAccelGyro (accel_gyro);

    accel_x = accel_gyro[0] / ACCEL_SENSITIVITY;
    accel_y = accel_gyro[1] / ACCEL_SENSITIVITY;
    accel_z = accel_gyro[2] / ACCEL_SENSITIVITY;
    //  pitch = atan2(accel_y, accel_z) + PI;
    //  roll = atan2(accel_x, accel_z) + PI;

    gyro_x = accel_gyro[3] / GYRO_SENSITIVITY;
    gyro_y = accel_gyro[4] / GYRO_SENSITIVITY;
    gyro_z = accel_gyro[5] / GYRO_SENSITIVITY;
    gyro_angle_x += gyro_x * (1/1000); //dt = 1ms
    gyro_angle_y += gyro_y * (1/1000);
    gyro_angle_z += gyro_z * (1/1000);

    //kalmanCalculate (angle_x, gyro_angle_x, )

    accel_gyro_average[0] -= (accel_gyro_average[0] / 25);
    accel_gyro_average[0] += (accel_gyro[0] / 25);

    accel_gyro_average[1] -= (accel_gyro_average[1] / 25);
    accel_gyro_average[1] += (accel_gyro[1] / 25);

    accel_gyro_average[2] -= (accel_gyro_average[2] / 25);
    accel_gyro_average[2] += (accel_gyro[2] / 25);
  }

  printf ("Ax: %d ** Ay: %d ** Az: %d\n", (accel_gyro_average[0]), (accel_gyro_average[1]), (accel_gyro_average[2]));
}

// KasBot V1  -  Kalman filter module
float Q_angle  =  0.0005; //0.001
float Q_gyro   =  0.003;  //0.003
float R_angle  =  0.05;  //0.03

float x_angle = 0.0;
float x_bias = 0.0;
float P_00 = 0.0, P_01 = 0.0, P_10 = 0.0, P_11 = 0.0;
float dt, y, S;
float K_0, K_1;

// newAngle = angle measured with atan2 using the accelerometer
// newRate = angle measured using the gyro
// looptime = loop time in millis()
float kalmanCalculate(float newAngle, float newRate, float looptime)
{
  dt = (float)((looptime)/1000);                                    // XXXXXXX arevoir
  x_angle += dt * (newRate - x_bias);
  P_00 +=  - dt * (P_10 + P_01) + Q_angle * dt;
  P_01 +=  - dt * P_11;
  P_10 +=  - dt * P_11;
  P_11 +=  + Q_gyro * dt;

  y = newAngle - x_angle;
  S = P_00 + R_angle;
  K_0 = P_00 / S;
  K_1 = P_10 / S;

  x_angle +=  K_0 * y;
  x_bias  +=  K_1 * y;
  P_00 -= K_0 * P_00;
  P_01 -= K_0 * P_01;
  P_10 -= K_1 * P_00;
  P_11 -= K_1 * P_01;

  return x_angle;
}
