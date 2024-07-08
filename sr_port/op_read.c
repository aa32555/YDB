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
#include "gtm_iconv.h"
#include "io.h"
#include "iosp.h"
#include "iotimer.h"
#include "iott_setterm.h"
#include "stringpool.h"
#include "op.h"
#include "ebc_xlat.h"
#include "tpnotacid_chk_inline.h"
#include "mvalconv.h"
#include "restrict.h"
#include "dm_audit_log.h"

GBLREF io_pair		io_curr_device;
GBLREF io_desc		*active_device;
GBLREF spdesc		stringpool;

error_def(ERR_TEXT);
error_def(ERR_APDLOGFAIL);

int op_read(mval *v, mval *timeout)
{
	char		*save_ptr, *start_ptr;
	int		stat;
	int4		msec_timeout;
	mval		val;
	size_t		cnt, insize, outsize;
	unsigned char	*temp_ch;
	DCL_THREADGBL_ACCESS;

	SETUP_THREADGBL_ACCESS;
	MV_FORCE_MSTIMEOUT(timeout, &msec_timeout, READTIMESTR);
	active_device = io_curr_device.in;
	v->mvtype = MV_STR;
	v->str.len = 0;
	stat = (io_curr_device.in->disp_ptr->read)(v, msec_timeout);
	if (IS_AT_END_OF_STRINGPOOL(v->str.addr, 0))
		stringpool.free += v->str.len;	/* see UNIX iott_readfl */
	assert(stringpool.free <= stringpool.top);
#	ifdef KEEP_zOS_EBCDIC
	if (DEFAULT_CODE_SET != io_curr_device.in->in_code_set)
	{
		cnt = insize = outsize = v->str.len;
		assert(stringpool.free >= stringpool.base);
		ENSURE_STP_FREE_SPACE(cnt);
		temp_ch = stringpool.free;
		save_ptr = v->str.addr;
		start_ptr = (char *)temp_ch;
		stringpool.free += cnt;
		assert(stringpool.free >= stringpool.base);
		assert(stringpool.free <= stringpool.top);
		ICONVERT(io_curr_device.in->input_conv_cd, (unsigned char **)&v->str.addr, &insize, &temp_ch, &outsize);
		v->str.addr = start_ptr;
	}
#	endif
	active_device = 0;
	/* If direct mode auditing is enabled, attempt to send the command to logger */
	if (IS_MUMPS_IMAGE && (AUDIT_ENABLE_RDMODE & RESTRICTED(dm_audit_enable)) && !dm_audit_log(v, AUDIT_SRC_OPREAD))
	{
		/* Logging has failed so terminate */
		send_msg_csa(CSA_ARG(NULL) VARLSTCNT(1) ERR_APDLOGFAIL);
		RTS_ERROR_CSA_ABT(NULL, VARLSTCNT(1) ERR_APDLOGFAIL);
	}
	return ((NO_M_TIMEOUT != msec_timeout) ? stat : FALSE);
}
