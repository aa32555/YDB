/****************************************************************
 *								*
 * Copyright (c) 2015-2021 Fidelity National Information	*
 * Services, Inc. and/or its subsidiaries. All rights reserved.	*
 *								*
 * Copyright (c) 2024 YottaDB LLC and/or its subsidiaries.	*
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
#include "arit.h"
#include "gtm_time.h"
#include <sys/time.h>

#define DECIMAL_BASE	10	/* stolen from gdsfhead which is silly to include here */
#define USE_RDTSC 1
#define USE_CLOCKMONOTONIC 0

#ifdef __x86_64__
#  include <x86intrin.h>
#endif

LITREF	int4	ten_pwr[];

#if USE_RDTSC
static uint64_t rdtsc(void);

#ifdef __x86_64__
static uint64_t rdtsc(void)
{
	return __rdtsc();
}
#elif defined(__armv6l__) || defined(__armv7l__) || defined(__aarch64__)
// https://stackoverflow.com/questions/77130235/how-to-return-cpu-cycle-count-from-register-on-arm64-in-c-using-asm-apple-m2
static uint64_t rdtsc(void)
{
	uint64_t value;
	asm("isb; mrs %0, CNTVCT_EL0" : "=r"(value));
	return value;
}
#endif
#endif
void op_zycycles(mval *s)
{
	uint64_t	cycle_count;
	uint64_t	tmp_cycle_count;
	int		numdigs;
	int4		pwr;
#if USE_RDTSC
	cycle_count = rdtsc();
	tmp_cycle_count = cycle_count;
#elif USE_CLOCKMONOTONIC
	struct timespec	ts;
	clock_gettime(CLOCK_MONOTONIC_RAW,&ts);
	tmp_cycle_count = cycle_count = (1LL * MICROSECS_IN_SEC * ts.tv_sec) + (ts.tv_nsec / NANOSECS_IN_USEC);
#else
	tmp_cycle_count = cycle_count = 0;
#endif
	for (numdigs = 0; tmp_cycle_count ; numdigs++, tmp_cycle_count /= DECIMAL_BASE)
		;
	if (numdigs <= NUM_DEC_DG_1L)
	{
		s->m[0] = 0;
		s->m[1] = (int4)cycle_count * ten_pwr[NUM_DEC_DG_1L - numdigs];
	} else
	{
		pwr = ten_pwr[numdigs - NUM_DEC_DG_1L];
		s->m[0] = (cycle_count % pwr) * ten_pwr[NUM_DEC_DG_2L - numdigs];
		s->m[1] = cycle_count / pwr;
	}
	s->mvtype = MV_NM;
	s->e = MV_XBIAS + numdigs;
	s->sgn = 0;
	return;
}
