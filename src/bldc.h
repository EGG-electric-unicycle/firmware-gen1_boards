/*
 * Generic Electric Unicycle firmware
 *
 * Copyright (C) Casainho, 2015.
 *
 * Released under the GPL License, Version 3
 */

#ifndef _BLDC_H
#define _BLDC_H

void commutation_disable (void);
void commutate (void);
unsigned int get_current_sector (void);

#endif
