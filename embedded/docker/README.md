# Embedded Docker

## About

This directory contains the Dockerfile for the embedded systems. It is mainly used building and testing the interpreter. The below commands should be run from the `docker` folder from which the `Dockerfile` is located.

## Building

To build the Docker image, run the following command:

```bash
sudo docker build -t embedded-test-env .
```

## Running

To run the Docker image, run the following command:

```bash
docker run -it --rm -v $(dirname "$(pwd)"):/root/dev/embedded -v /dev:/dev embedded-test-env
```

Or, `source` the `enter.sh` script.