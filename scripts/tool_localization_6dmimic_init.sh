#!/bin/bash

./setup_hearbit.sh&

cd ${MIMIC_HEARTBIT_CHECKER_ROOT}

./6dmimic_heartbit_checker&

#it is mandatory to put & in the last command, otherwise the finish function will not work

#./SA -p iilab -c sa_auto.conf &
#./SA -p default -c sa.conf &


# TODO:  try to automatic run the 6DMimic server!
#!/usr/bin/expect -f
#sudo apt install expect -y
#spawn ssh mimic@172.16.47.113
#expect "password:*"
#send "mimic\r"
#expect "$ "
#interact
