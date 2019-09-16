/****************************************************************
 *								*
 * Copyright 2001 Sanchez Computer Associates, Inc.		*
 *								*
 * Copyright (c) 2019 YottaDB LLC and/or its subsidiaries.	*
 * All rights reserved.						*
 *								*
 *	This source code contains the intellectual property	*
 *	of its copyright holder(s), and is made available	*
 *	under a license.  If you do not know the terms of	*
 *	the license, please stop and do not read further.	*
 *								*
 ****************************************************************/

#include "mdef.h"
#include "op.h"

GBLREF	int		dollar_truth;
GBLREF	boolean_t	bool_expr_saw_sqlnull;

void op_dt_false(void)
{
	if (bool_expr_saw_sqlnull)
	{
		dollar_truth = FALSE;
		bool_expr_saw_sqlnull = FALSE;
	} else
		dollar_truth = FALSE;
}
