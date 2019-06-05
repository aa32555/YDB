#################################################################
#								#
# Copyright (c) 2013-2017 Fidelity National Information		#
# Services, Inc. and/or its subsidiaries. All rights reserved.	#
#								#
# Copyright (c) 2017-2018 YottaDB LLC. and/or its subsidiaries.	#
# All rights reserved.						#
#								#
# Copyright (c) 2017-2018 Stephen L Johnson.			#
# All rights reserved.						#
#								#
#	This source code contains the intellectual property	#
#	of its copyright holder(s), and is made available	#
#	under a license.  If you do not know the terms of	#
#	the license, please stop and do not read further.	#
#								#
#################################################################

set(srdir "sr_darwin")
set(arch "x86_64")
set(bits 64)

set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -I${YDB_SOURCE_DIR}/sr_port")

set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -include ${YDB_SOURCE_DIR}/sr_port/ydbmerrors.h")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -x assembler-with-cpp")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -Wa,-defsym,__APPLE__=1")
# Platform directories
list(APPEND gt_src_list sr_darwin gt_src_list sr_x86_64 sr_x86_regs)
set(gen_xfer_desc 1)

# Assembler
set(CMAKE_INCLUDE_FLAG_ASM "-Wa,-I")
# For Darwin, we need to change the assembly symbols to start with _.
list(APPEND CMAKE_ASM_COMPILE_OBJECT "objconv -nu+ -v0 <OBJECT> <OBJECT>.rename")
list(APPEND CMAKE_ASM_COMPILE_OBJECT "mv <OBJECT>.rename <OBJECT>")

# Compiler
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsigned-char -Wmissing-prototypes -Wreturn-type -Wpointer-sign")
# Add flags for warnings that we want and don't want.
# First enable Wall. That will include a lot of warnings. In them, disable a few. Below is a comment from sr_linux/gtm_env_sp.csh
# on why these warnings specifically are disabled.
#	The following warnings would be desirable, but together can result in megabytes of warning messages. We
#	should look into how hard they would be to clean up. It is possible that some header changes could
#	reduce a large number of these.
#		set desired_warnings = ( $desired_warnings -Wconversion -Wsign-compare )
#	We should also look into how hard these would be to restore. Some of the warnings come from generated
#	code and macro use, making them harder to deal with.
# Note: -Wimplicit not explicitly mentioned since it is enabled by Wall
# Note: -Wuninitialized not explicitly mentioned since it is enabled by Wall
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-unused-result -Wno-parentheses -Wno-unused-value -Wno-unused-variable")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-char-subscripts")

#TODO: Warnings in obj_filesp
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wvla")

# Below is an optimization flag related description copied from sr_linux/gtm_env_sp.csh
#	-fno-defer-pop to prevent problems with assembly/generated code with optimization
#	-fno-strict-aliasing since we don't comply with the rules
#	-ffloat-store for consistent results avoiding rounding differences
#	-fno-omit-frame-pointer so %rbp always gets set up (required by caller_id()). Default changed in gcc 4.6.
# All these are needed only in case of pro builds (if compiler optimization if turned on).
# But they are no-ops in case of a dbg build when optimization is turned off so we include them in all cmake builds.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-strict-aliasing -fno-omit-frame-pointer")

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	# Newer versions of Linux by default include -fstack-protector in gcc. This causes the build to slightly bloat
	# in size and have a runtime overhead (as high as 5% extra CPU cost in our experiments). So keep that option
	# enabled only for DEBUG builds of YottaDB.
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstack-protector-all")
else()
	# For "Release" or "RelWithDebInfo" type of builds, keep this option disabled for performance reasons
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-stack-protector")
endif()

# On ARM Linux, gcc by default does not include -funwind-tables whereas it does on x86_64 Linux.
# This is needed to get backtrace() (used by caller_id.c etc.) working correctly.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -funwind-tables")

add_definitions(
  -D_GNU_SOURCE
  -D_FILE_OFFSET_BITS=64
  -D_XOPEN_SOURCE=600
  -D_LARGEFILE64_SOURCE
  -D_DARWIN_C_SOURCE
  )

# Locate external packages
find_package(Curses REQUIRED) # FindCurses.cmake
include_directories(${CURSES_INCLUDE_PATH})

find_package(Zlib REQUIRED)   # FindZLIB.cmake
include_directories(${ZLIB_INCLUDE_DIRS})

# default homebrew location for openssl includes
include_directories("/usr/local/opt/openssl/include")

find_path(LIBICU_INCLUDE_PATH NAMES uchar.h unicode/uchar.h HINTS /usr/local/opt/icu4c/include)
include_directories(${LIBICU_INCLUDE_PATH})

set(GTM_SET_ICU_VERSION 0 CACHE BOOL "Unless you want ICU from MacPorts/other avoid setting gtm_icu_version to get Apple's undocumented ICU library")

set(CMAKE_MACOSX_RPATH 1)

set(CMAKE_C_CREATE_STATIC_LIBRARY
  "libtool -static <OBJECTS> -o <TARGET>"
  )

# Linker
set(ydb_link  "-Wl,-U,gtm_filename_to_id -Wl,-U,ydb_zstatus -Wl,-exported_symbols_list,\"${YDB_BINARY_DIR}/ydbexe_symbols.export\"")
set(gtm_dep   "${YDB_BINARY_DIR}/ydbexe_symbols.export")

set(libyottadb_link "-Wl,-U,ydb_ci -Wl,-U,gtm_filename_to_id -Wl,-U,gtm_is_main_thread")
set(libyottadb_link "${libyottadb_link} -Wl,-U,accumulate -Wl,-U,is_big_endian -Wl,-U,to_ulong")
set(libyottadb_link "${libyottadb_link} -Wl,-U,gtm_ci")
set(libyottadb_link "${libyottadb_link} -Wl,-U,gtm_cip")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_call_variadic_plist_func")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_child_init")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_ci_t")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_cip")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_cip_t")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_ci_tab_open")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_ci_tab_open_t")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_ci_tab_switch")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_ci_tab_switch_t")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_data_s")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_data_st")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_delete_excl_s")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_delete_excl_st")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_delete_s")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_delete_st")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_file_id_free")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_file_id_free_t")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_file_is_identical")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_file_is_identical_t")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_file_name_to_id")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_file_name_to_id_t")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_fork_n_core")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_free")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_get_s")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_get_st")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_hiber_start")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_hiber_start_wait_any")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_incr_s")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_incr_st")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_lock_decr_s")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_lock_decr_st")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_lock_incr_s")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_lock_incr_st")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_lock_s")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_lock_st")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_malloc")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_message")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_message_t")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_node_next_s")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_node_next_st")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_node_previous_s")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_node_previous_st")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_set_s")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_set_st")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_stdout_stderr_adjust")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_stdout_stderr_adjust_t")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_str2zwr_s")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_str2zwr_st")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_subscript_next_s")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_subscript_next_st")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_subscript_previous_s")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_subscript_previous_st")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_thread_is_main")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_timer_cancel")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_timer_cancel_t")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_timer_start")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_timer_start_t")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_tp_s")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_tp_st")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_zwr2str_s")
set(libyottadb_link "${libyottadb_link} -Wl,-U,ydb_zwr2str_st")
set(libyottadb_link "${libyottadb_link} -Wl,-exported_symbols_list,\"${YDB_BINARY_DIR}/yottadb_symbols.export\"")
set(libyottadb_dep  "${YDB_BINARY_DIR}/ydbexe_symbols.export")

set(libsyslibs "-lncurses -lm -ldl -lc -lpthread")
