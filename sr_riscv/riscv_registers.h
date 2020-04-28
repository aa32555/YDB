/****************************************************************
 *								*
 * Copyright (c) 2018 YottaDB LLC and/or its subsidiaries.	*
 * All rights reserved.						*
 *								*
 * Copyright (c) 2018 Stephen L Johnson. All rights reserved.	*
 *								*
 * Copyright (c) 2020 Western Digital. All rights reserved.	*
 *								*
 *	This source code contains the intellectual property	*
 *	of its copyright holder(s), and is made available	*
 *	under a license.  If you do not know the terms of	*
 *	the license, please stop and do not read further.	*
 *								*
 ****************************************************************/

/*	RISC-V register names  */

#define RISCV_REG_X0		 0
#define RISCV_REG_X1		 1
#define	RISCV_REG_X2		 2
#define	RISCV_REG_X3		 3
#define	RISCV_REG_X4		 4
#define	RISCV_REG_X5		 5
#define	RISCV_REG_X6		 6
#define	RISCV_REG_X7		 7
#define	RISCV_REG_X8		 8
#define	RISCV_REG_X9		 9
#define	RISCV_REG_X10		10
#define	RISCV_REG_X11		11
#define	RISCV_REG_X12		12
#define	RISCV_REG_X13		13
#define	RISCV_REG_X14		14
#define	RISCV_REG_X15		15
#define	RISCV_REG_X16		16
#define	RISCV_REG_X17		17
#define	RISCV_REG_X18		18
#define	RISCV_REG_X19		19
#define	RISCV_REG_X20		20
#define	RISCV_REG_X21		21
#define	RISCV_REG_X22		22
#define	RISCV_REG_X23		23
#define	RISCV_REG_X24		24
#define	RISCV_REG_X25		25
#define	RISCV_REG_X26		26
#define	RISCV_REG_X27		27
#define	RISCV_REG_X28		28
#define	RISCV_REG_X29		29
#define	RISCV_REG_X30		30
#define	RISCV_REG_X31		31

#define RISCV_REG_ZERO		 RISCV_REG_X0
#define RISCV_REG_RA		 RISCV_REG_X1
#define RISCV_REG_SP		 RISCV_REG_X2
#define RISCV_REG_GP		 RISCV_REG_X3
#define RISCV_REG_TP		 RISCV_REG_X4
#define RISCV_REG_T0		 RISCV_REG_X5
#define RISCV_REG_T1		 RISCV_REG_X6
#define RISCV_REG_T2		 RISCV_REG_X7
#define RISCV_REG_S0		 RISCV_REG_X8
#define RISCV_REG_S1		 RISCV_REG_X9
#define RISCV_REG_A0		 RISCV_REG_X10
#define RISCV_REG_A1		 RISCV_REG_X11
#define RISCV_REG_A2		 RISCV_REG_X12
#define RISCV_REG_A3		 RISCV_REG_X13
#define RISCV_REG_A4		 RISCV_REG_X14
#define RISCV_REG_A5		 RISCV_REG_X15
#define RISCV_REG_A6		 RISCV_REG_X16
#define RISCV_REG_A7		 RISCV_REG_X17
#define RISCV_REG_S2		 RISCV_REG_X18
#define RISCV_REG_S3		 RISCV_REG_X19
#define RISCV_REG_S4		 RISCV_REG_X20
#define RISCV_REG_S5		 RISCV_REG_X21
#define RISCV_REG_S6		 RISCV_REG_X22
#define RISCV_REG_S7		 RISCV_REG_X23
#define RISCV_REG_S8		 RISCV_REG_X24
#define RISCV_REG_S9		 RISCV_REG_X25
#define RISCV_REG_S10		 RISCV_REG_X26
#define RISCV_REG_S11		 RISCV_REG_X27
#define RISCV_REG_T3		 RISCV_REG_X28
#define RISCV_REG_T4		 RISCV_REG_X29
#define RISCV_REG_T5		 RISCV_REG_X30
#define RISCV_REG_T6		 RISCV_REG_X31

/*	Number of arguments passed in registers.  */
#define MACHINE_REG_ARGS	 8
