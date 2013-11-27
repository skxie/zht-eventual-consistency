COUNTER=6
while [ $COUNTER -lt 23 ]; do
        if [ $COUNTER -ne 15 ]; then
        if [ $COUNTER -lt 10 ]; then
                ssh hec-0$COUNTER ./projects/ZHT/kill.sh benchmark_client
                ssh hec-0$COUNTER ./projects/ZHT/kill.sh benchmark_client
        else
                ssh hec-$COUNTER ./projects/ZHT/kill.sh benchmark_client
                ssh hec-$COUNTER ./projects/ZHT/kill.sh benchmark_client
        fi
        fi
        let COUNTER=COUNTER+1
done
