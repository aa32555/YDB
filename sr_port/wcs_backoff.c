/****************************************************************
 *								*
 * Copyright 2001, 2012 Fidelity Information Services, Inc	*
 *								*
 * Copyright (c) 2018-2020 YottaDB LLC and/or its subsidiaries.	*
 * All rights reserved.						*
 *								*
 *	This source code contains the intellectual property	*
 *	of its copyright holder(s), and is made available	*
 *	under a license.  If you do not know the terms of	*
 *	the license, please stop and do not read further.	*
 *								*
 ****************************************************************/

#include "mdef.h"
#include "sleep_cnt.h"

#ifdef VMS
# include <lib$routines.h>
#endif
#include "gtm_stdlib.h"
#include "gt_timer.h"
#ifdef UNIX
# include "random.h"
#endif
#include "wcs_backoff.h"

GBLREF uint4 process_id;

void wcs_backoff(unsigned int sleepfactor)
{
	/* wcs_backoff provides a layer over hiber_start that produces a [pseudo] random sleep varying
	 * to a maximum sleep time it is intended to be used in as part of a contention backoff
	 * where the argument is the attempt count. If the counter starts at 0, the invocation would
	 * typically be:
	 *   if (count) wcs_backoff(count);
	 */

	static int4	seed = 0;
	uint8		sleep_ns;

	assert(sleepfactor);
	if (0 == sleepfactor)
		return;
	if (sleepfactor > MAXSLPTIME)
		sleepfactor = MAXSLPTIME;
	if (0 == seed)
	{
		init_rand_table();
		seed = 1;
	}
	sleep_ns = ((uint8)(get_rand_from_table() % sleepfactor) * NANOSECS_IN_MSEC);
	if (0 == sleep_ns)
		return;				/* We have no wait this time */
	if ((uint8)NANOSECS_IN_SEC > sleep_ns)			/* Use simpler sleep for shorties */
	{
		SHORT_SLEEP(sleep_ns / NANOSECS_IN_MSEC);
	}
	else
		hiber_start(sleep_ns);		/* Longer sleeps use brute force */
	return;
}
