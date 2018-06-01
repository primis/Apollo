#ifndef __STDIO_H
#define __STDIO_H

#if !defined(NULL)              /* Define NULL */
	#define NULL ((void*)0)
#endif

int putchar(int character);
int puts(const char *str);

/* int vsprintf(char *buf, const char *fmt, va_list args);
int vprintf(const char *fmt, va_list args); */
int sprintf(char *buf, const char *fmt, ...);
int printf(const char *fmt, ...);

void debug(unsigned char *str);
#endif

