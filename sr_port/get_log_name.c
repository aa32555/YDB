/****************************************************************
 *								*
 * Copyright (c) 2001-2022 Fidelity National Information	*
 * Services, Inc. and/or its subsidiaries. All rights reserved.	*
 *								*
 *	This source code contains the intellectual property	*
 *	of its copyright holder(s), and is made available	*
 *	under a license.  If you do not know the terms of	*
 *	the license, please stop and do not read further.	*
 *								*
 ****************************************************************/

#include "mdef.h"

#include "gtm_string.h"

#include "io.h"
#include "mmemory.h"

GBLREF io_log_name *io_root_log_name;

error_def(ERR_INVSTRLEN);

#define LOGNAME_LEN 255

io_log_name *get_log_name(mstr *v, bool insert)
{
	io_log_name	*l, *prev, *new;
	int4		index, stat, v_len;
	unsigned char	buf[LOGNAME_LEN];

	assert(0 != io_root_log_name);
	assert(0 == io_root_log_name->len);
	v_len = v->len;
	if (0 >= v_len)
		return io_root_log_name;
	assert(0 <= (uint4)v_len);
	if (LOGNAME_LEN < v_len)
	{
		rts_error_csa(CSA_ARG(NULL) VARLSTCNT(4) ERR_INVSTRLEN, 2, v_len, LOGNAME_LEN);
		return NULL;
	}
	memcpy(buf, v->addr, v_len);
	for (prev = io_root_log_name, l = prev->next; NULL != l; prev = l, l = l->next)
	{
		if ((NULL != l->iod) && (n_io_dev_types == l->iod->type))
		{
			assert(FALSE);
			continue;	/* skip it on pro */
		}
		stat = memvcmp(l->dollar_io, l->len, buf, v_len);
		if (0 == stat)
			return l;
		if (0 < stat)
			break;
	}
	if (insert == INSERT)
	{
		assert(0 != prev);
		new = (io_log_name *)malloc(sizeof(*new) + v_len);
		memset(new, 0, sizeof(*new) - 1);
		new->len = v_len;
		memcpy(new->dollar_io, buf, v_len);
		new->dollar_io[v_len] = 0;
		prev->next = new;
		new->next = l;
		return new;
	}
	assert(NO_INSERT == insert);
	return 0;
}
