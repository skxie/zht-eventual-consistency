#!/bin/bash
COUNTER=21
while [ $COUNTER -lt 24 ]; do
        nohup ssh hec-$COUNTER 'bash -s' < run-client.sh > result/client-$COUNTER.out 2> result/client-error-$COUNTER.out &
        let COUNTER=COUNTER+1
done
