#################################################################
#								#
# Copyright (c) 2018-2021 YottaDB LLC and/or its subsidiaries.	#
# All rights reserved.						#
#								#
#	This source code contains the intellectual property	#
#	of its copyright holder(s), and is made available	#
#	under a license.  If you do not know the terms of	#
#	the license, please stop and do not read further.	#
#								#
#################################################################
# See README.md for more information about this Dockerfile
# Simple build/running directions are below:
#
# Build:
#   $ docker build -t yottadb/yottadb:latest .
#
# Use with data persistence:
#   $ docker run --rm -e ydb_chset=utf-8 -v `pwd`/ydb-data:/data -ti yottadb/yottadb:latest

ARG OS_VSN=20.04

# Stage 1: YottaDB build image
FROM ubuntu:${OS_VSN} as ydb-release-builder

ARG CMAKE_BUILD_TYPE=Release
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
                    file \
                    cmake \
                    make \
                    gcc \
                    git \
                    tcsh \
                    libconfig-dev \
                    libelf-dev \
                    libgcrypt-dev \
                    libgpg-error-dev \
                    libgpgme11-dev \
                    libicu-dev \
                    libncurses-dev \
                    libssl-dev \
                    zlib1g-dev \
                    && \
    apt-get clean

RUN mkdir /tmp/yottadb-src
ADD CMakeLists.txt /tmp/yottadb-src
# We want to copy the directories themselves, not their contents.
# Unfortunately, there is no way to do this with globs, so we have to specify each directory individually.
# c.f. <https://docs.docker.com/engine/reference/builder/#add>, <https://stackoverflow.com/questions/26504846>

# This list was generated with `ls sr* -d | tr ' ' '\n' | xargs -n1 -i printf 'ADD %s /tmp/yottadb-src/%s\n' {} {}`.
ADD cmake /tmp/yottadb-src/cmake
ADD sr_aarch64 /tmp/yottadb-src/sr_aarch64
ADD sr_armv7l /tmp/yottadb-src/sr_armv7l
ADD sr_i386 /tmp/yottadb-src/sr_i386
ADD sr_linux /tmp/yottadb-src/sr_linux
ADD sr_port /tmp/yottadb-src/sr_port
ADD sr_port_cm /tmp/yottadb-src/sr_port_cm
ADD sr_unix /tmp/yottadb-src/sr_unix
ADD sr_unix_cm /tmp/yottadb-src/sr_unix_cm
ADD sr_unix_gnp /tmp/yottadb-src/sr_unix_gnp
ADD sr_unix_nsb /tmp/yottadb-src/sr_unix_nsb
ADD sr_x86_64 /tmp/yottadb-src/sr_x86_64
ADD sr_x86_regs /tmp/yottadb-src/sr_x86_regs
ADD .git /tmp/yottadb-src/.git

ENV GIT_DIR=/tmp/yottadb-src/.git
RUN mkdir -p /tmp/yottadb-build \
 && cd /tmp/yottadb-build \
 && test -f /tmp/yottadb-src/.yottadb.vsn || \
    grep YDB_ZYRELEASE /tmp/yottadb-src/sr_*/release_name.h \
    | grep -o '\(r[0-9.]*\)' \
    | sort -u \
    > /tmp/yottadb-src/.yottadb.vsn \
 && cmake \
      -D CMAKE_INSTALL_PREFIX:PATH=/tmp \
      -D YDB_INSTALL_DIR:STRING=yottadb-release \
      -D CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
      /tmp/yottadb-src \
 && make -j $(nproc) \
 && make install

# Stage 2: YottaDB release image
FROM ubuntu:${OS_VSN} as ydb-release

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
                    file \
                    binutils \
                    libelf-dev \
                    libicu-dev \
                    locales \
                    pkg-config \
                    wget \
                    vim \
                    && \
    apt-get clean
RUN locale-gen en_US.UTF-8
WORKDIR /data
COPY --from=ydb-release-builder /tmp/yottadb-release /tmp/yottadb-release
RUN cd /tmp/yottadb-release  \
 && pkg-config --modversion icu-io \
      > /tmp/yottadb-release/.icu.vsn \
 && ./ydbinstall \
      --utf8 `cat /tmp/yottadb-release/.icu.vsn` \
      --installdir /opt/yottadb/current \
 && rm -rf /tmp/yottadb-release
ENV gtmdir=/data \
    LANG=en_US.UTF-8 \
    LANGUAGE=en_US:en \
    LC_ALL=en_US.UTF-8
ENTRYPOINT ["/opt/yottadb/current/ydb"]
