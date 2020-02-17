/****************************************************************
 *								*
 * Copyright (c) 2018-2019 YottaDB LLC and/or its subsidiaries. *
 * All rights reserved.						*
 *								*
 *	This source code contains the intellectual property	*
 *	of its copyright holder(s), and is made available	*
 *	under a license.  If you do not know the terms of	*
 *	the license, please stop and do not read further.	*
 *								*
 ****************************************************************/

#include "mdef.h"

#include "libyottadb_int.h"

GBLREF	boolean_t	caller_func_is_stapi;

/* Routine to drive ydb_zwr2str_s() in a worker thread so YottaDB access is isolated. Note because this drives
 * ydb_zwr2str_s(), we don't do any of the exclusive access checks here. The thread management itself takes care
 * of most of that currently but also the check in LIBYOTTADB_INIT*() macro will happen in ydb_zwr2str_s() still
 * so no need for it here. The one exception to this is that we need to make sure the run time is alive.
 *
 * Parms and return - same as ydb_zwr2str_s() except for the addition of tptoken and errstr.
 */
int ydb_zwr2str_st(uint64_t tptoken, ydb_buffer_t *errstr, const ydb_buffer_t *zwr, ydb_buffer_t *str)
{
	libyottadb_routines	save_active_stapi_rtn;
	ydb_buffer_t		*save_errstr;
	boolean_t		get_lock;
	intptr_t		retval;
	DCL_THREADGBL_ACCESS;

	SETUP_THREADGBL_ACCESS;
	LIBYOTTADB_RUNTIME_CHECK((int), errstr);
	VERIFY_THREADED_API((int), errstr);
	THREADED_API_YDB_ENGINE_LOCK(tptoken, errstr, LYDB_RTN_ZWR2STR, save_active_stapi_rtn, save_errstr, get_lock, retval);
	if (YDB_OK == retval)
	{
		caller_func_is_stapi = TRUE;	/* used to inform below SimpleAPI call that caller is SimpleThreadAPI */
		retval = ydb_zwr2str_s(zwr, str);
		THREADED_API_YDB_ENGINE_UNLOCK(tptoken, errstr, save_active_stapi_rtn, save_errstr, get_lock);
	}
	return (int)retval;
}
