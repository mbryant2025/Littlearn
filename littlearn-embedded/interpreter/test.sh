#!/bin/bash

cd test
mkdir -p build
cd build
cmake ..
make

./LittlearnInterpreterTests
