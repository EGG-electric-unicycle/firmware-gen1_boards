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
#include "tinystdio/tinystdio.h"
#include "motor.h"

// X axis gives the front/rear inclination of the wheel
// Z axis gives the lateral inclination of the wheel

static s16 accel_gyro[6];
static float accel_gyro_average[6];

BOOL IMU_init(void)
{
  unsigned int i;

  MPU6050_I2C_Init();
  MPU6050_Initialize();

  // if the MPU6050 is ready, make "calibration"
  // read the sensor values and average
  if (MPU6050_TestConnection())
  {
    accel_gyro_average[0] = 0;
    accel_gyro_average[1] = 0;
    accel_gyro_average[2] = 0;
    accel_gyro_average[3] = 0;
    accel_gyro_average[4] = 0;
    accel_gyro_average[5] = 0;

    for (i = 0; i <= 10; i++)
    {
      MPU6050_GetRawAccelGyro (accel_gyro);

      accel_gyro_average[0] += accel_gyro[0];
      accel_gyro_average[1] += accel_gyro[1];
      accel_gyro_average[2] += accel_gyro[2];
      accel_gyro_average[3] += accel_gyro[3];
      accel_gyro_average[4] += accel_gyro[4];
      accel_gyro_average[5] += accel_gyro[5];

      delay_ms(50); //wait for 50ms for the gyro to stable
    }

    accel_gyro_average[0] /= 10;
    accel_gyro_average[1] /= 10;
    accel_gyro_average[2] /= 10;
    accel_gyro_average[3] /= 10;
    accel_gyro_average[4] /= 10;
    accel_gyro_average[5] /= 10;

    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

BOOL IMU_read(void)
{
  float acc_x;
  float acc_y;
  float acc_z;
  float angle;
  static float gyro_rate;
  float dt;
  unsigned int micros_new;
  static unsigned int micros_old = 0;
  static unsigned int timer_1s = 0;

  // read the accel and gyro sensor values
  MPU6050_GetRawAccelGyro (accel_gyro);

  acc_x = accel_gyro[0];
  acc_y = accel_gyro[1];
  acc_z = accel_gyro[2];
  gyro_rate = accel_gyro[5] * GYRO_SENSITIVITY;

  // calc dt, using micro seconds value
  micros_new = micros ();
  dt = (micros_new - micros_old) / 1000000.0;
  micros_old = micros_new;

  angle = atan2(acc_x, acc_z); //calc angle between X and Y axis, in rads
  angle = (angle + PI) * RAD_TO_DEG; //convert from rads to degres
  angle = 0.98 * (angle + (gyro_rate * dt)) + 0.02 * (acc_y); //use the complementary filter.

  //control the motor now using the angle value
  // for testing the board in horizontal
  // z = 2047 when the board is parallel to ground
  // angle measure between x and z axis
  // angle after Complementary filter: 180 degres with board on horizontal
  //
  // Make the system work with max inclination of +- 10 degres --> 170 up to 190
  angle -= 180;
  if (angle > 9) angle = 10;
  if (angle < -9) angle = -10;
  angle *= 50;
  motor_set_duty_cycle (angle); // duty-cycle between -300 and 300 only! on this test


  timer_1s++;
  if (timer_1s > 99)
  {
    timer_1s = 0;

    //printf ("x: %3.3f :", acc_x);
    //printf ("y: %3.3f :", acc_y);
    //printf ("z: %3.3f :", acc_z);
    //printf ("angle: %3.3f : \n", angle);
  }
}
