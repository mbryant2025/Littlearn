# Interpreter

This directory contains the source code for the Littlearn interpreter. The interpreter is written in C++ and is built using CMake. The interpreter is built for both desktop and embedded environments. The interpreter is built for desktop environments by default. To find more about the syntax of the language, see the `syntax.md` file in the `docs` directory.

## Build


In the interpreter directory, use the build script to build the interpreter.

Prior to running, navigate to `error.hpp` to configure the build environment for embedded or desktop. This configures the program to output to stdout or to the hardware modules (serial).

```cpp
#define __EMBEDDED__ 0 // 0 for desktop, 1 for embedded
```

To build the executable:

```bash
./build.sh
```

## Test

To run the test suite, run the following script:

```bash
./test.sh
```

This will build and run the test suite. The test suite is located in the `test` directory.

## Debug

To build with debug, uncomment the following line in `cmakelists.txt`:

```cmake
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
```

Then, run the build script. To run Valgrind, enter:

```bash
valgrind --leak-check=full ./build/LittlearnInterpreter
```

Also ensure that the `main.cpp` file terminates (does not run in a loop) such that Valgrind can exit.


## TODO

* Negative literals
* Declaration without assignment
* Get rid of gatherTokensUntil, implement that logic in the BlockNode code
* Else blocks
* != and ==
* Print strings
* break, continue

* Proper error handling

For the hardware modules:

* write_seven_segment(int port, int value); //value is -999 through 9999
* write_led(int port, int value); //value is 0 or 1
* int val = read_switch(int port); //val is 0 or 1
* int val = detect_motion(int port); //val is 0 or 1
* int val = read_button(int port); //val is 0 or 1

  
