/****************************************************************
 *								*
 * Copyright (c) 2018 Fidelity National Information		*
 * Services, Inc. and/or its subsidiaries. All rights reserved.	*
 *								*
 * Copyright (c) 2021 YottaDB LLC and/or its subsidiaries.	*
 * All rights reserved.						*
 *								*
 *	This source code contains the intellectual property	*
 *	of its copyright holder(s), and is made available	*
 *	under a license.  If you do not know the terms of	*
 *	the license, please stop and do not read further.	*
 *								*
 ****************************************************************/
#ifndef SR_PORT_MLK_SHRHASH_MOVE_BUCKETS_H_
#define SR_PORT_MLK_SHRHASH_MOVE_BUCKETS_H_

#include "mdef.h"
#include "mlkdef.h"
#include "gdsroot.h"
#include "gdsblk.h"
#include "gdsbt.h"
#include "gdsfhead.h"

#define MLK_SHRHASH_FOUND_NO_BUCKET     MAXINT4 /* special value to indicate out-of-design situation seen */

int mlk_shrhash_find_bucket(mlk_pvtctl_ptr_t pctl, mlk_subhash_val_t hash);

#endif
