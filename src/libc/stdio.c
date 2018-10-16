/*
 * (c) 2018 Apollo Project developers
 * Stdio.c - Makes things more standard
 */

#include <stdio.h>
#include <string.h>
#include <sys/hal.h>


#if 1
#include <stdarg.h> /* va_list, va_start(), va_arg(), va_end() */
#else
/* homebrew STDARG.H */
#define	STACK_WIDTH	sizeof(int)
#define	TYPE_WIDTH(TYPE) ((sizeof(TYPE) + STACK_WIDTH - 1) & ~(STACK_WIDTH - 1))
#define	va_start(PTR, LASTARG) PTR = (va_list)((char *)&(LASTARG) + TYPE_WIDTH(LASTARG))
#define va_end(PTR)
#define va_arg(PTR, TYPE)	((char *)(PTR) += TYPE_WIDTH(TYPE), *((TYPE *)((char *)(PTR) - TYPE_WIDTH(TYPE))))
typedef void *va_list;

#endif

/* flags used in processing format for *printf */
#define	PR_LJ	0x01	/* left justify */
#define	PR_CA	0x02	/* use A-F instead of a-f for hex */
#define	PR_SG	0x04	/* signed numeric conversion (%d vs. %u) */
#define	PR_32	0x08	/* long (32-bit) numeric conversion */
#define	PR_16	0x10	/* short (16-bit) numeric conversion */
#define	PR_WS	0x20	/* PR_SG set and num was < 0 */
#define	PR_LZ	0x40	/* pad left with '0' instead of ' ' */
#define	PR_FP	0x80	/* pointers are far */
#define	PR_BUFLEN	16

typedef int (*fnptr_t)(unsigned c, void **helper);
int do_printf(const char *fmt, va_list args, fnptr_t fn, void *ptr);
static int vsprintf_help(unsigned c, void **ptr);
int vprintf_help(unsigned c, void **ptr);

static int vsprintf_help(unsigned c, void **ptr)
{
	char *dst;
	dst = *ptr;
	*dst++ = (char)c;
	*ptr = dst;
	return 0 ;
}

int vsprintf(char *buf, const char *fmt, va_list args)
{
	int rv;

	rv = do_printf(fmt, args, vsprintf_help, (void *)buf);
	buf[rv] = '\0';
	return rv;
}

int sprintf(char *buf, const char *fmt, ...)
{
	va_list args;
	int rv;

	va_start(args, fmt);
	rv = vsprintf(buf, fmt, args);
	va_end(args);
	return rv;
}

int vprintf_help(unsigned c, void **ptr)
{
	putchar(c);
	return 0 ;
}

int vprintf(const char *fmt, va_list args)
{
	return do_printf(fmt, args, vprintf_help, NULL);
}

int printf(const char *fmt, ...)
{
	va_list args;
	int rv;

	va_start(args, fmt);
	rv = vprintf(fmt, args);
	va_end(args);
	return rv;
}

int putchar(int c)
{
    char str[2] = "\0";
    str[0] = (char)c;
    write_console(str, 1);
    return 0;
}

int puts(const char *str)
{
    while(*str) {
        putchar((int)*str++);
    }

    putchar('\n');
    return 0;
}


int do_printf(const char *fmt, va_list args, fnptr_t fn, void *ptr)
{
	unsigned flags, actual_wd, count, given_wd;
	unsigned char *where, buf[PR_BUFLEN];
	unsigned char state, radix;
	long num;

	state = flags = count = given_wd = 0;

	for(; *fmt; fmt++) /* Begin scanning format specifier list */
	{
		switch(state)
		{

		case 0: /* STATE 0: AWAITING % */
			if(*fmt != '%')	/* not %... */
			{
				fn(*fmt, &ptr);	/* ...just echo it */
				count++;
				break;
			}
			state++; /* found %, get next char and advance state to check if next char is a flag */
			fmt++;
			/* Falls Through */
		case 1: /* STATE 1: AWAITING FLAGS (%-0) */
			if(*fmt == '%')	/* %% */
			{
				fn(*fmt, &ptr);
				count++;
				state = flags = given_wd = 0;
				break;
			}

			if(*fmt == '-')
			{
				if(flags & PR_LJ)/* %-- is illegal */
					state = flags = given_wd = 0;
				else
					flags |= PR_LJ;
				break;
			}

			state++; /* not a flag char: advance state to check if it's field width */

			if(*fmt == '0') /* check now for '%0...' */
			{
				flags |= PR_LZ;
				fmt++;
			}
			/* Falls Through */
		case 2: /* STATE 2: AWAITING (NUMERIC) FIELD WIDTH */
			if(*fmt >= '0' && *fmt <= '9')
			{
				given_wd = 10 * given_wd +
					(*fmt - '0');
				break;
			}
			state++;
			/* Falls Through */
		case 3: /* STATE 3: AWAITING MODIFIER CHARS (FNlh) */
			if(*fmt == 'F')
			{
				flags |= PR_FP;
				break;
			}
			if(*fmt == 'N')
				break;
			if(*fmt == 'l')
			{
				flags |= PR_32;
				break;
			}
			if(*fmt == 'h')
			{
				flags |= PR_16;
				break;
			}
			state++;
			/* Falls through */
		case 4: /* STATE 4: AWAITING CONVERSION CHARS (Xxpndiuocs) */
			where = buf + PR_BUFLEN - 1;
			*where = '\0';
			switch(*fmt)
			{
			case 'X':
				flags |= PR_CA;
				/* Falls through */
			/* xxx - far pointers (%Fp, %Fn) not yet supported */
			case 'x':
				/* Falls through */
			case 'p':
				/* Falls through */	
			case 'n':
				radix = 16;
				goto DO_NUM;
			case 'd':
				/* Falls through */	
			case 'i':
				flags |= PR_SG;
				/* Falls through */	
			case 'u':
				radix = 10;
				goto DO_NUM;
			case 'o':
				radix = 8;

DO_NUM:				if(flags & PR_32) /* load the value to be printed. l=long=32 bits: */
					num = va_arg(args, unsigned long);

				else if(flags & PR_16) /* h=short=16 bits (signed or unsigned) */
				{
					if(flags & PR_SG)
						num = va_arg(args, int);
					else
						num = va_arg(args, unsigned int);
				}

				else /* no h nor l: sizeof(int) bits (signed or unsigned) */
				{
					if(flags & PR_SG)
						num = va_arg(args, int);
					else
						num = va_arg(args, unsigned int);
				}

				if(flags & PR_SG) /* take care of sign */
				{
					if(num < 0)
					{
						flags |= PR_WS;
						num = -num;
					}
				}

				do /* Convert binary to octal/decimal/hex ASCII */
				{
					unsigned long temp;

					temp = (unsigned long)num % radix;
					where--;
					if(temp < 10)
						*where = temp + '0';
					else if(flags & PR_CA)
						*where = temp - 10 + 'A';
					else
						*where = temp - 10 + 'a';
					num = (unsigned long)num / radix;
				}
				while(num != 0);
				goto EMIT;

			case 'c': /* Disallow pad-left-with-zeroes for %c */
				flags &= ~PR_LZ;
				where--;
				*where = (unsigned char)va_arg(args, unsigned int);
				actual_wd = 1;
				goto EMIT2;     // Holy Crap, a Goto!

			case 's': /* Disallow pad-left-with-zeroes for %s */
				flags &= ~PR_LZ;
				where = va_arg(args, unsigned char *);
EMIT:
				actual_wd = strlen((char*)where);

				if(flags & PR_WS)
					actual_wd++;

				if((flags & (PR_WS | PR_LZ)) ==	(PR_WS | PR_LZ)) /* If we pad left with ZEROES, do the sign now */
				{
					fn('-', &ptr);
					count++;
				}

EMIT2:			if((flags & PR_LJ) == 0)  /* Pad on left with spaces or zeroes (for right justify) */
				{
					while(given_wd > actual_wd)
					{
						fn(flags & PR_LZ ?
							'0' : ' ', &ptr);
						count++;
						given_wd--;
					}
				}

				if((flags & (PR_WS | PR_LZ)) == PR_WS) /* If we pad left with SPACES, do the sign now */
				{
					fn('-', &ptr);
					count++;
				}

				while(*where != '\0') /* Emit string/char/converted number */
				{
					fn(*where++, &ptr);
					count++;
				}

				if(given_wd < actual_wd) /* Pad on right with spaces (for left justify) */
					given_wd = 0;

				else given_wd -= actual_wd;

				for(; given_wd; given_wd--)
				{
					fn(' ', &ptr);
					count++;
				}
				break;

			default:
				break;
			}
			/* Falls Through */
		default:
			state = flags = given_wd = 0;
			break;
		}
	}
	return count;
}

