/*
 * Generic Electric Unicycle firmware
 *
 * Copyright (C) Casainho, 2015.
 *
 * Released under the GPL License, Version 3
 */

/* Connections:
 *
 * Motor PHASE_A: green wire
 * Motor PHASE_B: yellow wire
 * Motor PHASE_B: blue wire
 *
 *
 * PIN				| IN/OUT| Works?|Function
 * ----------------------------------------------------------
 *
 * PA8  (TIM1_CH1)  		| out	| yes	| Bridge_C-High
 * PA9  (TIM1_CH2)  		| out	| yes	| Bridge_B-High
 * PA10 (TIM1_CH3)  		| out	| yes	| Bridge_A-High
 * PB13 (TIM1_CH1N) 		| out	| yes	| Bridge_C-Low (active low)
 * PB14 (TIM1_CH2N) 		| out	| yes	| Bridge_B-Low (active low)
 * PB15 (TIM1_CH3N) 		| out	| yes	| Bridge_A-Low (active low)
 *
 * PA2  (TIM2_CH3) 		| in	| yes	| Hall_sensor_A
 * PA1  (TIM2_CH2) 		| in	| yes	| Hall_sensor_B
 * PA0  (TIM2_CH1) 		| in	| yes	| Hall_sensor_C

 * PA6  (ADC12_IN6) 		| in	| ??	| BMF_signal-Yellow_A
 * PB0  (ADC12_IN8) 		| in	| ??	| BMF_signal-Green_B
 * PB1  (ADC12_IN9) 		| in	| ??	| BMF_signal-Blue_C
 *
 * PA5  (ADC12_IN5)		| in	| yes	| Battery_voltage_signal
 * PA7  (ADC12_IN7)		| in	| yes	| Motor_current_signal
 *
 * PB6  (I2C1_SCL)		| in/out| ??	| IMU_MPU6050-SCL
 * PB7  (I2C1_SDA)		| in/out| ??	| IMU_MPU6050-SDA
 *
 * PA15 			| out	| yes	| LED_1-battery_indicator (active low: float to disable and GND to turn on)
 * PB4	 			| out	| yes	| LED_2-battery_indicator (active low: float to disable and GND to turn on)
 * PB5	 			| out	| yes	| LED_3-battery_indicator (active low: float to disable and GND to turn on)
 * PB8	 			| out	| yes	| LED_4-battery_indicator (active low: float to disable and GND to turn on)
 * PB9	 			| out 	| yes	| LED-power_switcher	  (active low: float to disable and GND to turn on)
 *
 * PB3	 			| out	| yes	| Buzzer 		  (active high: push pull)
 * PA4	 			| in	| yes	| PS_signal 		  (calibrate_wheel)
 *
 */

#ifndef GPIO_H
#define GPIO_H

#define LED_1_BATTERY_INDICATOR		GPIO_Pin_15
#define LED_2_BATTERY_INDICATOR		GPIO_Pin_4
#define LED_3_BATTERY_INDICATOR         GPIO_Pin_5
#define LED_4_BATTERY_INDICATOR		GPIO_Pin_8
#define LED_POWER_SWITCHER		GPIO_Pin_9

#define HALL_SENSOR_A			GPIO_Pin_2
#define HALL_SENSOR_B			GPIO_Pin_1
#define HALL_SENSOR_C			GPIO_Pin_0

#define BRIDGE_A_HIGH			GPIO_Pin_10
#define BRIDGE_B_HIGH			GPIO_Pin_8
#define BRIDGE_C_HIGH			GPIO_Pin_9
#define BRIDGE_A_LOW			GPIO_Pin_15
#define BRIDGE_B_LOW			GPIO_Pin_13
#define BRIDGE_C_LOW			GPIO_Pin_14

#define BUZZER				GPIO_Pin_3
#define PS_SIGNAL 			GPIO_Pin_4
#define BATTERY_VOLTAGE_SIGNAL 		GPIO_Pin_5
#define MOTOR_CURRENT_SIGNAL 		GPIO_Pin_7


void gpio_init (void);

#endif /* GPIO_H_ */
