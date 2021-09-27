#!/bin/bash
echo "Killing 6dmimic_server"
pkill project1
pgrep --exact "project1"
status=$?

while [ $status -eq 0 ]; do
  pgrep --exact "project1"
  status=$?
  echo "Trying to kill. Aux: " + $status
  sleep 1
done
echo "Killed"
