/****************************************************************
 *								*
 * Copyright (c) 2001-2019 Fidelity National Information	*
 * Services, Inc. and/or its subsidiaries. All rights reserved.	*
 *								*
 *	This source code contains the intellectual property	*
 *	of its copyright holder(s), and is made available	*
 *	under a license.  If you do not know the terms of	*
 *	the license, please stop and do not read further.	*
 *								*
 ****************************************************************/

#include "mdef.h"
#include "gdsroot.h"
#include "bit_clear.h"

static unsigned char	bit_clear_mask[8] = {127, 191, 223, 239, 247, 251, 253, 254};

uint4 bit_clear (block_id bit, sm_uc_ptr_t base)
{
	uint4		retval;
	sm_uc_ptr_t	ptr;

	ptr = base + (bit / 8);
	retval = (1 << (bit & 7)) & *ptr;
	*ptr &= ~(1 << (bit & 7));
	return (0 != retval);
}
