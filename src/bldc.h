/*
 * Generic Electric Unicycle firmware
 *
 * Copyright (C) Casainho, 2015.
 *
 * Released under the GPL License, Version 3
 */
#include "pwm.h"

#ifndef _BLDC_H_
#define _BLDC_H_

// direction of motor movement
#define RIGHT 		1
#define LEFT 		0

void commutation_disable (void);
void commutate (void);
unsigned int get_current_sector (void);
unsigned int increment_sector (unsigned int sector);
unsigned int decrement_sector (unsigned int sector);
void set_direction (unsigned int direction);
unsigned int get_direction (void);

#endif
