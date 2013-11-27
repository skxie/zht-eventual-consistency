COUNTER=6
while [ $COUNTER -lt 24 ]; do
        if [ $COUNTER -ne 10 ]; then
        if [ $COUNTER -ne 15 ]; then
        if [ $COUNTER -lt 10 ]; then
                ssh hec-0$COUNTER ./projects/ZHT/kill.sh server_zht
                ssh hec-0$COUNTER ./projects/ZHT/kill.sh server_zht
        else
                ssh hec-$COUNTER ./projects/ZHT/kill.sh server_zht
                ssh hec-$COUNTER ./projects/ZHT/kill.sh server_zht
        fi
        fi
        fi
        let COUNTER=COUNTER+1
done
