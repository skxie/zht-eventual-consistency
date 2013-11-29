COUNTER=30
while [ $COUNTER -lt 33 ]; do
	ssh hec-$COUNTER ~/zht-eventual-consistency/src/batch/kill.sh zhtserver
	ssh hec-$COUNTER ~/zht-eventual-consistency/src/batch/kill.sh zhtserver
        let COUNTER=COUNTER+1
done
