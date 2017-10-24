#!/bin/bash

for CSVFILE in $(ls *.tbl); do
	JSONFILE=${CSVFILE%.tbl}.json
	./csv2mongodbjson.py -c $CSVFILE -j $JSONFILE -d '|'
done

#mongoimport --db test --collection customer --file customer.json
