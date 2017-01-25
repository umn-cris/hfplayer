#**
#**     File:  Makefile
#**    Authors:  Sai Susarla, Jerry Fredin, Alireza Haghdoost
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

# The default directory in which to install the scripts and binaries.
# You can override this by specifying 
# 	make INSTALL=<your_own_dir> 
# on the command line.
INSTALL = bin

# There are no configurable items below this line.
# Local build directory. 
export BUILD_DIR = build
BINDIR := $(abspath ${INSTALL})
export BINDIR

# SCRIPTS = $(wildcard *.pl *.pm *.exp *.py *.sh) ${BUILD_DIR}/hfplayer ${BUILD_DIR}/stdDeviation
SCRIPTS = $(wildcard *.pl *.pm *.exp *.py *.sh) ${BUILD_DIR}/hfplayer

all: all_${BUILD_DIR}
	cp -a ${SCRIPTS} ${BINDIR}

stdDeviation : ${BUILD_DIR}/Makefile 
	${MAKE} -C ${BUILD_DIR} stdDeviation

depAnalyser : ${BUILD_DIR}/Makefile 
	${MAKE} -C ${BUILD_DIR} depAnalyser

clean_stddev : ${BUILD_DIR}/Makefile
	${MAKE} -C ${BUILD_DIR} clean


install: all

clean: clean_${BUILD_DIR}

installclean: installclean_${BUILD_DIR}
	-if [ ! ${BINDIR} -ef ${PWD} ]; then \
		cd ${BINDIR} && rm -f ${SCRIPTS}; \
	fi

${BUILD_DIR}/Makefile:
	mkdir -p ${BUILD_DIR} ${BINDIR}
	(cd ${BUILD_DIR}; rm -f Makefile; ln -s ../c/Makefile; touch Makefile)

all_${BUILD_DIR}: ${BUILD_DIR}/Makefile
	${MAKE} -C ${BUILD_DIR} clean install

clean_${BUILD_DIR}:
	if [ -f ${BUILD_DIR}/Makefile ]; then \
		${MAKE} -C ${BUILD_DIR} clean; \
	fi
	rm ${BINDIR}/hfplayer
	rm ${BINDIR}/depAnalyser
	rm ${BINDIR}/stdDeviation

installclean_${BUILD_DIR}: ${BUILD_DIR}/Makefile
	${MAKE} -C ${BUILD_DIR} installclean;
	rm -rf ${BUILD_DIR}
