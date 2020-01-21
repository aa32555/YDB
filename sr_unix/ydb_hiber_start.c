/****************************************************************
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

#include "gtmxc_types.h"
#include "error.h"
#include "send_msg.h"
#include "libydberrors.h"
#include "libyottadb_int.h"
#include "libydberrors.h"

#ifdef DEBUG
GBLREF	boolean_t	simpleThreadAPI_active;
#endif

/* Simple YottaDB wrapper for hiber_start() */
int	ydb_hiber_start(unsigned long long sleep_nsec)
{
	boolean_t		error_encountered;
	DCL_THREADGBL_ACCESS;

	SETUP_THREADGBL_ACCESS;
	VERIFY_NON_THREADED_API;	/* clears a global variable "caller_func_is_stapi" set by SimpleThreadAPI caller
					 * so needs to be first invocation after SETUP_THREADGBL_ACCESS to avoid any error
					 * scenarios from not resetting this global variable even though this function returns.
					 */
	LIBYOTTADB_INIT(LYDB_RTN_HIBER_START, (int));	/* Note: macro could return from this function in case of errors */
	assert(0 == TREF(sapi_mstrs_for_gc_indx));	/* Previously unused entries should have been cleared by that
							 * corresponding ydb_*_s() call.
							 */
	ESTABLISH_NORET(ydb_simpleapi_ch, error_encountered);
	if (error_encountered)
	{	/* Some error occurred - just return to the caller ($ZSTATUS is set) */
		REVERT;
		return -(TREF(ydb_error_code));
	}
	ISSUE_TIME2LONG_ERROR_IF_NEEDED(sleep_nsec);
	assert(!simpleThreadAPI_active);	/* or else an INVAPIMODE error would have been issued in VERIFY_NON_THREADED_API */
	assert(MAXPOSINT4 >= (sleep_nsec / NANOSECS_IN_MSEC));	/* Or else a TIME2LONG error would have been issued above */
	hiber_start(sleep_nsec);
	LIBYOTTADB_DONE;
	REVERT;
	return YDB_OK;
}
