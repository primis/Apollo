/*
** <string.h>
**
*/

#ifndef _STRING_H
#define _STRING_H

#ifndef NULL
	#define NULL ((void*)0)
#endif

#include <stddef.h>

void *memcpy(void*, const void*, size_t);
void *memset(void *dst, int val, size_t len);
int memcmp(const void*, const void*, size_t);
char *strchr(const char*, int);
size_t strspn(const char*, const char*);
size_t strcspn(const char*, const char*);
char *strtok(char*, const char*);
char *strstr(const char*, const char*);
size_t strlen(const char*);
int strcmp(const char*, const char*);
int strncmp(const char*, const char*, size_t);
char *strcpy(char*, const char*);
char *strcat(char*, const char*);


#endif
