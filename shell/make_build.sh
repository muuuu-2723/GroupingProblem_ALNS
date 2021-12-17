#!/bin/bash

for DIR in $1; do
    if [ ! -e $DIR ];then
        mkdir $DIR
    fi
done
if [ ! -e $2 ];then
    mkdir $2
fi