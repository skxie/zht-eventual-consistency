COUNTER=21
while [ $COUNTER -lt 24 ]; do
	ssh hec-$COUNTER ./projects/ZHT/kill.sh zht_ben
	ssh hec-$COUNTER ./projects/ZHT/kill.sh zht_ben
	let COUNTER=COUNTER+1
done
