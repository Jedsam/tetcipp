#!/bin/bash
mkdir -p build
cd build
mkdir -p shaders
cmake -S ../ -B .
make && make Shaders && ./tetcipp
cd ..
