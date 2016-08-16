/*
 * (c) 2016 Apollo Project Developers
 * <string.h> C standard library for strings.
 */

#ifndef _STRING_H
#define _STRING_H

#ifndef NULL
    #define NULL 0
#endif

#ifndef _SIZE_T
#define _SIZE_T
   typedef unsigned int size_t;
#endif

void *memcpy(void *destination, const void *source, size_t num);
void *memmove(void *destination, const void *source, size_t num);
char *strcpy(char *destination, const char *source);
char *strncpy(char *destination, const char *source, size_t num);
void *memset(void *ptr, int value, size_t num);
size_t strlen(const char *str);
char *strcat(char *destination, const char *source);
char *strncat(char *destination, const char *source, size_t num);
int memcmp(const void *ptr1, const void *ptr2, size_t num);
int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, size_t num);
const void *memchr(const void *ptr, int value, size_t num);
char *strchr(const char *str, int character);
size_t strcspn(const char *str1, const char *str2);
char *strpbrk(const char *str1, const char *str2);
char *strrchr(const char *str, int character);
size_t strspn(const char *str1, const char *str2);
char *strstr(const char *str1, const char *str2);
char *strtok(char *str, const char *delimiters);

#endif /* _STRING_H */
