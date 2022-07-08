
cd ${MIMIC_GRASPING_SERVER_ROOT}/configs

profile=$(grep -o '"profile" : "[^"]*' general.json | grep -o '[^"]*$')

cd  ${MIMIC_GRASPING_SERVER_ROOT}/configs/$profile

file=$(curl "file:${MIMIC_GRASPING_SERVER_ROOT}/configs/$profile/localization_config.json" |\
        python -c 'import json,sys;obj=json.load(sys.stdin);print obj["tool_localization"]["configuration_file"]')

ip=$(curl "file:${MIMIC_GRASPING_SERVER_ROOT}/configs/$profile/$file" |\
        python -c 'import json,sys;obj=json.load(sys.stdin);print obj["listen_ip_address"]')

port=$(curl "file:${MIMIC_GRASPING_SERVER_ROOT}/configs/$profile/$file" |\
        python -c 'import json,sys;obj=json.load(sys.stdin);print obj["heartbit_port"]')

timeout=$(curl "file:${MIMIC_GRASPING_SERVER_ROOT}/configs/$profile/$file" |\
            python -c 'import json,sys;obj=json.load(sys.stdin);print obj["heartbit_timeout_in_seconds"]')

echo "Current IP: "$ip
echo "Heartbit Port: " $port
echo "Heartbit Timeout: " $timeout

cd ${MIMIC_HEARTBIT_CHECKER_ROOT}

echo "Current IP," $ip > config.config
echo "Listen Port," $port >> config.config
echo "Timeout [s]," $timeout >> config.config
