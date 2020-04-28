/****************************************************************
 *								*
 * Copyright 2003, 2009 Fidelity Information Services, Inc	*
 *								*
 * Copyright (c) 2018-2020 YottaDB LLC and/or its subsidiaries.	*
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
#ifndef EMIT_CODE_SP_INCLUDED
#define EMIT_CODE_SP_INCLUDED

#include "gtm_stdlib.h"		/* for "abs" prototype */

#include "riscv_registers.h"
#include "riscv_gtm_registers.h"

void	emit_base_offset_load(int base, int offset);
void	emit_base_offset_addr(int base, int offset);
int	encode_immed12(int offset);

#ifdef DEBUG
void    format_machine_inst(void);
void	fmt_ains(void);
void	fmt_brdisp(void);
void	fmt_brdispcond(void);
void	fmt_rd(int size);
void	fmt_rd_rn_shift_immr(int size);
void	fmt_rt_rt2_rn_shift_imm7(int size);
void	fmt_rd_rn_imm12(int size);
void	fmt_rd_shift_imm12(int size);
void	fmt_rd_raw_imm16(int size);
void	fmt_rd_raw_imm16_inv(int size);
void	fmt_rd_rm(int size);
void	fmt_rd_rn(int size);
void	fmt_rd_rn_rm(int size);
void	fmt_rd_rn_rm_sxtw(void);
void	fmt_reg(int reg, int size, int z_flag);
void	fmt_rm(int size);
void	fmt_rn(int size);
void	fmt_rn_rm(int size);
void	fmt_rn_raw_imm12(int size);
void	fmt_rn_shift_imm12(int size);
void	fmt_rt(int size, int z_flag);
void	fmt_rt2(int size);
void	fmt_rt_rn_raw_imm12(int size, int mult);
void	tab_to_column(int col);
#endif

#define INST_SIZE (int)SIZEOF(uint4)
#define BRANCH_OFFSET_FROM_IDX(idx_start, idx_end) (idx_end - (idx_start + 1))
#define MAX_BRANCH_CODEGEN_SIZE 32  /* The length in bytes, of the longest form of branch instruction sequence */

#define MAX_12BIT			0xfff
#define MAX_16BIT			0xffff
#define STACK_ARG_OFFSET(indx)		(8 * (indx))
#define MACHINE_FIRST_ARG_REG		RISCV_REG_A0

#define EMIT_BASE_OFFSET_ADDR(base, offset)		emit_base_offset_addr(base, offset)
#define EMIT_BASE_OFFSET_LOAD(base, offset)		emit_base_offset_load(base, offset)
#define EMIT_BASE_OFFSET_EITHER(base, offset, inst)										\
	((inst == GENERIC_OPCODE_LDA) ? emit_base_offset_addr(base, offset) : emit_base_offset_load(base, offset))

/* Register usage in some of the code generation expansions */
#define GET_ARG_REG(indx)		(RISCV_REG_A0 + (indx))

/* Define the macros for the instructions to be generated.. */

#define LONG_JUMP_OFFSET		(0x7ffffffc)	/* should be large enough to force the long jump instruction sequence */
#define MAX_OFFSET 			0xffffffff
#define EMIT_JMP_ADJUST_BRANCH_OFFSET


/*
 * GT.M on AIX and SPARC is 64bit
 * By default the loads/stores use ldd/std(load double),
 * but if the value being dealt with is a word, the
 * opcode in generic_inst is changed to ldw/stw
 * On other platforms, it is defined to null
 */
#define REVERT_GENERICINST_TO_WORD(inst)

#endif
