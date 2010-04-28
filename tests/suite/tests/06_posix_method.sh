#!/bin/bash
#
# Test if adios POSIX method 
# Uses codes from SRCDIR/programs/posix_method
#
# Environment variables set by caller:
# MPIRUN        Run command
# MPIRUN_NP     Run commands option to set number of processes
# MAXPROCS      Max number of processes allowed
# HAVE_FORTRAN  yes or no
# SRCDIR        Test source dir (.. of this script)
# TRUNKDIR      ADIOS trunk dir

PROCS=3

if [ $MAXPROCS -lt $PROCS ]; then
    echo "WARNING: Needs $PROCS processes at least"
    exit 77  # not failure, just skip
fi

# copy codes and inputs to . 
cp $SRCDIR/programs/posix_method .
cp $SRCDIR/programs/posix_method.xml .

echo "Run posix_method"
$MPIRUN $MPIRUN_NP $PROCS ./posix_method
EX=$?
if [ $EX != 0 ]; then
    echo "ERROR: posix method failed with exit code=$EX"
    exit $EX
fi

for ((i=0; i<$PROCS; i++)); do
    if [ ! -f posix_method.bp.$i ]; then
        echo "ERROR: posix_method failed. No BP file posix_method.bp.$i is created."
        exit 1
    fi
done



