#!/bin/bash
# Script name: BATS.sh
#
# Project name: Amazing
#
# Author: Kyu Kim, Alan Lu, Donald Wilson
# Date: February 2016
#
# Description: builds and tests AMStartup and amazing
#
# Input arguments:
# '-d': make debug (passed as the first argument)
#				 
# Output: 
# BATS.log, a log file containing:
# - date & time stamp of the beginning and end of the build
# - the hostname and Operating System where the build was run
# - the results of each step of the build
# - the results of running all the tests (name or purpose of test, success/fail, etc.)


module='Amazing Client'
path='./testing/'
logfile='BATS.log'
log=$path$logfile
EXEC='AMStartup'

cd ..

# set up log
printf "Build and Automated Test System (BATS) Log\n" > $log
printf "Module Name: $module\n" >> $log
printf "Host Name: $HOSTNAME\n" >> $log
printf "Host OS: $OSTYPE\n\n" >> $log


# build
printf "Build started at:  $(date)\n\n" >> $log

make clean &>> $log
if [ 0 -eq $? ]; then
	printf "Clean target\n\n" >> $log
else
	printf "Error making clean target\n" >> $log
	printf "Build exited unsuccessfully at $(date)\n" >> $log
	exit 1
fi

if [ "-d" == "$1" ]; then
	printf "make debug\n" >> $log
	make debug &>> $log
else
	printf "make (default)\n" >> $log
	make &>> $log
fi

if [ 0 -eq $? ]; then
	printf "Build finished at: $(date)\n" >> $log
else
	printf "Build exited unsuccessfully at $(date)\n" >> $log
	exit 1
fi

cd bin
# testing format: print name of test; expected results; system output
printf "\nTESTING AMStartup:\n" >> .$log

#
printf "\nTESTING NUMBER OF ARGUMENTS:\n" >> .$log

printf "\nTest: incorrect number of arguments (3)\n" >> .$log
printf "Expected: error message and exit code 1\n" >> .$log
$EXEC 1 1 &>> .$log
printf "Exit code: $?\n" >> .$log

printf "\nTest: incorrect number of arguments (5)\n" >> .$log
printf "Expected: error message and exit code 1\n" >> .$log
$EXEC 1 0 flume.cs.dartmouth.edu foo &>> .$log
printf "Exit code: $?\n" >> .$log

#
printf "\nTESTING VALIDITY OF ARGUMENTS:\n" >> .$log

printf "\nTest: valid --help option, AMStartup\n" >> .$log
printf "Expected: contents of helpfile should be displayed\n" >> .$log
$EXEC --help &>> .$log
printf "Exit code: $?\n" >> .$log

printf "\nTest: invalid --help option, AMStartup\n" >> .$log
printf "Expected: error message and exit code 1\n" >> .$log
$EXEC -help &>> .$log
printf "Exit code: $?\n" >> .$log

printf "\nTest: valid --help option, avatar client\n" >> .$log
printf "Expected: contents of helpfile should be displayed\n" >> .$log
amazing --help &>> .$log
printf "Exit code: $?\n" >> .$log

printf "\nTest: invalid --help option, avatar client\n" >> .$log
printf "Expected: error message and exit code 1\n" >> .$log
amazing -help &>> .$log
printf "Exit code: $?\n" >> .$log


printf "\nTest: incorrect number of avatars (1)\n" >> .$log
printf "Expected: error message and exit code 1\n" >> .$log
$EXEC 1 0 flume.cs.dartmouth.edu &>> .$log
printf "Exit code: $?\n" >> .$log

printf "\nTest: incorrect number of avatars (11)\n" >> .$log
printf "Expected: error message and exit code 1\n" >> .$log
$EXEC 11 0 flume.cs.dartmouth.edu &>> .$log
printf "Exit code: $?\n" >> .$log

printf "\nTest: incorrect difficulty level (-1)\n" >> .$log
printf "Expected: error message and exit code 1\n" >> .$log
$EXEC 2 -1 flume.cs.dartmouth.edu &>> .$log
printf "Exit code: $?\n" >> .$log

printf "\nTest: incorrect difficulty level (10)\n" >> .$log
printf "Expected: error message and exit code 1\n" >> .$log
$EXEC 2 10 flume.cs.dartmouth.edu &>> .$log
printf "Exit code: $?\n" >> .$log

#
printf "\nEnd of Testing\n" >> .$log

