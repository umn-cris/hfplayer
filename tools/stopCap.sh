#!/bin/bash
:<<=cut
=pod

=head1 NAME
	stopCap.sh

=head1 SUMMARY
	This script takes two IP addresses as its input and uses the shellCmd
	utility to stop trace and statistic capture.  It can also take a third 
	input as the file to output status to.  If the third input is not
	specified, then no status is requested.

=head1 USAGE
	stopCap.sh [-r] IP1 IP2 [status file]

=head1 VERSION
	2.1

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
#  4/27/2012	Jerry Fredin	Added header and history blocks
#  4/30/2012	Jerry Fredin	Added -r option (v1.2)
#  5/3/2012	Jerry Fredin	Fixed the -r option (v2.0)
#  7/10/2012	Jerry Fredin	Changed tracedump and volstatdump to
#				ioLogger and perfLogger (v2.1)
#				Also swapped IP address and cmd inputs
#				to shellCmd
#				Removed disable commands
REV="2.1"
REV_OPTION='-r'

if [ "X$1" = "X$REV_OPTION" ];
        then 
		echo "You are executing revision $REV of stopCap.sh"
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

REDIR_STRING="$3 "

if [ "X$3" = "X" ];
	then REDIR_STRING="/dev/null"
fi

./shellCmd 'ioLogger,"status"' $1 >> $REDIR_STRING
./shellCmd 'ioLogger,"stop"' $1
./shellCmd 'ioLogger,"volume disable"' $1
./shellCmd 'perfLogger,"status"' $1 >> $REDIR_STRING
./shellCmd 'perfLogger,"stop"' $1
./shellCmd 'perfLogger,"volume disable"' $1
./shellCmd 'ioLogger,"volume disable"' $1
./shellCmd 'ioLogger,"status"' $2 >> $REDIR_STRING
./shellCmd 'ioLogger,"stop"' $2
./shellCmd 'ioLogger,"volume disable"' $2
./shellCmd 'perfLogger,"status"' $2 >> $REDIR_STRING
./shellCmd 'perfLogger,"stop"' $2
./shellCmd 'perfLogger,"volume disable"' $2
exit
