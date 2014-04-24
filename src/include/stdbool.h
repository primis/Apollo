/*
** 2014 Apollo Project
** Author: Primis
** <stdbool.h>
*/

#ifndef _STDBOOL_H
#define _STDBOOL_H

#ifndef __cplusplus /* Make sure this isn't c++ */
	#define bool	_Bool
	#define true	1
	#define false	0
#else				/* If it is, use built in c++ operands */
	#define _Bool	bool
	#define bool	bool
	#define false	false
	#define true	true
#endif

#endif
