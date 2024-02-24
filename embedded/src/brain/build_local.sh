#!/bin/bash

build() {

    # Temporarily copy flags.h into the include directory
    cp ../flags/flags.h ./interpreter/include/flags.h

    cd local
    mkdir -p build
    cd build
    cmake ..
    make

    rm -f ./interpreter/include/flags.h
}

while getopts ":rvh" opt; do
  case $opt in
    r)
        build
        ./Brain
      ;;
    v)
        build
        valgrind --leak-check=full ./Brain
      ;;
    h)
        echo "Usage: ./build_local.sh -r -v -h"
        echo "Options:"
        echo "  -r  Run after build"
        echo "  -v  Run with valgrind after build"
        echo "  -h  Display this help"
        ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      echo "Please use -r to run after build or -v to run with valgrind" >&2
      ;;
  esac
done