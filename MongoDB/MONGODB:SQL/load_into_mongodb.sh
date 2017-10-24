#!/bin/bash

for JSONFILE in $(ls *.json); do
	COLLECTIONNAME=${JSONFILE%.json}
	mongoimport --db test --collection $COLLECTIONNAME --file $JSONFILE 
done
