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
#include "io.h"
#include "iousdef.h"

void ious_wteol(int4 x, io_desc *iod)
{
	assert(iod->state == dev_open);
	((((d_us_struct*)(iod->dev_sp))->disp->wteol))(x, iod);
}
