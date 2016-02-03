// ---------------------------------------------------------------------------------
//	Copyright 2001, 2002 Georges Menie (www.menie.org)
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
// ---------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------
//	putchar is the only external dependency for this file,
//	if you have a working putchar, just remove the following
//	define. If the function should be called something else,
//	replace outbyte(c) by your own function call.
// ---------------------------------------------------------------------------------

#include "libStdio.h"
#include "usart.h"

#define PutChar(x) usart1_send_char(x)

static void printchar(char **str, int c) {

	if (str) {
		**str = c;
		++(*str);
	}
	else {
		PutChar(c);
	}
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

static int prints(char **out, const char *string, int width, int pad)
{
	register int pc = 0, padchar = ' ';

	if (width > 0) {
		register int len = 0;
		register const char *ptr;
		for (ptr = string; *ptr; ++ptr) ++len;
		if (len >= width) width = 0;
		else width -= len;
		if (pad & PAD_ZERO) {
			padchar = '0';
		}
	}
	if (!(pad & PAD_RIGHT)) {
		for ( ; width > 0; --width) {
			printchar (out, padchar);
			++pc;
		}
	}
	for ( ; *string ; ++string) {
		printchar (out, *string);
		++pc;
	}
	for ( ; width > 0; --width) {
		printchar (out, padchar);
		++pc;
	}

	return pc;
}

/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 12

static int printi(char **out, int i, int b, int sg, int width, int pad, int letbase)
{
	char print_buf[PRINT_BUF_LEN];
	register char *s;
	register int t, neg = 0, pc = 0;
	register unsigned int u = i;

	if (i == 0) {
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return prints (out, print_buf, width, pad);
	}

	if (sg && b == 10 && i < 0) {
		neg = 1;
		u = -i;
	}

	s = print_buf + PRINT_BUF_LEN-1;
	*s = '\0';

	while (u) {
		t = u % b;
		if( t >= 10 ) {
			t += letbase - '0' - 10;
		}
		*--s = t + '0';
		u /= b;
	}

	if (neg) {
		if( width && (pad & PAD_ZERO) ) {
			printchar (out, '-');
			++pc;
			--width;
		}
		else {
			*--s = '-';
		}
	}

	return pc + prints (out, s, width, pad);
}

static int print(char **out, void *varg)
{
	register int width, pad;
	register int pc = 0;
	register char *format = *((char **)varg);
	varg+=sizeof(char*);
	char scr[4];

	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			width = pad = 0;
			if (*format == '\0') break;
			if (*format == '%') goto out;
			if (*format == '-') {
				++format;
				pad = PAD_RIGHT;
			}
			while (*format == '0') {
				++format;
				pad |= PAD_ZERO;
			}
			for ( ; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			if( *format == 's' ) {
				register char *s = *((char **)varg) ;
				pc += prints (out, s?s:"(null)", width, pad);
				varg+=sizeof(char*);
				continue;
			}
			if( *format == 'd' ) {
				pc += printi (out, *(int*)(varg), 10, 1, width, pad, 'a');
				varg+=sizeof(int);
				continue;
			}
			if( *format == 'x' ) {
				pc += printi (out, *(int*)(varg), 16, 0, width, pad, 'a');
				varg+=sizeof(int);
				continue;
			}
			if( *format == 'X' ) {
				pc += printi (out, *(int*)(varg), 16, 0, width, pad, 'A');
				varg+=sizeof(int);
				continue;
			}
			if( *format == 'u' ) {
				pc += printi (out, *(int*)(varg), 10, 0, width, pad, 'a');
				varg+=sizeof(int);
				continue;
			}
			if( *format == 'c' ) {
				/* char are converted to int then pushed on the stack */
				scr[0] = *(char*)(varg);
				scr[1] = '\0';
				pc += prints (out, scr, width, pad);
				varg+=sizeof(int);
				continue;
			}
		}
		else {
		out:
			printchar (out, *format);
			++pc;
		}
	}
	if (out) {
		**out = '\0';
	}
	return pc;
}

/* assuming sizeof(void *) == sizeof(int) */

int printf(const char *format, ...) __attribute__((noinline));
int printf(const char *format, ...)
{
	register void *varg = (void *)(&format);
	return print(0, varg);
}

int sprintf(char *out, const char *format, ...) __attribute__((noinline));
int sprintf(char *out, const char *format, ...)
{
	register void *varg = (void *)(&format);
	return print(&out, varg);
}

int puts( char *str ) {
	while(*str) {
		PutChar(*str);
		str++;
	}
	PutChar('\n');
	return 1;
}

#include <string.h>

// ---------------------------------------------------------------------------------
// Terminal screen cursor control functions
// clear screen of WinH8
// ---------------------------------------------------------------------------------
void clrscr( void ) {
        printf( "\x1b[2J" ) ; /* extra ESC char required for 330 */
                                /* It seems that when 330 is set to */
                                /* 0 wait states, it is too FAST ??????*/
}

// ---------------------------------------------------------------------------------
// Put cursor into column-x and row-y of WinH8
// ---------------------------------------------------------------------------------
void gotoxy(int x, int y) {

    x=(x>150)?150:((x<1)?1:x);
    y=(y>150)?150:((y<1)?1:y);

   printf("\x1b[%d;%dh", y, x);

}

void assert_failed(uint8_t* file, uint32_t line) {
	printf("Assert fail at File %s Line %d", file, (int)line);
	while(1);	// hang here
}
