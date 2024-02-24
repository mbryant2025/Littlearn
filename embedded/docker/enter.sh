#!/bin/bash


docker run -it --rm -v $(dirname "$(pwd)"):/root/dev/embedded -v /dev:/dev embedded-test-env