/****************************************************************
 *								*
 * Copyright (c) 2001-2017 Fidelity National Information	*
 * Services, Inc. and/or its subsidiaries. All rights reserved.	*
 *								*
 * Copyright (c) 2018-2021 YottaDB LLC and/or its subsidiaries.	*
 * All rights reserved.						*
 *								*
 *	This source code contains the intellectual property	*
 *	of its copyright holder(s), and is made available	*
 *	under a license.  If you do not know the terms of	*
 *	the license, please stop and do not read further.	*
 *								*
 ****************************************************************/

#include "mdef.h"

#include <errno.h>

#include "gtm_stdlib.h"
#include "gtm_stat.h"

#include "gdsroot.h"
#include "gtm_facility.h"
#include "fileinfo.h"
#include "gdsbt.h"
#include "gdsfhead.h"
#include "filestruct.h"
#include "error.h"
#include "change_reg.h"
#include "min_max.h"
#include "gvcst_protos.h"
#include "eintr_wrappers.h"
#include "util.h"
#include "gtmimagename.h"

GBLREF gd_addr		*gd_header;
GBLREF gd_region	*gv_cur_region;

void region_open(void);

error_def (ERR_DBNOREGION);

boolean_t region_init(bool cm_regions)
{
	gd_region		*baseDBreg, *first_nonstatsdb_reg, *reg, *reg_top;
	boolean_t		is_cm, all_files_open;
	sgmnt_addrs		*baseDBcsa;
	node_local_ptr_t	baseDBnl;

	all_files_open = TRUE;
	first_nonstatsdb_reg = NULL;
	reg_top = gd_header->regions + gd_header->n_regions;
	for (gv_cur_region = gd_header->regions; gv_cur_region < reg_top; gv_cur_region++)
	{
		if (gv_cur_region->open)
			continue;
		if (!IS_REG_BG_OR_MM(gv_cur_region))
			continue;
		if (IS_STATSDB_REG(gv_cur_region))
			continue;			/* Bypass statsDB files */
		if (IS_AUTODB_REG(gv_cur_region))
		{	/* This is an AUTODB region. Check if the corresponding db file name exists.
			 * If so, open it. If not, skip this region (do not want to create the db file in this case).
			 * To find out the db file name (after expansion of any '$' usages), we use "dbfilopn()" if needed.
			 * And then check if that file exists.
			 */
			gd_segment	*seg;
			struct stat	stat_buf;
			int		stat_res;

			assert(!gv_cur_region->seg_fname_initialized);
			reg = dbfilopn(gv_cur_region, TRUE);	/* TRUE indicates just update "seg->fname" if needed and
								 * return without opening the db file and/or creating
								 * AUTODB files.
								 */
			UNUSED(reg);
			assert(gv_cur_region->seg_fname_initialized);
			seg = gv_cur_region->dyn.addr;
			assert('\0' == seg->fname[seg->fname_len]);	/* assert it is null terminated */
			STAT_FILE((char *)seg->fname, &stat_buf, stat_res);
			if ((0 != stat_res) && (ENOENT == errno))
			{
				if (IS_DSE_IMAGE)
				{	/* If caller is DSE, indicate to the user that the region open was skipped */
					gtm_putmsg_csa(CSA_ARG(NULL) VARLSTCNT(6)
							ERR_DSESKIPOPEN, 4, REG_LEN_STR(gv_cur_region), DB_LEN_STR(gv_cur_region));
				}
				continue;
			}
		}
		is_cm = reg_cmcheck(gv_cur_region);
		if (!is_cm || cm_regions)
		{
			region_open();
			if (gv_cur_region->open)
			{
				if (NULL == first_nonstatsdb_reg)
					first_nonstatsdb_reg = gv_cur_region;
			} else
				all_files_open = FALSE;
		}
	}
	if (NULL == first_nonstatsdb_reg)
	{
		gv_cur_region = NULL;
		rts_error_csa(CSA_ARG(NULL) VARLSTCNT(1) ERR_DBNOREGION);
	}
	/* Fill in db file name of statsdb regions now that basedb has been opened in above "for" loop */
	for (reg = gd_header->regions; reg < reg_top; reg++)
	{
		if (reg_cmcheck(reg))
			continue;
		if (!IS_STATSDB_REG(reg))
			continue;			/* Bypass statsDB files */
		STATSDBREG_TO_BASEDBREG(reg, baseDBreg);
		if (baseDBreg->open)
		{
			baseDBcsa = &FILE_INFO(baseDBreg)->s_addrs;
			baseDBnl = baseDBcsa->nl;
			COPY_STATSDB_FNAME_INTO_STATSREG(reg, baseDBnl->statsdb_fname, baseDBnl->statsdb_fname_len);
			/* If a statsdb has already been created (by some other process), then we (DSE or LKE are the only
			 * ones that can reach here) will open the statsdb too. Otherwise we will not.
			 */
			assert(IS_DSE_IMAGE || IS_LKE_IMAGE);
			if (baseDBnl->statsdb_created && !reg->open)
			{
				gv_cur_region = reg;
				region_open();
				assert(reg->open);
			}
		}
	}
	gv_cur_region = first_nonstatsdb_reg;
	change_reg();
	return all_files_open;
}

void region_open(void)
{
	DCL_THREADGBL_ACCESS;

	SETUP_THREADGBL_ACCESS;
	ESTABLISH(region_init_ch);
	gv_cur_region->node = -1;
	gv_init_reg(gv_cur_region);
	REVERT;
}
