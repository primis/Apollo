/*
** <sys/callback.h>
** Callbacks are function holders. We can "throw" functions into them
** For dynamic calling during runtime. They are VERY Useful.
*/
#ifndef _SYS_CALLBACK_H
#define _SYS_CALLBACK_H

// Void Return Primatives
typedef void (*void_callback_t)(void);
typedef void (*void_callback_char_t)(char);
typedef void (*void_callback_short_t)(short);
typedef void (*void_callback_int_t)(int);
/// Unsigned Versions
typedef void (*void_callback_char_t)(unsigned char);
typedef void (*void_callback_short_t)(unsigned short);
typedef void (*void_callback_int_t)(unsigned int);

// Char Return Primatives
typedef char (*void_callback_t)(void);
typedef char (*void_callback_char_t)(char);
typedef char (*void_callback_short_t)(short);
typedef char (*void_callback_int_t)(int);
/// Unsigned Versions
typedef char (*void_callback_char_t)(unsigned char);
typedef char (*void_callback_short_t)(unsigned short);
typedef char (*void_callback_int_t)(unsigned int);

// Int Return Primatives
typedef int (*void_callback_t)(void);
typedef int (*void_callback_char_t)(char);
typedef int (*void_callback_short_t)(short);
typedef int (*void_callback_int_t)(int);
/// Unsigned Versions
typedef int (*void_callback_char_t)(unsigned char);
typedef int (*void_callback_short_t)(unsigned short);
typedef int (*void_callback_int_t)(unsigned int);

// Short Return Primatives
typedef short (*void_callback_t)(void);
typedef short (*void_callback_char_t)(char);
typedef short (*void_callback_short_t)(short);
typedef short (*void_callback_int_t)(int);
/// Unsigned Versions
typedef short (*void_callback_char_t)(unsigned char);
typedef short (*void_callback_short_t)(unsigned short);
typedef short (*void_callback_int_t)(unsigned int);

#endif
