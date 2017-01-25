/**
 **    File: c/Makefile
 **    Authors:  Sai Susarla, Weiping He, Jerry Fredin, Ibra Fall, Alireza Haghdoost
 **
 ******************************************************************************
 **
 **    Copyright 2012 NetApp, Inc.
 **
 **    Licensed under the Apache License, Version 2.0 (the "License");
 **    you may not use this file except in compliance with the License.
 **    You may obtain a copy of the License at
 **		  
 **    http://www.apache.org/licenses/LICENSE-2.0
 **				 
 **    Unless required by applicable law or agreed to in writing, software
 **    distributed under the License is distributed on an "AS IS" BASIS,
 **    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 **    See the License for the specific language governing permissions and
 **    limitations under the License.
 **
 ******************************************************************************
 **/


MOD = IOLogDump

SRC = ..
#BINDIR = .

PGMS1 = parsedump seqdetect parsedump_alt
PGMS = ${PGMS1} replaytrace
INSTALLED_PGMS = ${PGMS:%=${BINDIR}/%}
COMMON_OBJS = lib${MOD}Schema.o IOLogParsed.o IOLogUtils.o
VPATH := ${SRC}/c:.
CCFLAGS = -g -I${SRC}/c -I.
#CCFLAGS = -g -I${SRC}/c -I. -DAFAP

ifndef BUILD_DIR
all:
	@echo "Error: Invoke make from the top-level directory, not here."
else
all: ${PGMS}
endif

install: all
ifdef BINDIR
	mkdir -p ${BINDIR}
	install ${PGMS} ${BINDIR}
endif

clean: 
	rm -f ${PGMS} *.o lib${MOD}Schema.cc ${MOD}Schema.h

installclean: clean
	rm -f ${INSTALLED_PGMS}

${MOD}Schema.h lib${MOD}Schema.cc: ../genparsers.pl
	../genparsers.pl

${MOD}.h: ${MOD}Schema.h

parsedump.cc IOLogParsed.cc: ${MOD}.h IOLogParsed.h

IOLogTextDump.h: DRecord.h

replaytrace.cc: IOLogTextDump.h

IOLogParsed.cc: IOLogParsed.h

IOLogUtils.cc: IOLogUtils.h

#lib${MOD}Schema.cc: ${MOD}.h ${MOD}Schema.h

replaytrace: % : %.o ${COMMON_OBJS} asyncio.o
	c++ ${CCFLAGS} $^ -lrt -o $@

${PGMS1} : % : %.o ${COMMON_OBJS}
	c++ ${CCFLAGS} $^ -o $@

.cc.o: ${MOD}.h ${MOD}Schema.h
	c++ ${CCFLAGS} -c $< -o $@
