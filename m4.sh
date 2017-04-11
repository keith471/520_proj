#!/bin/bash

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

# first, compile all the programs into C++

SEMANTICS=`find "m4_tests/semantics/" -type f \( -name "*.go" ! -name "*.pretty.go" \)`

NUM_PROGS=0
NUM_CORRECT=0
for PROG in $SEMANTICS
do
    ((NUM_PROGS++))
    echo -n "$PROG: " | tee -a m4_log.log
    ./run.sh $PROG 2>&1 | tee -a m4_log.log | tr -d '\n'
    if [ ${PIPESTATUS[0]} -eq 0 ]
    then
        echo -e -n " \033[0;32m[pass]"
        echo "[pass]" >> m4_log.log
        ((NUM_CORRECT++))
    else
        echo -e -n " \033[0;31m[fail]"
        echo "[fail]" >> m4_log.log
    fi
    echo -e "\033[0m"
done
echo
echo ">>>>> # GoLite programs compiled to C++: ${NUM_CORRECT}/${NUM_PROGS}"
echo

# now, try to compile all the C++ files

CPP_FILES=`find "src/output/" -type f \( -name "*.cpp" \)`

NUM_CPP_PROGS=0
NUM_CPP_CORRECT=0
for PROG in $CPP_FILES
do
    ((NUM_CPP_PROGS++))
    echo -n "$PROG: " | tee -a m4_log.log
    g++ $PROG 2>&1 | tee -a m4_log.log | tr -d '\n'
    if [ ${PIPESTATUS[0]} -eq 0 ]
    then
        echo -e -n " \033[0;32m[pass]"
        echo "[pass]" >> m4_log.log
        ((NUM_CPP_CORRECT++))
    else
        echo -e -n " \033[0;31m[fail]"
        echo "[fail]" >> m4_log.log
    fi
    echo -e "\033[0m"
done
echo
echo ">>>>> # C++ programs compiled: ${NUM_CPP_CORRECT}/${NUM_CPP_PROGS}"
echo

echo
echo "*****************************"
echo "  Overall  "
echo "*****************************"
echo

echo ">>>>> # GoLite programs compiled to C++: ${NUM_CORRECT}/${NUM_PROGS}"
echo ">>>>> # C++ programs compiled: ${NUM_CPP_CORRECT}/${NUM_CPP_PROGS}"
