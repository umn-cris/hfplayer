#**
#**     File:  build/Makefile
#**    Authors:  Sai Susarla, Jerry Fredin
#**
#******************************************************************************
#**
#**    Copyright 2012 NetApp, Inc.
#**
#**    Licensed under the Apache License, Version 2.0 (the "License");
#**    you may not use this file except in compliance with the License.
#**    You may obtain a copy of the License at
#**		  
#**    http://www.apache.org/licenses/LICENSE-2.0
#**				 
#**    Unless required by applicable law or agreed to in writing, software
#**    distributed under the License is distributed on an "AS IS" BASIS,
#**    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#**    See the License for the specific language governing permissions and
#**    limitations under the License.
#**
#******************************************************************************
#**
MOD = IOLogDump

SRC = ../c
BINDIR = ../bin

#PGMS1 = parsedump seqdetect parsedump_alt
#PGMS = ${PGMS1} replaytrace
PGMS = hfplayer stdDeviation depAnalyser
INSTALLED_PGMS = ${PGMS:%=${BINDIR}/%}
COMMON_OBJS = lib${MOD}Schema.o HFParsed.o HFPlayerUtils.o
DEP_OBJS = lib${MOD}Schema.o HFParsed.o
VPATH := ${SRC}:.
#CCFLAGS = -g -I${SRC} -I.  -Wall -DPREFETCH -DLOADCTRL -std=c++0x -DDEBUG_TIMING
#CCFLAGS = -g -I${SRC} -I.  -Wall -DPREFETCH -DLOADCTRL -std=c++0x
CCFLAGS = -g -I${SRC} -I.  -Wall -DLOADCTRL -std=c++0x
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
#	rm -f ${PGMS} *.o lib${MOD}Schema.cc ${MOD}Schema.h
	rm -f ${PGMS} *.o

installclean: clean
	rm -f ${INSTALLED_PGMS}

#${MOD}Schema.h lib${MOD}Schema.cc: ../genparsers.pl
#${MOD}Schema.h: ../genparsers.pl
#	../genparsers.pl

${MOD}.h: ${MOD}Schema.h

#parsedump.cc HFParsed.cc: ${MOD}.h HFParsed.h

IOLogTextDump.h: DRecord.h

hfplayer.cc: IOLogTextDump.h

HFParsed.cc: HFParsed.h

HFPlayerUtils.cc: HFPlayerUtils.h

stdDeviation.cc: TextDataSet.h

depAnalyser.cc: TextDataSet.h

lib${MOD}Schema.cc: ${MOD}.h ${MOD}Schema.h

hfplayer: % : %.o ${COMMON_OBJS} hfcore.o
	c++ ${CCFLAGS} $^  -o $@ -laio -lrt -lpthread

stdDeviation: % : %.o ${COMMON_OBJS} stdDeviation.o 
	c++ ${CCFLAGS} $^  -o $@
	cp stdDeviation ${BINDIR}


depAnalyser: % : %.o ${COMMON_OBJS}  depAnalyser.o 
	c++ ${CCFLAGS}  $^  -o $@ 
	cp depAnalyser ${BINDIR}
	cp depAnalyser ${SRC}


${PGMS1} : % : %.o ${COMMON_OBJS}
	c++ ${CCFLAGS} $^ -o $@

.cc.o: ${MOD}.h ${MOD}Schema.h
	c++ ${CCFLAGS} -c ${SRC}/$< -o $@
