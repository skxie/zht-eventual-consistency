COUNTER=21
while [ $COUNTER -lt 24 ]; do
	ssh hec-$COUNTER ~/zht-eventual-consistency/src/batch/kill.sh server_zht
	ssh hec-$COUNTER ~/zht-eventual-consistency/src/batch/kill.sh server_zht
        let COUNTER=COUNTER+1
done
