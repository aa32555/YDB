/****************************************************************
 *								*
 * Copyright 2001 Sanchez Computer Associates, Inc.		*
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

/*	riscv_gtm_registers.h - GT.M arm register usage
 *
 */


#define GTM_REG_FRAME_POINTER	RISCV_REG_S0
#define GTM_REG_FRAME_VAR_PTR	RISCV_REG_S1
#define GTM_REG_FRAME_TMP_PTR	RISCV_REG_T0
#define GTM_REG_LITERAL_BASE	RISCV_REG_T2
#define GTM_REG_XFER_TABLE	RISCV_REG_S2
#define	GTM_REG_PV		RISCV_REG_S3
#define GTM_REG_DOLLAR_TRUTH    RISCV_REG_S4

#define	GTM_REG_R0		RISCV_REG_A0
#define	GTM_REG_R1		RISCV_REG_A1

#define GTM_REG_ACCUM		RISCV_REG_A0
#define GTM_REG_COND_CODE	RISCV_REG_A1
#define GTM_REG_CODEGEN_TEMP	RISCV_REG_T3
#define GTM_REG_CODEGEN_TEMP_1	RISCV_REG_T4
#define GTM_REG_ZERO		RISCV_REG_ZERO

#define CLRL_REG		RISCV_REG_ZERO
#define CALLS_TINT_TEMP_REG	RISCV_REG_S5
#define MOVC3_SRC_REG		RISCV_REG_S5
#define MOVC3_TRG_REG		RISCV_REG_S6
#define MOVL_RETVAL_REG		RISCV_REG_A0
#define MOVL_REG_R1		RISCV_REG_A1
#define CMPL_TEMP_REG		RISCV_REG_T5
