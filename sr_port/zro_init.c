/****************************************************************
 *								*
 * Copyright (c) 2001-2017 Fidelity National Information	*
 * Services, Inc. and/or its subsidiaries. All rights reserved.	*
 *								*
 * Copyright (c) 2018-2022 YottaDB LLC and/or its subsidiaries.	*
 * All rights reserved.						*
 *								*
 *	This source code contains the intellectual property	*
 *	of its copyright holder(s), and is made available	*
 *	under a license.  If you do not know the terms of	*
 *	the license, please stop and do not read further.	*
 *								*
 ****************************************************************/

#include "mdef.h"

#include "gtm_string.h"

#include "iosp.h"
#include "io.h"
#include "zroutines.h"
#include "ydb_trans_log_name.h"
#include "gtm_file_stat.h"

#include "op_fnzsearch.h"
#include "parse_file.h"
#include "gtm_common_defs.h"
#include "op.h"
#include "ydb_getenv.h"

GBLREF boolean_t		is_ydb_chset_utf8;

error_def(ERR_LOGTOOLONG);
error_def(ERR_ERRORSUMMARY);

#define MAX_NUMBER_FILENAMES	(256 * MAX_TRANS_NAME_LEN)

/* At entry into this function, "ydb_dist" env var would have been defined (either by the user before YottaDB
 * process startup OR by "dlopen_libyottadb" through a "setenv" at image startup. Therefore it is okay to use
 * "$ydb_dist" in the literals below. Those will be expanded by "gtm_file_stat" below.
 */
#define	ZROUTINES_DEFAULT1	"$ydb_dist/libyottadbutil.so"
#define	ZROUTINES_DEFAULT1UTF8	"$ydb_dist/utf8/libyottadbutil.so"
#define	ZROUTINES_DEFAULT2	"$ydb_dist"
#define	ZROUTINES_DEFAULT2UTF8  "$ydb_dist/utf8/"

void zro_init(void)
{
	int4		status;
	mstr		val, tn;
	char		buf1[MAX_NUMBER_FILENAMES]; /* buffer to hold translated name */
	boolean_t	is_ydb_env_match;
	uint4		ustatus;
	mstr		def1, def2;

	mval		fstr,ret;
	unsigned char	source_file_string[256];
	char		*faddr;
	unsigned short 	flen;
	plength		plen;
	unsigned char		*p, source_file_name[255];
	unsigned short		source_name_len;
	int i,length,path_len;
	char 			*ydb_dist,*destination;

	DCL_THREADGBL_ACCESS;

	SETUP_THREADGBL_ACCESS;

	fstr.mvtype = MV_STR;
	ydb_dist = ydb_getenv(YDBENVINDX_DIST_ONLY, NULL_SUFFIX, NULL_IS_YDB_ENV_MATCH);
	//printf("%s",ydb_dist);
	//fflush(stdout);
	destination = "plugin/o/*.so";
	path_len = snprintf(NULL,0,"%s/%s",ydb_dist,destination);
	faddr = malloc(path_len+1);
	snprintf(faddr,path_len+1,"%s/%s",ydb_dist,destination);
	//faddr = "/home/pooh/work/gitlab/YDB/build/plugin/o/*.so";
	flen = strlen(faddr);
	//faddr = "/tmp/*.so";
	//faddr = "/usr/library/V977_R201/dbg/plugin/o/*.so";

	//memcpy(source_file_string, faddr, flen);
	//MEMCPY_LIT(&source_file_string[flen], ".so");
	//fstr.str.addr = (char *)source_file_string;
	fstr.str.addr = faddr;
	//fstr.str.len = flen + SIZEOF(".so") - 1;
	fstr.str.len = flen;

	if ((TREF(dollar_zroutines)).addr)
		free((TREF(dollar_zroutines)).addr);
	status = ydb_trans_log_name(YDBENVINDX_ROUTINES, &tn, buf1, SIZEOF(buf1), IGNORE_ERRORS_FALSE, NULL);
	assert((SS_NORMAL == status) || (SS_NOLOGNAM == status));
	if ((0 == tn.len) || (SS_NOLOGNAM == status))
	{	/* "ydb_routines" env var is defined and set to "" OR undefined */
		tn.len = 1;
		tn.addr = buf1;
		MSTR_CONST(ext1, "");
		if (!is_ydb_chset_utf8) /* M Mode */
		{
			MSTR_CONST(def1, ZROUTINES_DEFAULT1);
			if (FILE_PRESENT == gtm_file_stat(&def1, &ext1, NULL, FALSE, &ustatus))
			{	/* "$ydb_dist/libyottadbutil.so" is present. So use it as $zroutines. */
				zsrch_clr(10);
				source_name_len = 0;
				for (i = 0; ; i++)
				{
					plen.p.pint = op_fnzsearch(&fstr, 0, 0, &ret);
					if (!ret.str.len)
					{	
						if (!i)
						{
							dec_err(VARLSTCNT(4) ERR_FILENOTFND, 2, fstr.str.len, fstr.str.addr);
							TREF(dollar_zcstatus) = -ERR_ERRORSUMMARY;
						} else
						{
							tn.addr = tn.addr-tn.len+1;
						}
						break;
					}
					
					memcpy(tn.addr, ret.str.addr, ret.str.len);
					tn.addr[ret.str.len] = ' ';
					tn.addr += ret.str.len + 1;
					tn.len = tn.len + ret.str.len + 1;
					//length = snprintf(NULL,0,"%s ",source_file_name);
					//snprintf(all_file_name,sizeof(all_file_name),"%s ",source_file_name);
					//p = &source_file_name[plen.p.pblk.b_dir];
				}
				//tn.len = ret.str.len;
				//tn.addr = ret.str.addr;
				//tn.len = sizeof(all_file_name);
				//tn.addr = all_file_name;
			} else
			{	/* "$ydb_dist/libyottadbutil.so" is NOT present. So use "$ydb_dist" as $zroutines. */
				//MSTR_CONST(def2, ZROUTINES_DEFAULT2);
				zsrch_clr(10);
				for (i = 0; ; i++)
				{
					plen.p.pint = op_fnzsearch(&fstr, 0, 0, &ret);
					if (!ret.str.len)
					{	
						if (!i)
						{
							dec_err(VARLSTCNT(4) ERR_FILENOTFND, 2, fstr.str.len, fstr.str.addr);
							TREF(dollar_zcstatus) = -ERR_ERRORSUMMARY;
						} else
						{
							// Add libyottadb.so in last place
							tn.addr = tn.addr-tn.len+1;
						}
						break;
					}
					
					memcpy(tn.addr, ret.str.addr, ret.str.len);
					tn.addr[ret.str.len] = ' ';
					tn.addr += ret.str.len + 1;
					tn.len = tn.len + ret.str.len + 1;
					//length = snprintf(NULL,0,"%s ",source_file_name);
					//snprintf(all_file_name,sizeof(all_file_name),"%s ",source_file_name);
					//p = &source_file_name[plen.p.pblk.b_dir];
				}
				//tn.len = ret.str.len;
				//tn.addr = ret.str.addr;
				//tn.len = sizeof(all_file_name);
				//tn.addr = all_file_name;
				//tn.len = def2.len;
				//tn.addr = def2.addr;
			}
		} else /* UTF-8 mode */
		{
			MSTR_CONST(def1, ZROUTINES_DEFAULT1UTF8);
			if (FILE_PRESENT == gtm_file_stat(&def1, &ext1, NULL, FALSE, &ustatus))
			{	/* "$ydb_dist/utf8/libyottadbutil.so" is present. So use it as $zroutines. */
				tn.len = def1.len;
				tn.addr = def1.addr;
			} else
			{	/* Try "$ydb_dist/utf8/" */
				MSTR_CONST(def2, ZROUTINES_DEFAULT2UTF8);
				if (FILE_PRESENT == gtm_file_stat(&def2, &ext1, NULL, FALSE, &ustatus))
				{	/* "$ydb_dist/utf8/" is present. So use it as $zroutines. */
					tn.len = def2.len;
					tn.addr = def2.addr;
				} else
				{       /* "$ydb_dist/utf8/" does not exist. Can't use $ydb_dist since it doesn't have UTF-8 objects. */
					rts_error_csa(CSA_ARG(NULL) VARLSTCNT(1) ERR_UTF8NOTINSTALLED) ;
				}
			}
		}
	}
	free(faddr);
	zro_load(&tn);

}
