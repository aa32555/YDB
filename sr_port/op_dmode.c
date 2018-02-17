/****************************************************************
 *								*
 * Copyright (c) 2001-2019 Fidelity National Information	*
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

#include "gtm_stdio.h"

#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "gdsroot.h"
#include "gdskill.h"
#include "gdsbt.h"
#include "gtm_facility.h"
#include "fileinfo.h"
#include "gdsfhead.h"
#include "gdscc.h"
#include "filestruct.h"
#include "jnl.h"
#include "indir_enum.h"
#include "io.h"
#include "iottdef.h"
#include "iotimer.h"
#include "stack_frame.h"
#include "buddy_list.h"		/* needed for tp.h */
#include "tp.h"
#include "send_msg.h"
#include "gtmmsg.h"		/* for gtm_putmsg() prototype */
#include "op.h"
#include "change_reg.h"
#include "dm_read.h"
#include "tp_change_reg.h"
#include "getzposition.h"
#include "restrict.h"
#include "dm_audit_log.h"
#include "outofband.h"
#ifdef DEBUG
#include "have_crit.h"		/* for the TPNOTACID_CHECK macro */
#endif


#define	DIRECTMODESTR	"DIRECT MODE (any TP RESTART will fail)"

GBLREF	uint4		dollar_trestart;
GBLREF	io_pair		io_curr_device;
GBLREF	io_pair		io_std_device;
GBLREF	stack_frame	*frame_pointer;
GBLREF	unsigned char	*restart_pc;
GBLREF	unsigned char	*restart_ctxt;
GBLREF	volatile int4	outofband;

LITREF	mval		literal_notimeout;

error_def(ERR_NOTPRINCIO);
error_def(ERR_NOPRINCIO);
error_def(ERR_APDLOGFAIL);

void	op_dmode(void)
{
	d_tt_struct		*tt_ptr;
	gd_region		*save_re;
	mval			prompt, dummy, *input_line;
	boolean_t		xec_cmd = TRUE;			/* Determines if command should be
								 * executed (for direct mode auditing purposes)
								 */
	boolean_t		isatty;
	static boolean_t	dmode_intruptd;
	static boolean_t	kill = FALSE;
	static int		loop_cnt = 0;
	static int		old_errno = 0;
	static io_pair		save_device;
	char			*ptr;
	DCL_THREADGBL_ACCESS;

	SETUP_THREADGBL_ACCESS;
	dummy.mvtype = dummy.str.len = 0;
	input_line = push_mval(&dummy);
	if (dmode_intruptd)
	{
		if (io_curr_device.out != save_device.out)
		{
			dec_err(VARLSTCNT(4) ERR_NOTPRINCIO, 2, save_device.out->trans_name->len,
				save_device.out->trans_name->dollar_io);
		}
	} else
	{
		dmode_intruptd = TRUE;
		save_device = io_curr_device;
	}
	io_curr_device = io_std_device;
	if ((TRUE == io_curr_device.in->dollar.zeof) || kill)
		op_zhalt(ERR_NOPRINCIO, FALSE);	/* op_zhalt doesn't restrict this because it's a halt with a return code */
	/* The following code avoids an infinite loop on UNIX systems when there is an error in writing to the principal device
	 * resulting in a call to the condition handler and an eventual return to this location
	 */
	if ((loop_cnt > 0) && (errno == old_errno))
	{	/* Tried and failed 2x to write to principal device */
		kill = TRUE;
		send_msg_csa(NULL, VARLSTCNT(1) ERR_NOPRINCIO);
		rts_error_csa(NULL, VARLSTCNT(1) ERR_NOPRINCIO);
	}
	++loop_cnt;
	old_errno = errno;
	*((INTPTR_T **)&frame_pointer->restart_pc) = (INTPTR_T *)CODE_ADDRESS(call_dm);
	*((INTPTR_T **)&frame_pointer->restart_ctxt) = (INTPTR_T *)GTM_CONTEXT(call_dm);
	loop_cnt = 0;
	old_errno = 0;
	if (tt == io_curr_device.in->type)
		tt_ptr = (d_tt_struct *)io_curr_device.in->dev_sp;
	else
		tt_ptr = NULL;
	if (!tt_ptr || !tt_ptr->mupintr)
		op_wteol(1);
	TPNOTACID_CHECK(DIRECTMODESTR);
	if (io_curr_device.in->type == tt)
	{
		isatty = TRUE;
		iott_flush(io_curr_device.out);	/* finish any pending flush timers (or else we would see a newline) */
		ptr = readline("YDB>");
		input_line->mvtype = MV_STR;
		input_line->str.len = strlen(ptr);
		input_line->str.addr = ptr;
		/* NARSTODO: Maintain $y based on input_line string's length and terminal's width */
		/* NARSTODO: $x can be set to 0 easily */
		/* NARSTODO: Any other terminal characteristics need to be set? Need to look into dm_read */
		/* NARSTODO: Migrate iott_read also to use readline */
		/* NARSTODO: Add READLINE device parameter */
		/* NARSTODO: Ensure outofband events (like jobinterrupt) are handled properly */
		/* NARSTODO: Need to rework code to do a dlopen of libreadline.so that way we honor READLINE devparm
		 *		only if dlopen succeeds and fail otherwise.
		 */
		/* NARSTODO: Add history ability. */
		/* NARSTODO: any other unknowns */
		/* dm_read(input_line); */
		/* If direct mode auditing is enabled, this attempts to send the command to logger */
		if ((AUDIT_ENABLE_DMODE & RESTRICTED(dm_audit_enable)) && !dm_audit_log(input_line, AUDIT_SRC_DMREAD))
		{
			xec_cmd = FALSE;	/* Do not execute command because logging failed */
			send_msg_csa(CSA_ARG(NULL) VARLSTCNT(1) ERR_APDLOGFAIL);
			gtm_putmsg_csa(CSA_ARG(NULL) VARLSTCNT(1) ERR_APDLOGFAIL);
		}
	} else
	{
		isatty = FALSE;
		prompt.mvtype = MV_STR;
		prompt.str = TREF(gtmprompt);
		op_write(&prompt);
		op_read(input_line, (mval *)&literal_notimeout);
		/* Check if auditing for direct mode is enabled. Also check if
		 * auditing for all READ is disabled, so we do not log twice.
		 */
		if ((AUDIT_ENABLE_DMODE & RESTRICTED(dm_audit_enable)) && !(AUDIT_ENABLE_RDMODE & RESTRICTED(dm_audit_enable))
			       	&& !dm_audit_log(input_line, AUDIT_SRC_OPREAD))
		{
			xec_cmd = FALSE;	/* Do not execute command because logging failed */
			send_msg_csa(CSA_ARG(NULL) VARLSTCNT(1) ERR_APDLOGFAIL);
			gtm_putmsg_csa(CSA_ARG(NULL) VARLSTCNT(1) ERR_APDLOGFAIL);
		}
	}
#	ifdef NARSTODO
	if (!isatty)
		op_wteol(1);
#	endif
	io_curr_device = save_device;
	dmode_intruptd = FALSE;
	/* Only executes command when Direct Mode Auditing
	 * is disabled or when sending/logging is successful
	 */
	if (xec_cmd)
		op_commarg(input_line, indir_linetail);
	frame_pointer->type = 0;
#	ifdef NARSTODO
	/* Get job interrupt to work */
	if (isatty && outofband)
		outofband_action(FALSE);
#	endif
}
