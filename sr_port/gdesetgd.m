;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;								;
; Copyright 2001 Sanchez Computer Associates, Inc.		;
;								;
; Copyright (c) 2018-2021 YottaDB LLC and/or its subsidiaries.	;
; All rights reserved.						;
;								;
;	This source code contains the intellectual property	;
;	of its copyright holder(s), and is made available	;
;	under a license.  If you do not know the terms of	;
;	the license, please stop and do not read further.	;
;								;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
gdesetgd:	;implement the verb: SETGD
GDESETGD
	i update,'$$ALL^GDEVERIF d message^GDE(gdeerr("GDNOTSET"),"""""") q
	i update,'$$GDEPUT^GDEPUT  d message^GDE(gdeerr("GDNOTSET"),"""""") q
	d GDFIND,CREATE^GDEGET:create,LOAD^GDEGET:'create
	q
GDFIND	set file=$zparse(tfile,"",defgldext)
	if file="" do
	. set file=$ztrnlnm(tfile)
	. set:file="" file=tfile
	. if tfile'=defgld do
	. . do message^GDE(gdeerr("INVGBLDIR"),$zwrite(file)_":"_$zwrite(defgld))
	. . set tfile=defgld
	. else  do
	. . do message^GDE(gdeerr("INVGBLDIR2"),$zwrite(file))
	. . zgoto gdeEntryState("zlevel")	; Return to GDE caller and/or exit
	set file=$zsearch($zparse(tfile,"",defgldext))
	if file="" set file=$zsearch($zparse(tfile,"",defgldext))
	if file="" set file=$zparse(tfile,"",defgldext),create=1 d message^GDE(gdeerr("GDUSEDEFS"),$zwrite(file))
	else  set create=0 do message^GDE(gdeerr("LOADGD"),$zwrite(file))
	quit
