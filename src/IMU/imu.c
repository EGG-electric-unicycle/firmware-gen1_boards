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
#include "MPU6050/MPU6050.h"

BOOL IMU_init(void)
{
  MPU6050_I2C_Init();
  MPU6050_Initialize();
  return MPU6050_TestConnection();
}

void IMU_startDMAtransfer()
{
  I2C_DMA_Read(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_XOUT_H);
}

// KasBot V1 - Kalman filter module
float Q_angle  =  0.01;
float Q_gyro   =  0.0003;
float R_angle  =  0.01;

float x_bias = 0;
float P_00 = 0, P_01 = 0, P_10 = 0, P_11 = 0;
float  y, S;
float K_0, K_1;

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
