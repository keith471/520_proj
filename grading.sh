#!/bin/bash

# 1. Build the compiler.
#
# You *MUST* provide a build.sh script in the root directory
# to build your compiler. If you do not produce a binary
# (i.e. something like python), then the build.sh script
# will be empty.
# A sample build.sh file using Makefile has been provided.

echo "*****************************"
echo "  Building compiler"
echo "*****************************"

if [ ! -f build.sh ]
then
	echo "ERROR: Missing build.sh script"
	exit
fi

./build.sh

# 2. Run script checking
#
# You *MUST* provide a run.sh script in the root directory
# to run your compiler. The run.sh script must take a single
# argument, the filename, and pass the contents to your compiler.
# A sample run.sh file using rediction has been provided.
 
if [ ! -f run.sh ]
then
	echo "ERROR: Missing run.sh script"
	exit
fi

# 2. Run all valid programs
#
# For valid programs, your compiler *MUST*
#   (a) output only: VALID
#   (b) exit with status code 0
# A log of the output is written to valid.log

VALID_DIR_SCAN=`find "grading/valid/scanner/" -type f \( -name "*.go" ! -name "*.pretty.go" \)`
VALID_DIR_PARSE=`find "grading/valid/parser/" -type f \( -name "*.go" ! -name "*.pretty.go" \)`
 
echo
echo "*****************************"
echo "  Valid programs"
echo "*****************************"

if [ -f valid_scan.log ]
then
	rm valid_scan.log
fi
if [ -f valid_parse.log ]
then
	rm valid_parse.log
fi

VALID_SCAN=0
VALID_SCAN_CORRECT=0
for PROG in $VALID_DIR_SCAN
do
	((VALID_SCAN++))

	echo -n "$PROG: " | tee -a valid_scan.log
	./run.sh $PROG 2>&1 | tee -a valid_scan.log | tr -d '\n'
	if [ ${PIPESTATUS[0]} -eq 0 ]
	then
		echo -e -n " \033[0;32m[pass]"
		echo "[pass]" >> valid_scan.log
		((VALID_SCAN_CORRECT++))
	else
		echo -e -n " \033[0;31m[fail]"
		echo "[fail]" >> valid_scan.log
	fi 
	echo -e "\033[0m"
done 
echo
echo ">>>>> # valid scanner programs handled: ${VALID_SCAN_CORRECT}/${VALID_SCAN}"
echo
 
VALID_PARSE=0
VALID_PARSE_CORRECT=0
for PROG in $VALID_DIR_PARSE
do
	((VALID_PARSE++))

	echo -n "$PROG: " | tee -a valid_parse.log
	./run.sh $PROG 2>&1 | tee -a valid_parse.log | tr -d '\n'
	if [ ${PIPESTATUS[0]} -eq 0 ]
	then
		echo -e -n " \033[0;32m[pass]"
		echo "[pass]" >> valid_parse.log
		((VALID_PARSE_CORRECT++))
	else
		echo -e -n " \033[0;31m[fail]"
		echo "[fail]" >> valid_parse.log
	fi 
	echo -e "\033[0m"
done 
echo
echo ">>>>> # valid parse programs handled: ${VALID_PARSE_CORRECT}/${VALID_PARSE}"

# 3. Run all invalid programs
#
# For invalid programs, your compiler *MUST*
#   (a) output: INVALID: <error>
#   (b) exit with status code 1
# A log of the output is written to invalid.log

INVALID_DIR_SCAN=`find "grading/invalid/scanner/" -type f \( -name "*.go" ! -name "*.pretty.go" \)`
INVALID_DIR_PARSE=`find "grading/invalid/parser/" -type f \( -name "*.go" ! -name "*.pretty.go" \)`

echo
echo "*****************************"
echo "  Invalid programs"
echo "*****************************"

if [ -f invalid_scan.log ]
then
	rm invalid_scan.log
fi
if [ -f invalid_parse.log ]
then
	rm invalid_parse.log
fi

INVALID_SCAN=0
INVALID_SCAN_CORRECT=0
for PROG in $INVALID_DIR_SCAN
do
	((INVALID_SCAN++))
	
	echo -n "$PROG: " | tee -a invalid_scan.log
	./run.sh $PROG 2>&1 | tee -a invalid_scan.log | tr -d '\n'
	if [ ${PIPESTATUS[0]} -eq 1 ]
	then
		echo -e -n " \033[0;32m[pass]"
		echo "[pass]" >> invalid_scan.log
		((INVALID_SCAN_CORRECT++))
	else
		echo -e -n " \033[0;31m[fail]"
		echo "[fail]" >> invalid_scan.log
	fi 
	echo -e "\033[0m"
done 
echo
echo ">>>>> # invalid scanner programs handled: ${INVALID_SCAN_CORRECT}/${INVALID_SCAN}"
echo

INVALID_PARSE=0
INVALID_PARSE_CORRECT=0
for PROG in $INVALID_DIR_PARSE
do
	((INVALID_PARSE++))
	
	echo -n "$PROG: " | tee -a invalid_parse.log
	./run.sh $PROG 2>&1 | tee -a invalid_parse.log | tr -d '\n'
	if [ ${PIPESTATUS[0]} -eq 1 ]
	then
		echo -e -n " \033[0;32m[pass]"
		echo "[pass]" >> invalid_parse.log
		((INVALID_PARSE_CORRECT++))
	else
		echo -e -n " \033[0;31m[fail]"
		echo "[fail]" >> invalid_parse.log
	fi 
	echo -e "\033[0m"
done 
echo
echo ">>>>> # invalid parser programs handled: ${INVALID_PARSE_CORRECT}/${INVALID_PARSE}"

echo
echo "*****************************"
echo "  Overall Score"
echo "*****************************"
echo

echo ">>>>> # valid scanner programs handled: ${VALID_SCAN_CORRECT}/${VALID_SCAN}"
echo ">>>>> # invalid scanner programs handled: ${INVALID_SCAN_CORRECT}/${INVALID_SCAN}"
echo ">>>>> # valid parse programs handled: ${VALID_PARSE_CORRECT}/${VALID_PARSE}"
echo ">>>>> # invalid parser programs handled: ${INVALID_PARSE_CORRECT}/${INVALID_PARSE}"
