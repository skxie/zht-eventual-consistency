#!/bin/bash
COUNTER=21
while [ $COUNTER -lt 24 ]; do
	nohup ssh hec-$COUNTER 'bash -s' < run.sh > result/server-$COUNTER.out 2> result/server-error-$COUNTER.out &
	let COUNTER=COUNTER+1
done
