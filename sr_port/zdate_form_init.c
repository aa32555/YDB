/****************************************************************
 *								*
 * Copyright (c) 2002-2021 Fidelity National Information	*
 * Services, Inc. and/or its subsidiaries. All rights reserved.	*
 *								*
 * Copyright (c) 2018 YottaDB LLC and/or its subsidiaries.	*
 * All rights reserved.						*
 *								*
 *	This source code contains the intellectual property	*
 *	of its copyright holder(s), and is made available	*
 *	under a license.  If you do not know the terms of	*
 *	the license, please stop and do not read further.	*
 *								*
 ****************************************************************/

#include "mdef.h"

#include "io.h"
#include "iosp.h"
#include "ydb_trans_log_name.h"
#include "startup.h"
#include "stringpool.h"
#include "zdate_form_init.h"
#include "gtm_stdlib.h"

GBLREF spdesc	stringpool;

error_def(ERR_LOGTOOLONG);
error_def(ERR_TRNLOGFAIL);

void zdate_form_init(struct startup_vector *svec)
{
	int4		status;
	mstr		tn;
	char		buf[MAX_TRANS_NAME_LEN];
	DCL_THREADGBL_ACCESS;

	SETUP_THREADGBL_ACCESS;
	TREF(zdate_form) = svec->zdate_form; /* default */
	if (SS_NORMAL != (status = ydb_trans_log_name(YDBENVINDX_ZDATE_FORM, &tn, buf, SIZEOF(buf), IGNORE_ERRORS_FALSE, NULL)))
	{
<<<<<<< HEAD
		assert(SS_NOLOGNAM == status);
		return;
=======
		if (SS_NOLOGNAM == status)
			return;
		else if (SS_LOG2LONG == status)
			RTS_ERROR_CSA_ABT(NULL, VARLSTCNT(5) ERR_LOGTOOLONG, 3, val.len, val.addr, SIZEOF(buf) - 1);
		else
			RTS_ERROR_CSA_ABT(NULL, VARLSTCNT(5) ERR_TRNLOGFAIL, 2, LEN_AND_LIT(ZDATE_FORM), status);
>>>>>>> 451ab477 (GT.M V7.0-000)
	}
	if (0 == tn.len)
		return;
	assert(tn.len < SIZEOF(buf));
	buf[tn.len] = '\0';
	TREF(zdate_form) = (int4)(STRTOL(buf, NULL, 10));
}
