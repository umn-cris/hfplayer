#!/bin/bash
#**
#**     File:  linuxfixes.sh
#**    Authors:  Jerry Fredin
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
if [ "$#" -ne "1"] ; then
	echo "Error, please put SD device name as an input"
	exit 100
fi

SD=$1
echo 2 > /sys/block/$SD/queue/nomerges
echo noop > /sys/block/$SD/queue/scheduler
/etc/init.d/udev stop
/etc/init.d/multipath-tools stop

