#!/bin/bash

for DIR in $1; do
    if [ ! -e $DIR ];then
        mkdir -p $DIR
    fi
done
if [ ! -e $2 ];then
    mkdir -p $2
fi