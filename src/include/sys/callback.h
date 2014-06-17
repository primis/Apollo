/*
** <sys/callback.h>
** Callbacks are function holders. We can "throw" functions into them
** For dynamic calling during runtime. They are VERY Useful.
*/
#ifndef _SYS_CALLBACK_H
#define _SYS_CALLBACK_H

#include <stdarg.h>

// Void Return Primatives
typedef void (*void_callback_t)(void);
typedef void (*void_callback_char_t)(char);
typedef void (*void_callback_short_t)(short);
typedef void (*void_callback_int_t)(int);
/// Unsigned Versions
typedef void (*void_callback_uchar_t)(unsigned char);
typedef void (*void_callback_ushort_t)(unsigned short);
typedef void (*void_callback_uint_t)(unsigned int);
/// Special Unlimited Variable version.
typedef void (*void_callback_arg_t)(unsigned int, ...);

// Char Return Primatives
typedef char (*char_callback_t)(void);
typedef char (*char_callback_char_t)(char);
typedef char (*char_callback_short_t)(short);
typedef char (*char_callback_int_t)(int);
/// Unsigned Versions
typedef char (*char_callback_uchar_t)(unsigned char);
typedef char (*char_callback_ushort_t)(unsigned short);
typedef char (*char_callback_uint_t)(unsigned int);

// Int Return Primatives
typedef int (*int_callback_t)(void);
typedef int (*int_callback_char_t)(char);
typedef int (*int_callback_short_t)(short);
typedef int (*int_callback_int_t)(int);
/// Unsigned Versions
typedef int (*int_callback_uchar_t)(unsigned char);
typedef int (*int_callback_ushort_t)(unsigned short);
typedef int (*int_callback_uint_t)(unsigned int);

// Short Return Primatives
typedef short (*short_callback_t)(void);
typedef short (*short_callback_char_t)(char);
typedef short (*short_callback_short_t)(short);
typedef short (*short_callback_int_t)(int);
/// Unsigned Versions
typedef short (*short_callback_uchar_t)(unsigned char);
typedef short (*short_callback_ushort_t)(unsigned short);
typedef short (*short_callback_uint_t)(unsigned int);

#endif
