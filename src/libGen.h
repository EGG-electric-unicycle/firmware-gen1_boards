/*
 * libGen.h
 *
 *  Created on: Oct 2, 2011
 *      Author: wss
 */

#ifndef LIBGEN_H_
#define LIBGEN_H_

#include "stm32f10x.h"

//#ifndef __cplusplus
//typedef enum {FALSE = 0, TRUE = !FALSE} bool;
//#endif

#ifndef DI
#define DI			__disable_irq()
#endif

#ifndef EI
#define EI			__enable_irq()
#endif

#endif /* LIBGEN_H_ */
