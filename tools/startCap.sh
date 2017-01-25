#!/bin/bash
:<<=cut
=pod

=head1 NAME
	startCap.sh

=head1 DESCRIPTION
	This script takes two IP addresses as its input and uses the shellCmd
	utility to start trace and statistic capture.  It can also be executed with the
	-r option to print the revsion of the script.

=head1 VERSION
	version 2.0

=head1 CREATED BY
	Jerry Fredin

=head1 LAST MODIFIED BY
	Jerry Fredin

=cut
#
#	Copyright 2012, NetApp, Inc. All rights reserved.
#
#  Change History
#
#  5/2/2012	Jerry Fredin	Added header and history blocks
#				Also added -r option (v1.1)
#  5/3/2012	Jerry Fredin	Fixed the -r option (v1.2)
#
#  6/28/2012	Jerry Fredin	Added a check for two input parameters
#				Changed tracedump to ioLogger
#				Changed volstatdump to perfLogger
#				Swapped IP and cmd input to shellCmd
#				Removed enable and disable commands
#				Changed revision to 2.0
#

REV="2.0"
REV_OPTION='-r'

if [ "X$1" = "X$REV_OPTION" ];
	then 
		echo "You are executing revision $REV of startCap.sh"
	exit
fi

if [ "X$1" = "X" ];
	then
		echo "You must specify the IP addresses on the command line."
		exit
fi

if [ "X$2" = "X" ];
	then
		echo "You must specify the IP addresses on the command line."
		exit
fi

#Start controllers specified by $1 and $2
./shellCmd 'ioLogger,"volume enable TraceVol-A"' $1
./shellCmd 'ioLogger,"reset"' $1
./shellCmd 'ioLogger,"start"' $1
./shellCmd 'perfLogger,"volume enable StatVol-A"' $1
./shellCmd 'perfLogger,"reset"' $1
./shellCmd 'perfLogger,"start"' $1
./shellCmd 'ioLogger,"volume enable TraceVol-B"' $2
./shellCmd 'ioLogger,"reset"' $2
./shellCmd 'ioLogger,"start"' $2
./shellCmd 'perfLogger,"volume enable StatVol-B"' $2
./shellCmd 'perfLogger,"reset"' $2
./shellCmd 'perfLogger,"start"' $2
exit

