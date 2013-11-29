#!/bin/bash
COUNTER=30
while [ $COUNTER -lt 33 ]; do
	nohup ssh hec-$COUNTER 'bash -s' < run.sh > result/server-$COUNTER.out 2> result/server-error-$COUNTER.out &
	let COUNTER=COUNTER+1
done
