# Interpreter

This directory contains the source code for the Littlearn interpreter. The interpreter is written in C++ and is built using CMake. The interpreter is built for both desktop and embedded environments. The interpreter is built for desktop environments by default. To find more about the syntax of the language, see the `syntax.md` file in the `docs` directory.

## Build


In the interpreter directory, use the build script to build the interpreter.

Prior to running, navigate to `outputStream.hpp` to configure the build environment for embedded or desktop. This configures the program to output to stdout or to the hardware modules (serial).

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

## One-line samples

```c
{int n = 343; int count = 0; print(n); while (n > 1) { count = count + 1; if (n % 2) { n = 3 * n + 1; } else { n = n / 2; } print(n); wait(5); } print(count);}
```

```c
{int x = read_port(1); print(x); }
```

```c
{int count = 0; while(count < 69) {wait(75); if(read_port(1)) {count = count + 1; print_seven_segment(count); write_port(2,1); int tmp = count % 2; if(tmp){write_port(2,1);} if(1-tmp){write_port(2,0);}}}}
```

## Failsafe

Since code is stored on the brain between power cycles, it is possible for the brain to be in an invalid state. The system checks for invalid code upon boot, but it is possible for the ESP32 to crash if unexpected code is run. To prevent this, the system has a failsafe. This failsafe requires port 6 to be high on boot. In this case, it will ignore the code stored on the brain.



## TODO

* Interpreter
  * Declaration without assignment
  * Else blocks
  * != and ==
  * >= and <=
  * user defined functions
  * Time since start
  * Bug where breaks and continues nested do not work
  * Finish error handling
  * Error for using keyword as variable name
  * +=, -=, *=, /=, %=
  * Arrays
  * Check if we need to have error checking at the beginning of every function
  * Handle !
  * Remove error numbers
  * Templated returnable objects

* ESP32
  * Reconnect without restarting
  * Fix port 6 failsafe
  * Halt current program
  * Dont retokenize and parse until reupload

* Changes for blockly:
  * Make complicated expressions have correct ()
  * break, continue
  * Else blocks

* GUI:
  * Auto change connected status
  * Make code and console not expand
  * Console clear

* Long term
  * Electronically disconnect ports when uploading