/*
 * Generic Electric Unicycle firmware
 *
 * Copyright (C) Casainho, 2015.
 *
 * Released under the GPL License, Version 3
 */

#ifndef _ADC_H_
#define _ADC_H_

void adc_init (void);
unsigned int adc_get_current_value (void);
unsigned int adc_get_PS_signal_value (void);

#endif
