/*
** 2014 Apollo Project
** Author: Primis
** <stddef.h>
*/

#ifndef _STDDEF_H
#define _STDDEF_H

#ifndef NULL
	#define NULL ((void*)0)
#endif

#ifndef _SIZE_T
	#define _SIZE_T
	typedef long unsigned int size_t;
#endif

#ifndef _PTRDIFF_T
	#define _PTRDIFF_T
	typedef long int ptrdiff_t;
#endif

#ifndef _WCHAR_T
	#define _WCHAR_T
	typedef int wchar_t;
#endif

#define offsetof(TYPE,MEMBER) ((size_t) &((TYPE*)0)->MEMBER)
#endif
