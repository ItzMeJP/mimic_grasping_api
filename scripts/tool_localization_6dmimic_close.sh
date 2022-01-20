#!/bin/bash
#echo "Killing 6dmimic_server"
#pkill SA
#pgrep --exact "SA"
#status=$?

#while [ $status -eq 0 ]; do
#  pgrep --exact "SA"
#  status=$?
#  echo "Trying to kill. Aux: " + $status
#  sleep 1
#done
#echo "Killed"
pkill mimic_void_proc
