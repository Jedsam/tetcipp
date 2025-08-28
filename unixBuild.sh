#!/bin/bash
mkdir -p build
cd build
mkdir -p shaders
mkdir -p logs
cmake -S ../ -B .
make && make Shaders && ./tetcipp
cd ..
