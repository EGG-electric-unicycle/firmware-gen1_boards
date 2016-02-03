/*
 * libStdio.h
 *
 *  Created on: Oct 2, 2011
 *      Author: wss
 */

#ifndef LIBSTDIO_H_
#define LIBSTDIO_H_

#include "libGen.h"

int sprintf(char *out, const char *format, ...) __attribute__((noinline));
int printf(const char *format, ...) __attribute__((noinline));
int puts( char *str );

void clrscr( void );
void gotoxy(int x, int y);

void assert_failed(uint8_t* file, uint32_t line);	// used by assert_param() if enabled

// This function must be provided by other files or library
//void PutChar(unsigned char c);

#endif /* LIBSTDIO_H_ */
