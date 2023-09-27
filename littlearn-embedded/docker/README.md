# Embedded Docker

## About

This directory contains the Dockerfiles for the embedded systems. It is mainly used building and testing the interpreter.

## Building

To build the Docker image, run the following command:

```bash
docker build -t littlearn-interpreter .
```

## Running

To run the Docker image, run the following command:

```bash
docker run -it --rm -v $(dirname "$(pwd)")/interpreter:/root/dev/interpreter littlearn-interpreter
```