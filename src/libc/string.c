/*
 * (c) 2016 Apollo Project Developers
 * libc/string.c - functions for <string.h>
 */

#include <string.h>

void *memcpy(void *destination, const void *source, size_t num)
{
    char *sp = (char*) source;
    char *dp = (char*) destination;
    for(; num != 0; num--) {
        *dp++ = *sp++;
    }
    return destination;
}

void *memmove(void *destination, const void *source, size_t num)
{
    char *sp = (char*) source;
    char *dp = (char*) destination;

    // Source -> Destination is not really a move at all
    if(sp==dp) {
        return destination;
    }

    // Backwards copy if source could overrun destination
    if(sp<dp) {
        sp = sp + (num-1);
        dp = dp + (num-1);
        for(; num != 0; num--) {
            *dp-- = *sp--;
        }
    }

    // Foward copy if desintation comes before source
    else {
        for(; num != 0; num--) {
            *dp++ = *sp++;
        }
    }

    return destination;
}

char *strcpy(char *destination, const char *source)
{
    char *dp = destination;
    do {
        *dp++ = *source++;
    } while (*source);
    return destination;
}

char *strncpy(char *destination, const char *source, size_t num)
{
    char *dp = destination;
    for(; (num != 0) && *source; num--) {
            *dp++ = *source++;
    }
    for(; num != 0; num--) {
        *dp = '\0';
    }
    return destination;
}

void *memset(void *ptr, int value, size_t num)
{
    char *dp = (char*) ptr;
    for(; num != 0; num--) {
        *dp++ = value;
    }
    return ptr;
}

size_t strlen(const char *str)
{
    size_t num;
    while(str[num]) {
        num++;
    }
    return num;
}

char *strcat(char *destination, const char *source)
{
    strcpy(destination + strlen(destination), source);
    return destination;
}

char *strncat(char *destination, const char *source, size_t num)
{
    strncpy(destination + strlen(destination), source, num);
    return destination;
}

int memcmp(const void *ptr1, const void *ptr2, size_t num)
{
    const unsigned char *c1 = ptr1;
    const unsigned char *c2 = ptr2;
    while(num--) {
        if(*c1 != *c2){
            return (*c1 - *c2);
        }
        c1++;
        c2++;
    }
    return 0;
}

int strcmp(const char *str1, const char *str2)
{
    for(; *str1 == *str2; str1++, str2++) {
        if(*str1 == 0)
        {
            return 0;
        }
    }
    return *str1 - *str2;
}

int strncmp(const char *str1, const char *str2, size_t num)
{
    for(; *str1 == *str2 && num; str1++, str2++, num--) {
        if(*str1 == 0)
        {
            return 0;
        }
    }
    return *str1 - *str2;
}

const void *memchr(const void *ptr, int value, size_t num)
{
    unsigned char *haystack = (unsigned char *)ptr;
    while(num--) {
        if(*haystack == (unsigned char)value) {
            return haystack;
        }
        haystack++;
    }
    return NULL;
}

char *strchr(const char *str, int character)
{
    return (char *)memchr(str, character, strlen(str));
}

size_t strcspn(const char *str1, const char *str2)
{
    size_t location = 0;
    while(*str1) {
        if(strchr(str2,*str1++)) {
            return location;
        }
        location++;
    }
    return location;
}

char *strpbrk(const char *str1, const char *str2)
{
    for(; *str1; str1++) {
        if(strchr(str2, *str1)) {
            return (char *)str1;
        }
    }
    return NULL;
}

char *strrchr(const char *str, int character)
{
    char *location = NULL;
    do {
        if (*str == (char) character) {
            location = (char *)str;
        }
    } while(*str++);
    return location;
}

size_t strspn(const char *str1, const char *str2)
{
    size_t location = 0;
    while (*str1 && strchr(str2, *str1++)) {
        location++;
    }
    return location;
}

char *strstr(const char *str1, const char *str2)
{
    size_t num = strlen(str2);
    for(; *str1; str1++) {
        if(!memcmp(str1, str2, num)) {
            return (char *)str1;
        }
    }
    return NULL;
}

char *strtok(char *str, const char *delimiters)
{
    static char *ptr = 0;
    if(str) {
        ptr = str;
    } else if(!ptr) {
        return 0;
    }
    str = ptr + strspn(ptr, delimiters);
    ptr = str + strcspn(str, delimiters);
    if(ptr == str) {
        return ptr = 0;
    }
    ptr = *ptr ? *ptr=0, ptr+1 : 0;
    return str;
}

