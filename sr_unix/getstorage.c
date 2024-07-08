/****************************************************************
 *								*
 * Copyright (c) 2001-2024 Fidelity National Information	*
 * Services, Inc. and/or its subsidiaries. All rights reserved.	*
 *								*
 *	This source code contains the intellectual property	*
 *	of its copyright holder(s), and is made available	*
 *	under a license.  If you do not know the terms of	*
 *	the license, please stop and do not read further.	*
 *								*
 ****************************************************************/

#include "mdef.h"

#include <sys/resource.h>
#include <sys/time.h>
#include <errno.h>
#include "gtm_unistd.h"

#include "getstorage.h"

#define ERRSTR "getrlimit()"

error_def(ERR_SYSCALL);

int4	gtm_getrlimit(void)
{
	struct rlimit	rl;
	int 		save_errno;
	rlim_t		size;

	if (0 != getrlimit(RLIMIT_DATA, &rl))
	{
		save_errno = errno;
		RTS_ERROR_CSA_ABT(NULL, VARLSTCNT(8) ERR_SYSCALL, 5, RTS_ERROR_LITERAL(ERRSTR), CALLFROM, save_errno);
	}
	/* if rlim_cur is unlimited(-1) it's also larger than MAXPOSINT4 */
	size = (MAXPOSINT4 < rl.rlim_cur) ? MAXPOSINT4 : rl.rlim_cur;
	return size;
}

int4	getstorage(void)
{
	struct rlimit	rl;
	int 		save_errno;
	rlim_t		cur_sbrk, size;

	if (0 != getrlimit(RLIMIT_DATA, &rl))
	{
		save_errno = errno;
		RTS_ERROR_CSA_ABT(NULL, VARLSTCNT(8) ERR_SYSCALL, 5, RTS_ERROR_LITERAL(ERRSTR), CALLFROM, save_errno);
	}
#if !defined(__MVS__)
	/* Two step conversion to eliminate warnings */
	cur_sbrk = (rlim_t)sbrk(0);
	if ((-1 == cur_sbrk) && (ENOMEM == errno))
	{	/* sbrk() returns -1 and sets errno to ENOMEM */
		save_errno = errno;
		RTS_ERROR_CSA_ABT(NULL, VARLSTCNT(8) ERR_SYSCALL, 5, RTS_ERROR_LITERAL(ERRSTR), CALLFROM, save_errno);
	}
#else
	cur_sbrk = 0;			/* smw until something better */
#endif
	size = rl.rlim_cur - cur_sbrk;
/* #if !defined(GTM64) && defined(INT8_SUPPORTED) */
	if (MAXPOSINT4 < size)
		size = MAXPOSINT4;
	else if (0 > size)
	{
		assert(FALSE);
		size = 0;
	}
/* Temporarily, all platform return a diminished potential storage value until stack alignment issues on x86_64
 * are fixed allowing floats again or a better fix is made.
 *
 * #elif defined(GTM64)
 * 	if (MAX_LONG_IN_DOUBLE < size)
 * 		size = MAX_LONG_IN_DOUBLE;
 * #endif
 */
	return size;
}
