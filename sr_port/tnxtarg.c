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
#include "compiler.h"

void tnxtarg(oprtype *a)
{	/*return a reference to the next triple to be produced */
	DCL_THREADGBL_ACCESS;

	SETUP_THREADGBL_ACCESS;
	a->oprclass = TNXT_REF;
	a->oprval.tref = (TREF(curtchain))->exorder.bl;
	return;
}
