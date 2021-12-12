/****************************************************************
 *								*
 * Copyright 2001, 2013 Fidelity Information Services, Inc	*
 *								*
 * Copyright (c) 2017-2021 YottaDB LLC and/or its subsidiaries.	*
 * All rights reserved.						*
 *								*
 *	This source code contains the intellectual property	*
 *	of its copyright holder(s), and is made available	*
 *	under a license.  If you do not know the terms of	*
 *	the license, please stop and do not read further.	*
 *								*
 ****************************************************************/

/* Define handling of ALL signals. Most are now ignored but nothing should interrupt us
 * without our knowledge and a handler defined.
 */
#include "mdef.h"

#include "gtm_string.h"
#include "gtm_signal.h"
#include "gtm_stdio.h"
#include "gtm_pthread.h"

#include "io.h"
#include "gtmio.h"
#include "continue_handler.h"
#include "sig_init.h"
#include "invocation_mode.h"
#include "gtmimagename.h"
#include "libyottadb_int.h"

#ifdef GTM_PTHREAD
GBLREF	boolean_t		gtm_jvm_process;
#endif
GBLREF	struct sigaction	orig_sig_action[];

/* sig_init() is the signal handling initialization function for regular signal handling setting handlers for ALL signals
 * from 1 to NSIG with the handlers for all signals except the 4 signal handlers specified in the parameters which are set
 * to what is provided. Note that some signal handlers (for ^C, jobinterrupt, and others) are set in other routines. These
 * can be found by searching the code base for "sigaction".
 *
 * Note this routine does the same function (signal handling initialization) for regular signal handling as sig_init_lang_altmain()
 * does for regular alternate signal handling. Any changes here may need to be reflected there.
 */
void sig_init(void (*signal_handler)(), void (*ctrlc_handler)(), void (*suspsig_handler)(), void (*continue_handler)())
{
	struct sigaction 	ignore, null_action, def_action, susp_action, gen_action, ctrlc_action, cont_action;
	int			sig, rc;
        DCL_THREADGBL_ACCESS;

        SETUP_THREADGBL_ACCESS;
	memset(&ignore, 0, SIZEOF(ignore));
	sigemptyset(&ignore.sa_mask);
	/* Initialize handler definitions we deal with. All signals except those setup for SIG_DFL/SIG_IGN are setup
	 * to receive all info since those signals may need to be forwarded.
	 */
	null_action = def_action = ignore;
	ignore.sa_handler = SIG_IGN;
	def_action.sa_handler = SIG_DFL;
	null_action.sa_flags = YDB_SIGACTION_FLAGS;
	susp_action = gen_action = ctrlc_action = cont_action = null_action;
	null_action.sa_sigaction = null_handler;
	susp_action.sa_sigaction = suspsig_handler;
	gen_action.sa_sigaction = signal_handler;
	ctrlc_action.sa_sigaction = ctrlc_handler;
	cont_action.sa_sigaction = continue_handler;
	/* Save the current handler for each signal in orig_sig_action[] array indexed by the signal number */
	for (sig = 1; sig <= (NSIG - 1); sig++)
	{
		sigaction(sig, NULL, &orig_sig_action[sig]);	/* Save original handler */
		switch (sig)
		{
			case SIGHUP:
        			/* Tandem hack:  rather than ignore SIGHUP, we must catch it and do nothing with the signal. This
				 * prevents ioctls on modem/tty devices from hanging when carrier drops during the system call.
        			 */
				sigaction(sig, &null_action, NULL);
				break;
			case SIGCHLD:
				/* Default handling necessary for SIGCHLD signal. CAUTION: consider the affect on JOB (timeout)
				 * implementation before changing this behavior (like defuncts, ECHILD errors, etc.).
				 */
				sigaction(sig, &def_action, NULL);
				break;
			case SIGTSTP:
			case SIGTTIN:
			case SIGTTOU:
				/* These are all signals that suspend a process. */
				if (NULL != suspsig_handler)
					sigaction(sig, &susp_action, NULL);
				else
					sigaction(sig, &ignore, NULL);
				break;
			case SIGINT:
				/* If supplied with a control-C handler, install it now. */
				if (NULL != ctrlc_handler)
					sigaction(sig, &ctrlc_action, NULL);
				break;
			case SIGCONT:
				/* Special handling for SIGCONT. */
				if (NULL != continue_handler)
				{
#					ifndef DISABLE_SIGCONT_PROCESSING
					if (FALSE == TREF(disable_sigcont))
						sigaction(SIGCONT, &cont_action, NULL);
#					else
					TREF(disable_sigcont) = TRUE;
#					endif
				} else
				{
					sigaction(sig, &ignore, NULL);
					TREF(disable_sigcont) = TRUE;
				}
				break;
			case SIGSEGV:
#				ifdef GTM_PTHREAD
				if (gtm_jvm_process)
					break;
#				endif
			case SIGABRT:
#				ifdef GTM_PTHREAD
				if (gtm_jvm_process)
					break;
#				endif
			case SIGBUS:
#			ifdef _AIX
			case SIGDANGER:
#			endif
			case SIGFPE:
#			ifdef __MVS__
			case SIGABND:
#			else
				/* On Linux SIGIOT is commonly same as SIGABRT, so to avoid duplicate cases, check for that. */
#			if !defined(__CYGWIN__) && defined (SIGIOT) && (SIGIOT != SIGABRT)
			case SIGIOT:
#			endif
#			ifndef __linux__
			case SIGEMT:
#			endif
#			endif
			case SIGILL:
			case SIGQUIT:
#			ifndef __linux__
			case SIGSYS:
#			endif
			case SIGTERM:
			case SIGTRAP:
				/* These are all being handled by the generic_signal_handler. */
				sigaction(sig, &gen_action, NULL);
				break;
			case SIGKILL:
			case SIGSTOP:
				/* As SIGKILL and SIGSTOP cannot be caught, we are bypassing them. */
				break;
			default:
				/* If we are in call-in/simpleAPI mode and a non-default handler is installed, leave it
				 * installed. Otherwise, set the signal to be ignored (SIG_IGN). Note this may initially
				 * be true of some signals we setup later (e.g. timers, intrpt, etc) but they'll be
				 * replaced when they get setup and we still have a record of what was there in
				 * orig_sig_action[]. Note we cannot use the IS_SIMPLEAPI_MODE macro here because initialization
				 * (ydb_init()) is not far enough along to have created a stackframe we can check so this check
				 * *always* fails with IS_SIMPLEAPI_MODE. We have to manually check the invocation mode instead.
				 */
				if ((MUMPS_CALLIN & invocation_mode) && (SIG_DFL != (sighandler_t)orig_sig_action[sig].sa_sigaction)
				    && (SIG_IGN != (sighandler_t)orig_sig_action[sig].sa_sigaction))
					break;
				sigaction(sig, &ignore, NULL);
		}
	}
}
