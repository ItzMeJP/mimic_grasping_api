#!/bin/bash

cd /home/joaopedro/lazarus_ws/6dmimic_void_proc
./mimic_void_proc &
#it is mandatory to put & in the last command, otherwise the finish function will not work

#./SA -p iilab -c sa_auto.conf &
#./SA -p default -c sa.conf &

#!/usr/bin/expect -f
#sudo apt install expect -y
#spawn ssh mimic@172.16.47.113
#expect "password:*"
#send "mimic\r"
#expect "$ "
#interact
