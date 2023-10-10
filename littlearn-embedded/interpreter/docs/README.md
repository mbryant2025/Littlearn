# Interpreter

## Build


In the interpreter directory, run:

```bash
mkdir build
cd build
cmake ..
make
```

Alternatively, run `./build.sh` in the interpreter directory.

## TODO

* Negative literals
* Declaration without assignment
* Get rid of gatherTokensUntil, implement that logic in the BlockNode code
* Else blocks
* != and ==
* Print strings
* break, continue

For the hardware modules:

* write_seven_segment(int port, int value); //value is -999 through 9999
* write_led(int port, int value); //value is 0 or 1
* int val = read_switch(int port); //val is 0 or 1
* int val = detect_motion(int port); //val is 0 or 1
* int val = read_button(int port); //val is 0 or 1

  
