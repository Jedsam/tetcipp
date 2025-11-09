#!/bin/bash
mkdir -p build
cd build
mkdir -p shaders
mkdir -p logs
cmake -DCMAKE_BUILD_TYPE=Debug -S ../ -B .
make && make Shaders && gdb ./tetcipp
cd ..
