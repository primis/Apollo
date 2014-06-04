/*
** <sys/callback.h>
** Callbacks are function holders. We can "throw" functions into them
** For dynamic calling during runtime. They are VERY Useful.
*/
#ifndef _SYS_CALLBACK_H
#define _SYS_CALLBACK_H

typedef void (*void_callback_t)(void);
typedef void (*void_callback_char_t)(char);
typedef void (*void_callback_short_t)(short);
typedef void (*void_callback_int_t)(int);
typedef void (*void_callback_unsigned_int_t)(unsigned int);

#endif
