/*
 * (c) 2016 Apollo Project Developers
 * <string.h> C standard library for strings.
 */

#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>

#ifndef NULL
    #define NULL 0
#endif

#ifndef _SIZE_T
#define _SIZE_T
   typedef unsigned int size_t;
#endif

void *memcpy(void *, const void *, size_t);
void *memmove(void *, const void *, size_t);
char *strcpy(char *, const char *);
char *strncpy(char *, const char *, size_t);
void *memset(void *, int, size_t);
size_t strlen(const char *);
char *strcat(char *, const char *);
char *strncat(char *, const char *, size_t);
int memcmp(const void *, const void *, size_t);
int strcmp(const char *, const char *);
int strncmp(const char *, const char *, size_t);
const void *memchr(const void *, int, size_t);
char *strchr(const char *, int);
size_t strcspn(const char *, const char *);
char *strpbrk(const char *, const char *);
char *strrchr(const char *, int);
size_t strspn(const char *, const char *);
char *strstr(const char *, const char *);
char *strtok(char *, const char *);


#endif /* _STRING_H */
