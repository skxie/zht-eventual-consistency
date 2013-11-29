#!/bin/bash
COUNTER=30
while [ $COUNTER -lt 33 ]; do
        nohup ssh hec-$COUNTER 'bash -s' < run-client.sh > result/client-$COUNTER.out 2> result/client-error-$COUNTER.out &
        let COUNTER=COUNTER+1
done
