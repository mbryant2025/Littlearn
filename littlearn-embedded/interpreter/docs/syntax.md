# Littlearn Interpreter Syntax

## Introduction

The Littlearn interpreter is a simple interpreter for the custom [RickScript OR C-] language. It is written in C++ and handles the parsing and execution of the language. It is designed specifically for the Littlearn project and is to be run on a microcontroller. It can also be compiled on a computer for testing purposes. The language can control the I/O of the microcontroller and can be used to create simple programs.

## Sample program

To get a feel for the language, here is a sample program that will loop over the digits 0-9 and determine their sum. It is very similar to C. This code can be heavily optimized, but it is a good example of the syntax.

```c
int sum = 0;
int i = 0;
while (i < 10) {
    sum = sum + i;
    i = i + 1;
}
```

## Syntax

Snake case is recomended but not required. The language is case sensitive.

### Control flow

The language supports the following control flow statements:

- `if` statements

```c
if (condition) {
    // code
}
```

- `while` loops

```c
while (condition) {
    // code
}
```


### Variables

Variables can be declared and assigned to using the following syntax:

```c
int a = 5;
```

The following types are supported:

- `int` - 32 bit signed integer

```c
int a = 5;
```

- `float` - 32 bit floating point number

```c
float a = 5.0;
```

- `bool` - boolean value (true or false)

```c
bool a = true;
```

- `char` - 8 bit character

```c
char a = 'a';
```

- `string` - string of characters

```c
string a = "hello world";
```

- `array` - array of values with constant size. Under the hood this is a struct with a pointer to the array and a size value

```c
array<int> a = [1, 2, 3, 4, 5];
```

- `color` - color value (RGB), under the hood this is an int[3], includes built-in aliases for common colors (RED, GREEN, BLUE, WHITE, BLACK, YELLOW, ORANGE, PURPLE, PINK, CYAN, MAGENTA, BROWN, GRAY, DARK_GRAY, LIGHT_GRAY)

```c
color a = RED;
color b = [255, 0, 0];
```

### I/O

The language supports the following I/O statements:

- `print` - prints a value to stdout, with a newline

```c
print("hello world");
```

- `serial_print` - prints a value to the serial port, with a newline
    
```c
serial_print("hello world");
```


