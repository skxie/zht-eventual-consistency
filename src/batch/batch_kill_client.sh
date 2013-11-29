COUNTER=21
while [ $COUNTER -lt 24 ]; do
	ssh hec-$COUNTER ./projects/ZHT/kill.sh benchmark_client
	ssh hec-$COUNTER ./projects/ZHT/kill.sh benchmark_client
	let COUNTER=COUNTER+1
done
