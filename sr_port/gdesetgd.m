;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;								;
; Copyright (c) 2001-2021 Fidelity National Information		;
; Services, Inc. and/or its subsidiaries. All rights reserved.	;
;								;
;	This source code contains the intellectual property	;
;	of its copyright holder(s), and is made available	;
;	under a license.  If you do not know the terms of	;
;	the license, please stop and do not read further.	;
;								;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
gdesetgd:	;implement the verb: SETGD
GDESETGD
	i update,'$$ALL^GDEVERIF zm gdeerr("GDNOTSET") q
	i update,'$$GDEPUT^GDEPUT  zm gdeerr("GDNOTSET") q
	d GDFIND,CREATE^GDEGET:create,LOAD^GDEGET:'create
	q
GDFIND	s file=$zparse(tfile,"",defgldext)
	i file="" s file=$ztrnlnm(tfile) s:file="" file=tfile zm gdeerr("INVGBLDIR"):file:defgld s tfile=defgld
	; 30 millisec is an arbitrarily chosen value yielding a wait that seems sufficient, but not too annoying
	for i=.03:-.001:.0 do  q:'create  h i	; look long enough to ensure not in window of delete/rename from a concurrent edit
	. i $zsearch("qfb")
	. s file=$zsearch($zparse(tfile,"",defgldext))
	. i file="" s file=$zsearch($zparse(tfile,"",defgldext))
	. i file="" s file=$zparse(tfile,"",defgldext),create=1
	. e  s create=0 zm gdeerr("LOADGD"):file
	zm:create gdeerr("GDUSEDEFS"):file
	q
