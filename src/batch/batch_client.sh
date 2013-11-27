#!/bin/bash
COUNTER=6
while [ $COUNTER -lt 13 ]; do
        if [ $COUNTER -ne 10 ]; then
        if [ $COUNTER -ne 15 ]; then
        if [ $COUNTER -lt 10 ]; then
                nohup ssh hec-0$COUNTER 'bash -s' < run-client.sh > result/client-$COUNTER.out 2> result/client-error-$COUNTER.out &
        else
                nohup ssh hec-$COUNTER 'bash -s' < run-client.sh > result/client-$COUNTER.out 2> result/client-error-$COUNTER.out &
        fi
        fi
        fi
        let COUNTER=COUNTER+1
done
