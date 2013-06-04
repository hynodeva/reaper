#!/bin/sh

echo "building debug"

mkdir debug
cp *.cl debug/
cp *.conf debug/
cd debug
cmake -D CMAKE_BUILD_TYPE=Debug ..
make

cd ..

echo "building release"
mkdir build
cp *.cl build/
cp *.conf build/
cd build
cmake -D CMAKE_BUILD_TYPE=Release ..
make
