#!/bin/bash

cd $(dirname $0)
cd ../Data/VRP/

g++ make_VRP_json.cpp -std=c++17 -o make_VRP_json.out

files="./*.txt"

echo $files

for file in $files; do
    ./make_VRP_json.out $file
done