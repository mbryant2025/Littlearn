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

## One-line sample

```c
{int n = 343; int count = 0; print(n); while (n > 1) { count = count + 1; int temp = n % 2; if (temp - 1) { n = n / 2; } if (temp) { n = 3 * n; n = n + 1; } print_seven_segment(n); wait(5); } print_seven_segment(count); }
```

```c
{int x = read_port(1); print(x); }
```

```c
{int count = 0; while(count < 69) {wait(75); if(read_port(1)) {count = count + 1; print_seven_segment(count); write_port(2,1); int tmp = count % 2; if(tmp){write_port(2,1);} if(1-tmp){write_port(2,0);}}}}
```




## TODO

* Negative literals
* Declaration without assignment
* Get rid of gatherTokensUntil, implement that logic in the BlockNode code
* Else blocks
* != and ==
* Print strings
* break, continue

* Proper error handling

* enums

* check for max script size


  
