#!/bin/bash

LOG=0
VERBOSE=0

while getopts ":lv" opt; do
	case $opt in
		l)
			LOG=1
			;;
		v)
			VERBOSE=1
			;;
		\?)
			echo "Invalid option: -$OPTARG" >&2
			exit
			;;
	esac
done

# 1. Build the compiler.
#
# You *MUST* provide a build.sh script in the root directory
# to build your compiler. If you do not produce a binary
# (i.e. something like python), then the build.sh script
# will be empty.
# A sample build.sh file using Makefile has been provided.

echo -n -e "\033[93m"
echo "*****************************"
echo "  Building compiler"
echo "*****************************"
echo -e -n "\033[0m"

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

SCORES=()

for DIR_TYPE in grading/*/; do
	CONF=`cat $DIR_TYPE/CONF`
	TYPE=$(basename $DIR_TYPE)
	TYPE="${TYPE^}"

	for DIR_PHASE in $DIR_TYPE*/; do
		PHASE=$(basename $DIR_PHASE)
		PHASE="${PHASE^}"

		echo -e "\033[93m"
		echo "*****************************"
		echo "  $TYPE $PHASE"
		echo "*****************************"
		echo -e -n "\033[0m"

		TESTS=`find $DIR_PHASE -type f \( -name "*.go" ! -name "*.pretty.go" \)`
		COUNT=0
		COUNT_CORRECT=0

		for TEST in $TESTS
		do
			((COUNT++))
			RESULT=$(./run.sh $TEST 2>&1)
			STATUS=${PIPESTATUS[0]}

			RESULT=${RESULT#$TEST}

			if [[ $RESULT == *"java.lang.NullPointerException"* ]]; then
				STATUS=-1
			fi

			if [ $STATUS -eq $CONF ]
			then
				if [ $VERBOSE -eq 1 ]
				then
					echo
					echo "$TEST: $RESULT" | tr -d '\n'
				       	echo -n -e " \033[0;32m[pass]\033[0m"
					if [ $LOG -eq 1 ]
					then
						echo "$TEST: $RESULT [pass]" >> ${TYPE}_${PHASE}.log
					fi
				fi
				((COUNT_CORRECT++))
			else
				echo
				echo "$TEST: $RESULT" | tr -d '\n'
				echo -n -e " \033[0;31m[fail]\033[0m"
				if [ $LOG -eq 1 ]
				then
					echo "$TEST: $RESULT [fail]" >> ${TYPE}_${PHASE}.log
				fi
			fi
		done
		if [ $VERBOSE -eq 1 ]
		then
			if [ $COUNT -gt 0 ]
			then
				echo
			fi
		else
			if [ $COUNT -ne $COUNT_CORRECT ]
			then
				echo
			fi
		fi
		echo
		echo -e "\e[41m# ${TYPE} ${PHASE}: ${COUNT_CORRECT}/${COUNT}\e[49m"
		SCORES+=("\e[41m# ${TYPE} ${PHASE}: ${COUNT_CORRECT}/${COUNT}\e[49m")
	done
done

echo -e "\033[93m"
echo "*****************************"
echo "  Overall"
echo "*****************************"
echo -e "\033[0m"

for i in ${!SCORES[*]}; do
	echo -e ${SCORES[$i]}
done
